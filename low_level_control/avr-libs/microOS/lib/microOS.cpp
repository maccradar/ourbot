#include "microOS.h"
#include <Wire.h>

uint8_t 				MicroOS::_thread_count = 0;
Thread**					MicroOS::_threads;
HAL* 					MicroOS::_hal;
CommunicatorInterface* 	MicroOS::_communicator;

#ifdef LEDCONTROL_ENABLE
int MicroOS::ledControl(void)
{
	for(uint8_t k=0;k<HAL_INTERFACE_LED_ENABLE;k++){
		_hal->getLed(k)->update();
	}
	return 0;
}
#endif

#ifdef STEPPERCONTROL_ENABLE
int MicroOS::stepperControl(void)
{
	for(uint8_t k=0;k<HAL_INTERFACE_STEPPER_ENABLE;k++){
	#ifdef STEPPERCONTROL_FIXEDTIME 
		_hal->getStepper(k)->update(STEPPERCONTROL_FIXEDTIME);
	#else
		_hal->getStepper(k)->update();
	#endif
	}
	return 0;
}
#endif

#ifdef COMMUNICATORCONTROL_ENABLE
int MicroOS::communicatorControl(void)
{
	_communicator->update();
	return 0;
}
#endif

#ifdef BATTERYCONTROL_ENABLE
int MicroOS::batteryControl(void)
{
	_hal->readBatteryVoltage();
	return 0;
}
#endif

uint8_t MicroOS::init(HAL* hal, CommunicatorInterface* communicator)
{
	#ifdef I2C_ENABLE
		Wire.begin();
	#endif

	_hal = hal;
	_hal->init();
	_communicator = communicator;
	_communicator->start();
	
	#ifdef LEDCONTROL_ENABLE
		_hal->onboardLed()->setSchedule(50, 1000);
		addThread(LEDCONTROL_PRIORITY, LEDCONTROL_TIME, &MicroOS::ledControl, false, LEDCONTROL_ID);
	#endif
	#ifdef STEPPERCONTROL_ENABLE
		addThread(STEPPERCONTROL_PRIORITY, STEPPERCONTROL_TIME, &MicroOS::stepperControl, false, STEPPERCONTROL_ID);
	#endif
	#ifdef COMMUNICATORCONTROL_ENABLE
		addThread(COMMUNICATORCONTROL_PRIORITY, COMMUNICATORCONTROL_TIME, &MicroOS::communicatorControl, false, COMMUNICATORCONTROL_ID);
	#endif
	#ifdef BATTERYCONTROL_ENABLE
		addThread(BATTERYCONTROL_PRIORITY, BATTERYCONTROL_TIME, &MicroOS::batteryControl, false, BATTERYCONTROL_ID);
	#endif
	
	return 0;
}

void MicroOS::start()
{
    for(uint8_t k=0;k<_thread_count;k++){
		_threads[k]->start();
	}
}

void MicroOS::run()
{
	for(uint8_t k=0;k<_thread_count;k++){
		if(_threads[k]->action())
			break;
	}
}

HAL* MicroOS::hal()
{
	return _hal;
}

int MicroOS::findThread(uint8_t ID)
{
	for(uint8_t k=0;k<_thread_count;k++){
		if(_threads[k]->getID()==ID){
			return k;
		}
	}
	
	return -1;
}


uint8_t MicroOS::addThread(priority_t priority, uint32_t period, int (*Fcn)(), bool start, uint8_t ID)
{	
    Thread *thread = new Thread(priority, period, Fcn);
    thread->setID(ID);	
    uint8_t newID = addThread(thread);
    
    if(start)
        thread->start();
        
    return newID;
}

uint8_t MicroOS::addThread(Thread *thread)
{
	uint8_t k = 0;
	bool IDtaken = false;
	
	// Check if the requested ID is still available
	if((thread->getID() == 0) || (findThread(thread->getID()) >= 0)){
		IDtaken = true;
	}
	
	// Search the list for an ID which has not yet been assigned
	while(IDtaken){
		thread->setID(thread->getID() + 1); 			//new search ID
		IDtaken = (findThread(thread->getID()) >= 0);	//test if it already exists
	}
	
	// Create new list of threads: one longer than before
	Thread **newThreads = new Thread*[_thread_count+1];
	
	// Add threads of higher priority first
	if(_thread_count > 0){
	    while((k < _thread_count) && (thread->getPriority() <= _threads[k]->getPriority())){
		    newThreads[k] = _threads[k];
		    k++;
	    }
	}
	
	// Add the new thread after all higher priority threads have taken their place
	newThreads[k] = thread;
	k++; _thread_count++;
	
	// Add all threads of lower priority
	while(k<_thread_count){
		newThreads[k] = _threads[k-1];
		k++;
	}
	
	// Delete the array with pointers to the threads and assign new array
	delete[] _threads;	
	_threads = newThreads;
	
	// Debug: list current thread ids
	/*Serial.println("Current thread IDs..");
	for(k=0;k<_thread_count;k++){
	    Serial.println(_threads[k]->getID());
	}*/
	
	// Return the ID of the newly added thread
	return thread->getID();
}

Thread *MicroOS::thread(uint8_t ID)
{
	uint8_t k = findThread(ID);

	if(k>0)
		return _threads[k];
	else
		return NULL;
}

