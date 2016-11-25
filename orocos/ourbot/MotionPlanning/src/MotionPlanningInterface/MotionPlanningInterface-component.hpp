#ifndef OROCOS_MOTIONPLANNINGINTERFACE_COMPONENT_HPP
#define OROCOS_MOTIONPLANNINGINTERFACE_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>
#include <rtt/os/TimeService.hpp>
#include <rtt/Time.hpp>
#include <math.h>

using namespace RTT;
using namespace RTT::os;

typedef struct obstacle {
    std::vector<double> position;
    std::vector<double> velocity;
    std::vector<double> acceleration;
    std::vector<double> checkpoints;
    std::vector<double> radii;
    bool avoid;
} obstacle_t;

class MotionPlanningInterface : public RTT::TaskContext{
  private:
    InputPort<std::vector<double> > _target_pose_port;
    InputPort<std::vector<double> > _mp_trigger_port;
    InputPort<std::vector<double> > _obstacle_port;
    InputPort<std::vector<double> > _robobs_pose_port;
    InputPort<std::vector<double> > _robobs_velocity_port;
    OutputPort<std::vector<double> > _ref_pose_trajectory_port[3];
    OutputPort<std::vector<double> > _ref_velocity_trajectory_port[3];
    OutputPort<std::vector<double> > _ref_pose_trajectory_ss_port[3];
    OutputPort<bool> _valid_trajectories_port;

    TimeService::ticks _timestamp;
    void initObstacles();
    void computeObstacles();
    bool _got_target;
    bool _enable;
    std::vector<double> _mp_trigger_data;
    int _max_computation_periods;
    int _maximum_failures;
    bool _target_detection;

    int _failure_cnt;
    bool _first_iteration;
    bool _valid;

  protected:
    virtual bool trajectoryUpdate() = 0;
    virtual bool initialize() = 0;
    virtual bool config() = 0;
    virtual bool targetReached();
    std::vector<obstacle_t> _obstacles;

    int _trajectory_length;
    int _trajectory_length_tx;
    int _trajectory_length_full;
    int _tx_subsample;
    int _update_length;
    double _control_sample_rate;
    double _pathupd_sample_rate;
    double _horizon_time;
    double _sample_time;
    double _update_time;
    int _predict_shift;
    std::vector<double> _est_pose;
    std::vector<double> _target_pose;
    std::vector<double> _obstacle_data;
    std::vector<std::vector<double> > _ref_pose_trajectory;
    std::vector<std::vector<double> > _ref_velocity_trajectory;
    std::vector<std::vector<double> > _ref_pose_trajectory_ss;
    bool _ideal_prediction;
    int _n_obs;

  public:
    MotionPlanningInterface(std::string const& name);
    void setTargetPose(double, double, double);
    virtual bool configureHook();
    virtual bool startHook();
    virtual void updateHook();
    void writeSample();
    bool gotTarget();
    bool valid();
    void enable();
    void disable();
};
#endif
