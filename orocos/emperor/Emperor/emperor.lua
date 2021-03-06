require 'rttlib'
require 'rfsm'
require 'rfsm_rtt'
require 'rfsmpp'

local tc = rtt.getTC();
local fsm
local fqn_out, events_in

-- create properties
tc:addProperty(rtt.Property('int', 'print_level', 'Level of output printing'))
tc:addProperty(rtt.Property('double', 'reporting_rate', 'Frequency to take snapshots for the reporter'))

-- create/connect ports
emperor_fsm_event_port = rtt.InputPort('string')
emperor_send_event_port = rtt.OutputPort('string')
emperor_failure_event_port = rtt.OutputPort('string')
emperor_current_state_port = rtt.OutputPort('string')

gamepad_A_port = rtt.InputPort('bool')
gamepad_B_port = rtt.InputPort('bool')
gamepad_up_port = rtt.InputPort('bool')
gamepad_down_port = rtt.InputPort('bool')
gamepad_lb_port = rtt.InputPort('bool')

tc:addPort(emperor_fsm_event_port, 'emperor_fsm_event_port', 'Event port for driving the emperor FSM')
tc:addPort(emperor_send_event_port, 'emperor_send_event_port', 'Port to send events to the emperor FSM from the emperor')
tc:addPort(emperor_failure_event_port,'emperor_failure_event_port','Port to send indicate a failure in the emperor')
tc:addPort(emperor_current_state_port, 'emperor_current_state_port', 'current active state of the emperor FSM')
tc:addPort(gamepad_A_port, 'gamepad_A_port', 'A button of gamepad')
tc:addPort(gamepad_B_port, 'gamepad_B_port', 'B button of gamepad')
tc:addPort(gamepad_up_port, 'gamepad_up_port', 'Up button of gamepad')
tc:addPort(gamepad_down_port, 'gamepad_down_port', 'Down button of gamepad')
tc:addPort(gamepad_lb_port, 'gamepad_lb_port',  'LB button of gamepad')
emperor_send_event_port:connect(emperor_fsm_event_port)

local fsm

function configureHook()
   -- create local copies of the property values
   print_level = tc:getProperty('print_level'):get()
   reporting_rate = tc:getProperty('reporting_rate'):get()

   -- variables to use in updateHook
   communicator = tc:getPeer('communicator')
   gamepad = tc:getPeer('gamepad')
   camera = tc:getPeer('camera')
   reporter = tc:getPeer('reporter')
   communicator_update = communicator:getOperation('update')
   communicator_error = communicator:getOperation('inRunTimeError')
   gamepad_error = gamepad:getOperation('inRunTimeError')
   camera_error = camera:getOperation('inRunTimeError')

   reporter_snapshot = reporter:getOperation('snapshot')
   period = tc:getPeriod()
   snapshot_cnt = 1./(reporting_rate*period)

   -- load state machine
   fsm = rfsm.init(rfsm.load('Emperor/emperor_fsm.lua'))
   if not fsm then
      rtt.logl('Error','Could not initialize emperor state machine')
      return false
   end

  -- connect event ports to state machine
   fsm.getevents = rfsm_rtt.gen_read_str_events(emperor_fsm_event_port)
   rfsm.post_step_hook_add(fsm, rfsm_rtt.gen_write_fqn(emperor_current_state_port))

   if print_level >= 2 then
      -- enable state entry and exit dbg output
      fsm.dbg=rfsmpp.gen_dbgcolor('Emperor FSM', { STATE_ENTER=true, STATE_EXIT=true}, false)
      -- redirect rFSM output to rtt log
      fsm.info=function(...) rtt.logl('Info', table.concat({...}, ' ')) end
      fsm.warn=function(...) rtt.logl('Warning', table.concat({...}, ' ')) end
      fsm.err=function(...) rtt.logl('Error', table.concat({...}, ' ')) end
   end
   return true
end

function startHook()
   if not reporter:start() then
      rtt.logl('Error', 'Could not start reporter component!')
      return false
   end
   return true
end

function updateHook()
   -- update communication
   communicator_update()
   -- snapshot for reporter
   snapshot()
   -- error check
   if gamepad_error() then
      rtt.logl('Error', 'RunTimeError in gamepad component!')
      rfsm.send_events(fsm, 'e_failed')
      return
   end
   if camera_error() then
      rtt.logl('Error', 'RunTimeError in camera component!')
      rfsm.send_events(fsm, 'e_failed')
      return
   end
   if tc:inRunTimeError() then
      rtt.logl('Error', 'RunTimeError in emperor component!')
      rfsm.send_events(fsm, 'e_failed')
      return
   end
   if communicator_error() then
      rtt.logl('Error','RunTimeError in communicator component!')
      rfsm.send_events(fsm, 'e_failed')
      return
   end
   -- run state machine
   rfsm.run(fsm)
end

function stopHook()
   gamepad:stop()
   camera:stop()
   communicator:stop()
   reporter:stop()
end

function cleanupHook()
   rttlib.tc_cleanup()
end


function snapshot()
   if snapshot_cnt >= 1./(reporting_rate*period) then
      reporter_snapshot:send()
      snapshot_cnt = 1
   else
      snapshot_cnt = snapshot_cnt + 1
   end
end

