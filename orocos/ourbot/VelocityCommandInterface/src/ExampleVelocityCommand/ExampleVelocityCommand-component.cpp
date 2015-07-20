#include "ExampleVelocityCommand-component.hpp"
#include <rtt/Component.hpp>
#include <iostream>

ExampleVelocityCommand::ExampleVelocityCommand(std::string const& name) : VelocityCommandInterface(name){

}

void ExampleVelocityCommand::updateHook(){
  VelocityCommandInterface::updateHook();
}

ORO_LIST_COMPONENT_TYPE(ExampleVelocityCommand);
