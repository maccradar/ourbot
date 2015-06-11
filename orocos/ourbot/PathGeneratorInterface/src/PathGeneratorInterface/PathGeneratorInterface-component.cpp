#include "PathGeneratorInterface-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>

PathGeneratorInterface::PathGeneratorInterface(std::string const& name) : TaskContext(name, PreOperational), _est_pose(3){
  ports()->addPort("est_pose_inport", _est_pose_inport).doc("Estimated pose");

  ports()->addPort("ref_pose_x_outport", _ref_pose_outport[0]).doc("x reference");
  ports()->addPort("ref_pose_y_outport", _ref_pose_outport[1]).doc("y reference");
  ports()->addPort("ref_pose_t_outport", _ref_pose_outport[2]).doc("theta reference");

  ports()->addPort("ref_ffw_x_outport", _ref_ffw_outport[0]).doc("x ffw reference");
  ports()->addPort("ref_ffw_y_outport", _ref_ffw_outport[1]).doc("y ffw reference");
  ports()->addPort("ref_ffw_t_outport", _ref_ffw_outport[2]).doc("theta ffw reference");
}

bool PathGeneratorInterface::configureHook(){
  // Get configurator component and get parameters
  TaskContext* configurator = getPeer("configurator");
  if (configurator==NULL){
    log(Error) << "No peer configurator !" <<endlog();
    return false;
  }
  OperationCaller<int(void)> getPathLength = configurator->getOperation("getPathLength");
  if (!getPathLength.ready()){
    log(Error) << "No operation getPathLength of peer configurator !" <<endlog();
    return false;
  }
  OperationCaller<int(void)> getControlSampleRate = configurator->getOperation("getControlSampleRate");
  if (!getControlSampleRate.ready()){
    log(Error) << "No operation getControlSampleRate of peer configurator !" <<endlog();
    return false;
  }
  OperationCaller<int(void)> getPathUpdSampleRate = configurator->getOperation("getPathUpdSampleRate");
  if (!getPathUpdSampleRate.ready()){
    log(Error) << "No operation getPathUpdSampleRate of peer configurator !" <<endlog();
    return false;
  }
  OperationCaller<std::vector<double>(void)> getKinLimitations = configurator->getOperation("getKinLimitations");
  if (!getKinLimitations.ready()){
    log(Error) << "No operation getKinLimitations of peer configurator !" <<endlog();
    return false;
  }

  _path_length          = getPathLength();
  _control_sample_rate  = getControlSampleRate();
  _pathUpd_sample_rate  = getPathUpdSampleRate();
  _kin_limitations      = getKinLimitations();

  // Reserve required memory and initialize with zeros
  for(int i=0;i<3;i++){
    _ref_pose[i].resize(_path_length);
    _ref_ffw[i].resize(_path_length);
  }

  // Show example data sample to ports to make data flow real-time
  std::vector<double> example(_path_length, 0.0);
  for(int i=0; i<3; i++){
    _ref_pose_outport[i].setDataSample(example);
    _ref_ffw_outport[i].setDataSample(example);
  }

  return true;
}

bool PathGeneratorInterface::startHook(){
  if (!_est_pose_inport.connected()){
    log(Warning) << "_est_pose_inport not connected !" <<endlog();
  }
  if (!initialize()){
    log(Error) << "Error occured in initialize() !" <<endlog();
    return false;
  }
  std::cout << "PathGenerator started !" <<std::endl;
  return true;
}

void PathGeneratorInterface::updateHook(){
  // Read estimated pose
  std::vector<double> pose(3);
  if (_est_pose_inport.connected() && (_est_pose_inport.read(pose) == RTT::NewData)){
    _est_pose = pose;
  }

  // Update path
  pathUpdate();

  // Write path
  for (int i=0; i<3; i++){
    _ref_pose_outport[i].write(_ref_pose[i]);
    _ref_ffw_outport[i].write(_ref_ffw[i]);
  }

  log(Info) << "Path updated !" <<endlog();
}

int PathGeneratorInterface::getPathLength() { return _path_length; }
int PathGeneratorInterface::getControlSampleRate() { return _control_sample_rate; }
int PathGeneratorInterface::getPathUpdSampleRate() { return _pathUpd_sample_rate; }
std::vector<double> PathGeneratorInterface::getKinLimitations() { return _kin_limitations; }
std::vector<double> PathGeneratorInterface::getEstPose() { return _est_pose; }
void PathGeneratorInterface::setRefPose(std::vector<double> const& ref_pose_x, std::vector<double> const& ref_pose_y, std::vector<double> const& ref_pose_t){
 _ref_pose[0] = ref_pose_x;
 _ref_pose[1] = ref_pose_y;
 _ref_pose[2] = ref_pose_t;
}
void PathGeneratorInterface::setRefFfw(std::vector<double> const& ref_ffw_x, std::vector<double> const& ref_ffw_y, std::vector<double> const& ref_ffw_t){
 _ref_ffw[0] = ref_ffw_x;
 _ref_ffw[1] = ref_ffw_y;
 _ref_ffw[2] = ref_ffw_t;
}