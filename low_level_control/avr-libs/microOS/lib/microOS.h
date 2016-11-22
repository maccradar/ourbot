#ifndef MICROOS_H
#define MICROOS_H

#include "WProgram.h"
#include <Wire.h>
#include "microOS_config.h"
//#include "eeprom_m.h"
#include "hal.h"
#include "communicator_interface.h"
#include "thread.h"

/*
	All static members to ensure there is only one copy of the OS!
*/

class MicroOS
{
private:
	static uint8_t					_thread_count; 	
	static Thread**					_threads;
	
	static HAL*						_hal;
	static CommunicatorInterface*	_communicator;
	
	static int findThread(uint8_t ID);
	
public:
	MicroOS();		
	
	static uint8_t init(HAL* hal, CommunicatorInterface* communicator);
	static void start();
	static void run();
	
	static HAL*	hal();
	
	static uint8_t addThread(priority_t priority, uint32_t period, int (*Fcn)(), bool start, uint8_t ID=0);
	static uint8_t addThread(Thread *thread);
	static Thread *thread(uint8_t ID);
	//deleteThread();
	
#ifdef LEDCONTROL_ENABLE
	static int ledControl(void);
#endif
#ifdef STEPPERCONTROL_ENABLE
	static int stepperControl(void);
#endif
#ifdef COMMUNICATORCONTROL_ENABLE
	static int communicatorControl(void);
#endif
#ifdef BATTERYCONTROL_ENABLE
	static int batteryControl(void);
#endif
	
};

#endif //MICROOS_H
