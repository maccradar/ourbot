#ifndef HAL_INTERFACE_H
#define HAL_INTERFACE_H

#include "WProgram.h"
#include "hal_config.h"
#include "component_interface.h"
#include "hardware_include.h"

class HALInterface : public ComponentInterface
{
protected:
	#if HAL_INTERFACE_HBRIDGE_ENABLE > 0
		HBridgeInterface** _hbridges;
		const uint8_t _hbridge_count;
	#endif	
	#if HAL_INTERFACE_STEPPER_ENABLE > 0
		Stepper** _steppers;
		const uint8_t _stepper_count;
	#endif
	#if HAL_INTERFACE_LED_ENABLE > 0
		LED** _leds;
		const uint8_t _led_count;
	#endif
	#if HAL_INTERFACE_SENSOR_ENABLE > 0
		Sensor1D** _sensors;
		const uint8_t _sensor_count;
	#endif
	#if HAL_INTERFACE_IMU_ENABLE > 0
		IMUInterface** _imus;
		const uint8_t _imu_count;
	#endif
	
public:	
	HALInterface(uint8_t ID = 0);
	
	bool init();
	
	#if HAL_INTERFACE_HBRIDGE_ENABLE > 0
		HBridgeInterface* hbridgeID(uint8_t ID);
		HBridgeInterface* getHBridge(uint8_t index);
	#endif
	
	#if HAL_INTERFACE_STEPPER_ENABLE > 0
		Stepper* stepperID(uint8_t ID);
		Stepper* getStepper(uint8_t index);
	#endif
	
	#if HAL_INTERFACE_LED_ENABLE > 0
		LED* ledID(uint8_t ID);
		LED* getLed(uint8_t index);
		LED* onboardLed();
	#endif
	
	#if HAL_INTERFACE_SENSOR_ENABLE > 0
		Sensor1D* sensorID(uint8_t ID);
		Sensor1D* getSensor(uint8_t index);
	#endif
	
	#if HAL_INTERFACE_IMU_ENABLE > 0
		IMUInterface* imuID(uint8_t ID);
		IMUInterface* getImu(uint8_t index);
	#endif
	
	#ifdef HAL_INTERFACE_BATTERY_ENABLE
		Sensor1D *batteryVoltageSensor();
		int readBatteryVoltage();
		int peekBatteryVoltage();
	#endif
};

#endif //HAL_INTERFACE_H