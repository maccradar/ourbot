#ifndef OROCOS_VELOCITYCOMMANDINTERFACE_COMPONENT_HPP
#define OROCOS_VELOCITYCOMMANDINTERFACE_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>

using namespace RTT;

class VelocityCommandInterface : public RTT::TaskContext{
  private:
    OutputPort<std::vector<double> > _cmd_velocity_port;

    std::vector<double> _cmd_velocity;

  protected:
    void setCmdVelocity(std::vector<double> const&);

  public:
    VelocityCommandInterface(std::string const& name);
    void setVelocity(double, double, double);
    virtual bool configureHook();
    virtual bool startHook();
    virtual void updateHook();
    virtual void stopHook();
    virtual void cleanupHook();
};
#endif
