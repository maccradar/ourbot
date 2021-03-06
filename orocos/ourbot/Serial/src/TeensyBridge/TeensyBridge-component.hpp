#ifndef TEENSYBRIDGE_H
#define TEENSYBRIDGE_H

//#define TEENSYBRIDGE_TESTFLAG
//#define TEENSYBRIDGE_DEBUGFLAG

#ifdef TEENSYBRIDGE_DEBUGFLAG
	#define TEENSYBRIDGE_DEBUG_PRINT(x)	std::cout << x << std::endl;
#else
	#define TEENSYBRIDGE_DEBUG_PRINT(x)	//std::cout << x << std::endl;
#endif

#include "../USBInterface/USBInterface-component.hpp"
#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>
#include <rtt/Component.hpp>
#include <vector>
#include "protocol_mavlink.h"
#include "IMU.hpp"

#define TEENSYBRIDGE_CONTROL_MODE_SIMPLE		0
#define TEENSYBRIDGE_CONTROL_MODE_INDIVIDUAL	1

#define TEENSYBRIDGE_IMU_LEFT_ID				0
#define TEENSYBRIDGE_IMU_RIGHT_ID				1

typedef std::vector<RTT::base::PortInterface*> Ports;

class TeensyBridge : public USBInterface
{
	private:
		// MAVLINK MESSAGE HANDLERS
		ProtocolMavlink	_protocol;
		mavlink_motor_state_t	_motor_states[4];
		mavlink_raw_imu_data_t	_raw_imu_data[2];
		mavlink_gpio_t			_gpio;
		//mavlink_threadtime_t	_threadtime;
		void writeRawDataToPorts();

		// KINEMATIC HANDLERS
		std::vector<double> _ourbot_size;
		double _wheel_radius;
		uint32_t _encoder_ticks_per_revolution;
		double _current_sensor_gain;
		int _current_sensor_offset;

		double _kinematic_conversion_position;
		double _kinematic_conversion_orientation;
		double _kinematic_conversion_wheel;
		std::vector<double>	_pose;
		std::vector<double>	_velocity;

		uint8_t _control_mode;

		std::vector<double> _velocity_pid_soft;
		std::vector<double> _velocity_pid_strong;
		std::vector<double> _max_velocity;

		RTT::InputPort<std::vector<double> >  _cmd_velocity_port;
		RTT::OutputPort<std::vector<double> > _cmd_velocity_passthrough_port;
		RTT::OutputPort<std::vector<double> > _cal_enc_pose_port;
		RTT::OutputPort<std::vector<double> > _cal_velocity_port;
		RTT::OutputPort<std::vector<double> > _raw_enc_ticks_port;
		RTT::OutputPort<std::vector<double> > _raw_enc_speed_port;
		RTT::OutputPort<std::vector<double> > _raw_enc_cmd_speed_port;
		RTT::OutputPort<std::vector<double> > _cal_motor_voltage_port;
		RTT::OutputPort<std::vector<double> > _cal_motor_current_port;
		RTT::OutputPort<std::vector<double> > _debug_port;
		RTT::OutputPort<double> _cal_trailer_angle_port;

		bool action(mavlink_message_t msg);
		void setMotorReference(int reference, int ID, int mode);
		void setController(uint8_t controllerID, double P, double I, double D);

		void recalculatePose();
		void recalculateVelocity();
		double saturate(double v, double max);

		// IMU HANDLERS
		IMU* _imus[2]; //array of imus
		void updateIMU(uint8_t ID);
		IMU* findIMU(uint8_t ID);
		void addIMUPorts(TaskContext* comp, const std::string insertion, const int i);

	public:
		TeensyBridge(std::string const& name);

		bool configureHook();
		bool startHook();
		void stopHook();
		void updateHook();

		uint32_t getPacketsDropped();
		uint32_t getPacketsReceived();
		std::vector<double> getPose();
		std::vector<double> getVelocity();

		//Commands
		void setControlMode(uint8_t control_mode);
		void setMotorVelocity(double rpm, int ID);
		void setMotorCurrent(double current, int ID);
		void setMotorVoltage(double voltage, int ID);
		void setVelocity(double vx, double vy, double w);
		void setCurrentController(double P, double I, double D);
		void setVelocityController(double P, double I, double D);
		void softVelocityControl();
		void strongVelocityControl();
		void showCurrents();
		void showVelocities();
		void showEncoders();
		void showPositions();
		void showMotorState(int ID);
		void showDebug();
		void showThreadTime();
		void showIMUData(int ID);
		void showTrailerAngle();
};

#endif //TEENSYBRIDGE_H
