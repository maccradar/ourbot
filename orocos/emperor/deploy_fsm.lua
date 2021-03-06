require 'rttlib'
require 'rfsm_rtt'
require 'rfsmpp'

-- components to load
local components_to_load = {
  emperor = 'OCL::LuaTLSFComponent',
  gamepad = 'GamePad',
  communicator = 'Communicator',
  camera = 'Camera',
  reporter = 'OCL::NetcdfReporting'
}

-- ports to report
local ports_to_report = {
  gamepad = {'cmd_velocity_port'}
}

-- packages to import
local packages_to_import = {'Serial', 'Camera', 'Communicator'}

-- configuration files to load
local system_config_file = 'Configuration/system-config.cpf'
local component_config_files = {
  communicator = '../ourbot/Configuration/communicator-config.cpf',
  reporter = 'Configuration/reporter-config.cpf',
  camera = 'Configuration/camera-config.cpf',
  gamepad = 'Configuration/gamepad-config.cpf'
}

local components = {}
local dp = rtt.getTC():getPeer('Deployer')

return rfsm.state {

  rfsm.transition {src = 'initial', tgt = 'deploy'},
  rfsm.transition {src = 'deploy', tgt = 'failure', events = {'e_failed'}},
  rfsm.transition {src = 'deployed', tgt = 'failure', events = {'e_failed'}},
  rfsm.transition {src = '.deploy.load_emperor', tgt = 'deployed', events = {'e_done'}},
  rfsm.transition {src = 'failure', tgt = 'deploy', events = {'e_reset'}},

  initial = rfsm.conn{},
  deployed = rfsm.conn{},

  failure = rfsm.state {
    entry = function()
      _deployer_failure_event_port:write('e_failed')
      components.communicator:update()
    end,

    doo = function()
      local cnt = 0
      while (cnt <= 3) do
        cnt = cnt+1
      end
      dp:stopComponents()
    end,
   },

  deploy = rfsm.state {
    rfsm.transition {src = 'initial', tgt = 'load_components'},
    rfsm.transition {src = 'load_components', tgt = 'configure_components', events = {'e_done'}},
    rfsm.transition {src = 'configure_components', tgt = 'connect_components', events = {'e_done'}},
    rfsm.transition {src = 'connect_components', tgt = 'connect_remote_components', events = {'e_done'}},
    rfsm.transition {src = 'connect_remote_components', tgt = 'set_activities', events = {'e_done'}},
    rfsm.transition {src = 'set_activities', tgt = 'prepare_reporter', events = {'e_done'}},
    rfsm.transition {src = 'prepare_reporter', tgt = 'load_emperor', events = {'e_done'}},

    initial = rfsm.conn{},

    load_components = rfsm.state {
      entry = function(fsm)
        components = {}
        -- import necessary packages
        for _, package in pairs(packages_to_import) do
          if not rtt.provides("ros"):import(package) then rfsm.send_events(fsm,'e_failed') return end
        end
        -- go through the table of components to load them
        for name, type in pairs(components_to_load) do
          if not dp:loadComponent(name, type) then rfsm.send_events(fsm,'e_failed') return end
          components[name] = dp:getPeer(name)
        end
        -- add all components as peer of emperor
        for name, comp in pairs(components) do
          if (name ~= 'emperor') then
            if not dp:addPeer('emperor', name) then rfsm.send_events(fsm,'e_failed') return end
          end
        end
        -- load execution file in emperor component
        if not components.emperor:exec_file(emperor_file) then rfsm.send_events(fsm,'e_failed') return end
      end
    },


    configure_components = rfsm.state {
      entry = function(fsm)
        for name, comp in pairs(components) do
          if (name ~= 'reporter' and name ~= 'emperor') then -- reporter configured in later stage
            comp:loadService('marshalling')
            -- every component loads system configurations
            if not comp:provides('marshalling'):updateProperties(system_config_file) then rfsm.send_events(fsm,'e_failed') return end
            -- if available, a component loads its specific configurations
            if(component_config_files[name]) then
              if not comp:provides('marshalling'):updateProperties(component_config_files[name]) then rfsm.send_events(fsm,'e_failed') return end
            end
            -- configure the component
            if not comp:configure() then rfsm.send_events(fsm,'e_failed') return end
          end
        end
      end,
    },

    connect_components = rfsm.state {
      entry = function(fsm)
        -- connect the deployer to emperor (for communicating failure events)
        if not _deployer_fsm_event_port:connect(components.emperor:getPort('emperor_failure_event_port')) then rfsm.send_events(fsm,'e_failed') return end
        if not _deployer_failure_event_port:connect(components.emperor:getPort('emperor_fsm_event_port')) then rfsm.send_events(fsm,'e_failed') return end
        if not dp:connectPorts('emperor', 'gamepad') then rfsm.send_events(fsm, 'e_failed') return end
      end,
    },

    connect_remote_components = rfsm.state {
      entry = function(fsm)
        -- add every component as peer of communicator
        for name, comp in pairs(components) do
          if (name ~= 'communicator') then
            if not dp:addPeer('communicator', name) then rfsm.send_events(fsm,'e_failed') return end
          end
        end
        if not dp:addPeer('communicator', 'lua') then rfsm.send_events(fsm,'e_failed') return end
        -- load operations
        local addIncoming = components.communicator:getOperation('addIncomingConnection')
        local addOutgoing = components.communicator:getOperation('addOutgoingConnection')
        -- emperor
        if not addOutgoing('emperor', 'emperor_send_event_port', 'fsm_event', 'ourbots') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('emperor', 'emperor_fsm_event_port', 'fsm_event') then rfsm.send_events(fsm,'e_failed') return end
        -- gamepad
        if not addOutgoing('gamepad', 'cmd_velocity_port', 'cmd_velocity', 'ourbots') then rfsm.send_events(fsm,'e_failed') return end
        -- camera
        if not addOutgoing('camera', 'target_out_port', 'target_out', 'ourbots') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'target_in_port', 'target_in') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot0_est_pose_port', 'est_pose_dave') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot1_est_pose_port', 'est_pose_krist') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot2_est_pose_port', 'est_pose_kurt') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot0_ref_x_port', 'ref_x_dave') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot1_ref_x_port', 'ref_x_krist') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot2_ref_x_port', 'ref_x_kurt') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot0_ref_y_port', 'ref_y_dave') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot1_ref_y_port', 'ref_y_krist') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('camera', 'robot2_ref_y_port', 'ref_y_kurt') then rfsm.send_events(fsm,'e_failed') return end
        -- deployer
        if not addOutgoing('lua', 'deployer_failure_event_port', 'deployer_event', 'ourbots') then rfsm.send_events(fsm,'e_failed') return end
        if not addIncoming('lua', 'deployer_fsm_event_port', 'deployer_event') then rfsm.send_events(fsm, 'e_failed') return end
      end,
    },

    set_activities = rfsm.state {
      entry = function(fsm)
        dp:setActivity('emperor', 1./emperor_rate, 0, rtt.globals.ORO_SCHED_OTHER)
        dp:setActivity('gamepad', 1./velcmd_rate, 0, rtt.globals.ORO_SCHED_OTHER)
        dp:setActivity('camera', 1./camera_rate, 0, rtt.globals.ORO_SCHED_OTHER)
        dp:setActivity('reporter', 0, 0, rtt.globals.ORO_SCHED_OTHER)
        dp:setMasterSlaveActivity('emperor', 'communicator')
          --add here extra activities
      end,
    },

    prepare_reporter = rfsm.state {
      entry = function(fsm)
        -- load the mashalling service and the configuration file
        components.reporter:loadService('marshalling')
        components.reporter:provides('marshalling'):loadProperties(component_config_files['reporter'])
        -- add components to report
        for comp, portlist in pairs(ports_to_report) do
          for i, port in pairs(portlist) do
            if not dp:addPeer('reporter', comp) then rfsm.send_events(fsm,'e_failed') end
            if not components.reporter:reportPort(comp, port) then rfsm.send_events(fsm,'e_failed') end
          end
        end
        -- configure the reporter
        if not components.reporter:configure() then rfsm.send_events(fsm, 'e_failed') return end
      end
    },

    load_emperor = rfsm.state {
      entry = function(fsm)
        -- emperor loads system configurations
        components.emperor:loadService('marshalling')
        if not components.emperor:provides('marshalling'):updateProperties(system_config_file) then rfsm.send_events(fsm,'e_failed') return end
        -- configure the emperor
        if not components.emperor:configure() then rfsm.send_events(fsm,'e_failed') return end
        -- load the local application script
        components.emperor:loadService("scripting")
        if not components.emperor:provides("scripting"):loadPrograms(app_file) then rfsm.send_events(fsm,'e_failed') return end
        -- start the emperor, gamepad and camera
        if not components.emperor:start() then rfsm.send_events(fsm,'e_failed') return end
        if not components.communicator:start() then rfsm.send_events(fsm, 'e_failed') return end
        components.gamepad:start() -- no failure when not connected
        if not components.camera:start() then rfsm.send_events(fsm, 'e_failed') return end
      end,
    },
  },
}
