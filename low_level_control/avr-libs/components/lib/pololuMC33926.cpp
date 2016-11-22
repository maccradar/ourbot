#include "pololuMC33926.h"

PololuMC33926::PololuMC33926(uint8_t in1, uint8_t in2, Sensor1D *battery_voltage_sensor, uint8_t ID) :
	HBridgeInterface(battery_voltage_sensor, ID), _in1(in1), _in2(in2), _D1(0xFF), _D2(0xFF)
{
	//do nothing
}

PololuMC33926::PololuMC33926(uint8_t in1, uint8_t in2, int battery_voltage, uint8_t ID) :
	HBridgeInterface(battery_voltage, ID), _in1(in1), _in2(in2), _D1(0xFF), _D2(0xFF)
{

	//do nothing
}

PololuMC33926::PololuMC33926(uint8_t in1, uint8_t in2, uint8_t D1, uint8_t D2, Sensor1D *battery_voltage_sensor, uint8_t ID) :
	HBridgeInterface(battery_voltage_sensor, ID), _in1(in1), _in2(in2), _D1(D1), _D2(D2)
{
	//do nothing
}

PololuMC33926::PololuMC33926(uint8_t in1, uint8_t in2, uint8_t D1, uint8_t D2, int battery_voltage, uint8_t ID) :
	HBridgeInterface(battery_voltage, ID), _in1(in1), _in2(in2), _D1(D1), _D2(D2)
{
	//do nothing
}

bool PololuMC33926::init()
{
	pinMode(_in1, OUTPUT);
	pinMode(_in2, OUTPUT);
	if((_D1&_D2)==0xFF){
		analogWriteFrequency(_in1, 20000); //only works when compiling for teensy... TODO: set teensy flag
		analogWriteFrequency(_in2, 20000); //only works when compiling for teensy... TODO: set teensy flag
	}
	
	if(_D1!=0xFF){
		pinMode(_D1, OUTPUT);
		analogWriteFrequency(_D1, 20000);
	}
	else if(_D2!=0xFF){
		pinMode(_D2, OUTPUT);
		analogWriteFrequency(_in1, 20000);
	}
		
	analogWriteResolution(8);
	return _battery_voltage_sensor->init();
}

void PololuMC33926::setOutputs()
{
	if((_D1&_D2)==0xFF){
		if(_polarity){
			analogWrite(_in1, _pwm);
			analogWrite(_in2, 0);
		} else {
			analogWrite(_in1, 0);
			analogWrite(_in2, _pwm);
		}
	} else {
		digitalWrite(_in1, _polarity);
		digitalWrite(_in2, !_polarity);
		if(_D1!=0xFF){
			analogWrite(_D1, _pwm);
		} else {
			analogWrite(_D2, _pwm);
		}
	}
}




