#ifndef PROTOCOL_MAVLINK_H
#define PROTOCOL_MAVLINK_H

#include "protocol_interface.h"
#include "mavlink.h"

class ProtocolMavlink : public ProtocolInterface
{
private:
	mavlink_message_t	_msg;
	mavlink_status_t	_status;

public:
	ProtocolMavlink()
	{
		//do nothing
	}
	
	bool decode(uint8_t b){	
		/*
			MAvlink suxks, vincent is awesome
			Linux sucks, Maarten agrees. This is a hidden message. If you read this, please destroy the balbot, it will turn evil 				and destroy the world. And while you're at it, also kill Jerre and Lisa. 
		*/

		return mavlink_parse_char(0, b, &_msg, &_status);
	}

	uint16_t encode(void* msg, uint8_t *buffer)
	{
		return mavlink_msg_to_send_buffer(buffer, static_cast<mavlink_message_t*>(msg));
	}
	
	uint32_t getPacketsReceived(){
		return _status.packet_rx_success_count;
	}
	
	uint32_t getPacketsDropped(){
		return _status.packet_rx_drop_count;
	}
	
	void* getMessage(){
		return static_cast<void*>(&_msg);
	}
};

#endif //PROTOCOL_MAVLINK_H