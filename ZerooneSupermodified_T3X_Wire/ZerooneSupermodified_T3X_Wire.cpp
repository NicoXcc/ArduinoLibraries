/*******************************************************************************
  ZerooneSupermodified_Due_Wire.cpp - ZeroOne Supermodified Controller API
  for Arduino Due (Wire interface only).
  Copyright (c) 2015 ZeroOne Mechatronics.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/

extern "C" {
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include "utility/zoSystemTimer.h"
    #include "utility/zoTypes.h"
  #include "utility/zoString.h"

}

#include "HardwareSerial.h"
#include "ZerooneSupermodified_T3X_Wire.h"

 

#include <i2c_t3.h>

#define WIRE_PINS   I2C_PINS_18_19
#if defined(__MKL26Z64__)               // LC
#define WIRE1_PINS   I2C_PINS_22_23
#endif
#if defined(__MK20DX256__)              // 3.1-3.2
#define WIRE1_PINS   I2C_PINS_29_30
#endif
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)  // 3.5/3.6
#define WIRE1_PINS   I2C_PINS_37_38
#define WIRE2_PINS   I2C_PINS_3_4
#endif
#if defined(__MK66FX1M0__)              // 3.6
#define WIRE3_PINS   I2C_PINS_56_57
#endif

//--Globals (needed for ISR access)---------------------------------------------
static volatile int _rs485ReDePin;

//--Initialize class variables--------------------------------------------------
volatile ZO_PROTOCOL_PACKET zoSms::BufferedPacket;
ZO_HW_ABSTRACTION zoSms::ha;
ZO_PROTOCOL_UART_DECODER_STATE zoSms::decoderState = WAIT_ON_HEADER_0;
volatile bool zoSms::i2cPacketReceived = false;
volatile bool zoSms::CommSuccess = true;
uint8_t zoSms::Warning = ZO_WARNING_NONE;

//--Cunstructors----------------------------------------------------------------
zoSms::zoSms(ZO_HW_TYPE hwType, int rs485ReDePin)

{
	CommSuccess = true;
	Warning = ZO_WARNING_NONE;
	ha.localNodeID = 1;
	ha.hw = hwType;
	ha.putPacket = putPacketSerial;
	ha.getPacket = getPacketSerial;	
	_rs485ReDePin = rs485ReDePin;
	decoderState = WAIT_ON_HEADER_0;
	i2cPacketReceived = false;
	
//if( _rs485ReDePin != -1 )
	//ha.ser->begin(57600,_rs485ReDePin);

	switch(hwType)
	{
	

#if defined(__MK66FX1M0__)
		case ZO_HW_SERIAL:
			ha.ser = &Serial1;
		break;
#endif
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)  		
		case ZO_HW_SERIAL1:
			ha.ser = &Serial1;
		break;
		
		case ZO_HW_SERIAL2:
			ha.ser = &Serial2;
		break;
		
		case ZO_HW_SERIAL3:
			ha.ser = &Serial3;
		break;
#endif
		
		case ZO_HW_WIRE:
			Wire.begin(ha.localNodeID);
			ha.putPacket = putPacketWire;
			ha.getPacket = getPacketWire;
			Wire.onReceive(wireRxHandler);
		break;
	}
	

}

		
	

zoSms::zoSms(ZO_HW_TYPE hwType)
{
	zoSms(hwType,-1);
}

zoSms::zoSms()
{
	zoSms(ZO_HW_SERIAL);
}


//--Public methods--------------------------------------------------------------


bool zoSms::getCommunicationSuccess()
{
	bool success;
	
	success = CommSuccess;				//store to local
	CommSuccess = true;					//initialize for next comm
	
	return success;
}

uint8_t zoSms::getWarning()
{
	uint8_t warn;
	
	warn = Warning;						//store to local
	Warning = ZO_WARNING_NONE;			//clear warning
	
	return warn;
}

void zoSms::setProfileAcceleration(uint8_t nodeId, uint32_t accel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x03;
	p.byteCount = 4;
	*((uint32_t*)p.data) = accel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setProfileConstantVelocity(uint8_t nodeId, uint32_t vel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x04;
	p.byteCount = 4;
	*((uint32_t*)p.data) = vel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setCurrentLimit(uint8_t nodeId, uint16_t curr)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x05;
	p.byteCount = 2;
	*((uint16_t*)p.data) = curr;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setDurationForCurrentLimit(uint8_t nodeId, uint16_t dur)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x06;
	p.byteCount = 2;
	*((uint16_t*)p.data) = dur;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setPIDgainP(uint8_t nodeId, uint16_t curr)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x00;
	p.byteCount = 2;
	u16ToStr(curr,p.data);
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}
void zoSms::setPIDgainI(uint8_t nodeId, uint16_t curr)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x01;
	p.byteCount = 2;
	u16ToStr(curr,p.data);
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setPIDgainD(uint8_t nodeId, uint16_t curr)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x02;
	p.byteCount = 2;
	u16ToStr(curr,p.data);
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}



void zoSms::moveWithVelocity(uint8_t nodeId,int32_t vel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x07;
	p.byteCount = 4;
	*((int32_t*)p.data) = vel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::moveToAbsolutePosition(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x08;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::moveToRelativePosition(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x09;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::profiledMoveWithVelocity(uint8_t nodeId, int32_t vel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0A;
	p.byteCount = 4;
	*((int32_t*)p.data) = vel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::profiledMoveToAbsolutePosition(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0B;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::profiledMoveToRelativePosition(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0C;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setVelocitySetpoint(uint8_t nodeId, int32_t vel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0D;
	p.byteCount = 4;
	*((int32_t*)p.data) = vel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setAbsolutePositionSetpoint(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0E;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setRelativePositionSetpoint(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x0F;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setProfiledVelocitySetpoint(uint8_t nodeId, int32_t vel)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x10;
	p.byteCount = 4;
	*((int32_t*)p.data) = vel;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setProfiledAbsolutePositionSetpoint(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x11;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setProfiledRelativePositionSetpoint(uint8_t nodeId, int64_t pos)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x12;
	p.byteCount = 8;
	*((int64_t*)p.data) = pos;
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::configureDigitalIOs(uint8_t nodeId,bool dio1,bool dio2,bool dio3)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x13;
	p.byteCount = 1;
	p.data[0]=0;
	if(dio1)
		p.data[0] |= 0x01;
	if(dio2)
		p.data[0] |= 0x02;
	if(dio3)
		p.data[0] |= 0x04;			
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setDigitalOutputs(uint8_t nodeId,bool do1,bool do2,bool do3)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x14;
	p.byteCount = 1;
	p.data[0]=0;
	if(do1)
		p.data[0] |= 0x01;
	if(do2)
		p.data[0] |= 0x02;
	if(do3)
		p.data[0] |= 0x04;			
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::setNodeID(uint8_t oldNodeId, uint8_t newNodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = oldNodeId;
	p.ownNodeID = 1;
	p.commandID = 0x15;
	p.byteCount = 1;
	p.data[0]=newNodeId;
			
	p.lrc = calcLRC(&p);
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::resetIncrementalPosition(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x18;
	p.byteCount = 0;
	p.lrc = 0x18;
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::start(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x19;
	p.byteCount = 0;
	p.lrc = 0x19;
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::halt(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x1A;
	p.byteCount = 0;
	p.lrc = 0x1A;
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::stop(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x1B;
	p.byteCount = 0;
	p.lrc = 0x1B;
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

void zoSms::resetErrors(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x1E;
	p.byteCount = 0;
	p.lrc = 0x1E;
	
	if( ha.putPacket(&p) )
		getResponse(&p);
}

uint32_t zoSms::getProfileAcceleration(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x67;
	p.byteCount = 0;
	p.lrc = 0x67;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint32_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

uint32_t zoSms::getProfileConstantVelocity(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x68;
	p.byteCount = 0;
	p.lrc = 0x68;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint32_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

uint16_t zoSms::getCurrentLimit(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x69;
	p.byteCount = 0;
	p.lrc = 0x69;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint16_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

uint16_t zoSms::getCurrentLimitDuration(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x6A;
	p.byteCount = 0;
	p.lrc = 0x6A;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint16_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

uint16_t zoSms::getPIDgainP(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;

	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x64;
	p.byteCount = 0;
	p.lrc = 0x64;

	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return strToU16(p.data);
		else
			return -1;
	}
	else
		return -1;
}
uint16_t zoSms::getPIDgainI(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;

	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x65;
	p.byteCount = 0;
	p.lrc = 0x65;

	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return strToU16(p.data);
		else
			return -1;
	}
	else
		return -1;
}
uint16_t zoSms::getPIDgainD(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;

	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x66;
	p.byteCount = 0;
	p.lrc = 0x66;

	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return strToU16(p.data);
		else
			return -1;
	}
	else
		return -1;
}


bool zoSms::getDigitalIOConfiguration(uint8_t nodeId, uint8_t dio)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x6B;
	p.byteCount = 0;
	p.lrc = 0x6B;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return(( p.data[0] & (1<<dio) ) == (1<<dio) );
		else
			return -1;
	}
	else
		return -1;
}

bool zoSms::getDigitalIn(uint8_t nodeId, uint8_t din)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x6D;
	p.byteCount = 0;
	p.lrc = 0x6D;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return(( p.data[0] & (1<<din) ) == (1<<din) );
		else
			return -1;
	}
	else
		return -1;
}

//uint16_t zoSms::getAnalogIn(uint8_t nodeId, uint8_t ain)
//{
	//ZO_PROTOCOL_PACKET p;
	
	//p.addressedNodeID = nodeId;
	//p.ownNodeID = 1;
	//p.commandID = 0x6E;
	//p.byteCount = 0;
	//p.lrc = 0x6E;
	
	//if( ha.putPacket(&p) )
	//{
		//if( getResponse(&p) )
			//return *( (uint16_t*) (p.data[(ain*2)-2]));
		//else
			//return -1;
	//}
	//else
		//return -1;
//}

uint16_t zoSms::getAnalogIn(uint8_t nodeId, uint8_t ain)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x6E;
	p.byteCount = 0;
	p.lrc = 0x6E;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return strToU16(&(p.data[(ain*2)-2]));
		else
			return -1;
	}
	else
		return -1;
}


int64_t zoSms::getPosition(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x6F;
	p.byteCount = 0;
	p.lrc = 0x6F;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((int64_t*)(p.data));
		else
			return -999;
	}
	else
		return -333;
}

uint16_t zoSms::getAbsolutePosition(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x70;
	p.byteCount = 0;
	p.lrc = 0x70;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint16_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

int32_t zoSms::getVelocity(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x71;
	p.byteCount = 0;
	p.lrc = 0x71;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((int32_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

uint16_t zoSms::getCurrent(uint8_t nodeId)
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = nodeId;
	p.ownNodeID = 1;
	p.commandID = 0x72;
	p.byteCount = 0;
	p.lrc = 0x72;
	
	if( ha.putPacket(&p) )
	{
		if( getResponse(&p) )
			return *((uint16_t*)(p.data));
		else
			return -1;
	}
	else
		return -1;
}

void zoSms::broadCastDoMove()
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = 0;
	p.ownNodeID = 1;
	p.commandID = 0xC8;
	p.byteCount = 0;
	p.lrc = 0xC8;
	
	ha.putPacket(&p);
}

void zoSms::broadcastStart()
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = 0;
	p.ownNodeID = 1;
	p.commandID = 0xC9;
	p.byteCount = 0;
	p.lrc = 0xC9;
	
	ha.putPacket(&p);
}

void zoSms::broadcastHalt()
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = 0;
	p.ownNodeID = 1;
	p.commandID = 0xCA;
	p.byteCount = 0;
	p.lrc = 0xCA;
	
	ha.putPacket(&p);
}

void zoSms::broadcastStop()
{
	ZO_PROTOCOL_PACKET p;
	
	p.addressedNodeID = 0;
	p.ownNodeID = 1;
	p.commandID = 0xCB;
	p.byteCount = 0;
	p.lrc = 0xCB;
	
	ha.putPacket(&p);
}

//--Private methods-------------------------------------------------------------
 bool zoSms::getResponse(ZO_PROTOCOL_PACKET* p)
{

	uint32_t startMillis = millis();
//	unsigned short int startMillis=millis();
    
	
	while( !(ha.getPacket(p)) )
	{
		if( (millis() - startMillis) > ZO_PROTOCOL_COMMAND_RESPONSE_TIMEOUT_MS)
		{
			CommSuccess = false;
			Warning = ZO_WARNING_RESPONSE_TIMEOUT;
			break;
	//	Serial.println(millis() - startMillis);
		}

	else CommSuccess = true; //Correct library when needing a reset error

	}

	if( CommSuccess )
	{	
		if( p->lrc != calcLRC(p) )
		{	
			CommSuccess = false;
			Warning = ZO_WARNING_WRONG_LRC;
		}
		
		if( p->commandID == ZO_PROTOCOL_ERROR_ID)
		{
			CommSuccess = false;
			Warning = p->data[0];
//			Serial.println(millis() - startMillis);
		}
	}

	return CommSuccess;

}

uint8_t zoSms::calcLRC(ZO_PROTOCOL_PACKET* p)
{
	uint8_t i;
	uint8_t lrc = 0;
	
	lrc ^= p->commandID;
	lrc ^= p->byteCount;
	
	for( i=0; i<p->byteCount; i++)
		lrc ^= p->data[i];

   return lrc;
}


bool zoSms::putPacketWire(const ZO_PROTOCOL_PACKET* packet)
{
	uint8_t i2cError = 0;
	uint8_t str[ZO_PROTOCOL_PACKET_SIZE];
	uint8_t i;

	str[0] = packet->addressedNodeID;
	str[1] = packet->ownNodeID;
	str[2] = packet->commandID;
	str[3] = packet->byteCount;
	
	for( i=0; i< packet->byteCount; i++ ){
		str[4+i] = packet->data[i];}

	str[(packet->byteCount + 4)] = packet->lrc;
	
	// Restart Wire as master. Arduino Due needs to be a master in order to transmit.
//	//Wire.begin();//
//	Wire.resetBus();
	Wire.beginTransmission(packet->addressedNodeID);
	Wire.write(str,packet->byteCount + 5);
	i2cError = Wire.endTransmission(true);

	// After the transmission, become a slave in order to receive.
	
	//

//	delay(200);

	if(i2cError == 0)
		return true;
	
	if(i2cError == 1)
	{
		CommSuccess = false;
		Warning = ZO_WARNING_WIRE_BUFFER_NOT_ENOUGH;
		return false;
	}
	
	if( (i2cError == 2) || (i2cError == 3) )
	{
		CommSuccess = false;
		Warning = ZO_WARNING_WIRE_NO_ACKNOIWLEDGE;
		return false;
	}
	
	if(i2cError == 4)
	{
		CommSuccess = false;
		Warning = ZO_WARNING_WIRE_BUS_ERROR;
		return false;
	}
	
	return false;
}


  void zoSms::wireRxHandler(size_t numbytes)
{
	uint8_t i;
	
	if(i2cPacketReceived)
	{
		CommSuccess = false;
		Warning = ZO_WARNING_WIRE_PACKET_OVERWRITTEN;
	}

//	Wire.requestFrom(BufferedPacket.addressedNodeID, numbytes, false); //????01

	BufferedPacket.addressedNodeID = Wire.readByte();
	BufferedPacket.ownNodeID = Wire.readByte();
	BufferedPacket.commandID = Wire.readByte();
	BufferedPacket.byteCount = Wire.readByte();

	for( i=0; i < BufferedPacket.byteCount ; i++ ){
		BufferedPacket.data[i] = Wire.readByte();	
	}
	BufferedPacket.lrc = Wire.readByte();

	//indicate we received a packet
	i2cPacketReceived = true;	
//Wire.endTransmission(true);
		

}

bool zoSms::getPacketWire(ZO_PROTOCOL_PACKET* packet)
{
	uint8_t i;

	if( i2cPacketReceived )
	{
		noInterrupts();
		
		//copy buffered packet
		packet->addressedNodeID = BufferedPacket.addressedNodeID;		
		packet->ownNodeID = BufferedPacket.ownNodeID;
		packet->commandID = BufferedPacket.commandID;
		packet->byteCount = BufferedPacket.byteCount;
		packet->lrc = BufferedPacket.lrc;
	
		for(i=0;i<packet->byteCount;i++)
			packet->data[i] = BufferedPacket.data[i];
		
		//indicate received packet was used
		i2cPacketReceived = false;
//---------------------------------------------------------------------------------------		
//---------------------------------------------------------------------------------------		


		interrupts();
		
		return true;
	}
	else
		return false;
}

bool zoSms::putPacketSerial(const ZO_PROTOCOL_PACKET* packet)
{
	uint8_t i;
	

	if( _rs485ReDePin != -1 ){
	ha.ser->flush();
	ha.ser->transmitterEnable(_rs485ReDePin);
	delay(5);
	}
	//put data on uart
	ha.ser->write(ZO_PROTOCOL_HEADER_0);
	
	ha.ser->write(ZO_PROTOCOL_HEADER_1);
	ha.ser->write(packet->addressedNodeID);
	ha.ser->write(packet->ownNodeID);
	ha.ser->write(packet->commandID);
	ha.ser->write(packet->byteCount);

	for(i=0;i<packet->byteCount;i++)
		ha.ser->write(packet->data[i]);
	ha.ser->write(packet->lrc);
	
	//Prepare to disable transmitter after transmit is done.
	//At this time data have been passed into UDRE and are shifted out one by 
	//one. This can take some time and we want to disable the transmitter 
	//exactly after transmission ends. 
	//The transmit complete interrupt will do just that.

// delay(50);	


	return true;	
}

bool zoSms::getPacketSerial(ZO_PROTOCOL_PACKET* packet)
{
	static uint8_t byteCount;
	bool isWholePacket = false;
	uint8_t c;
	
	if( ha.ser->peek() == -1)
		return false;
	else
		c = ha.ser->read();

	switch(decoderState) 
	{
		case WAIT_ON_HEADER_0:
			if ( c==ZO_PROTOCOL_HEADER_0 )
				decoderState = WAIT_ON_HEADER_1;
			else
				decoderState = WAIT_ON_HEADER_0;
			break;

		case WAIT_ON_HEADER_1:
			if( c == ZO_PROTOCOL_HEADER_1 )
				decoderState = WAIT_ON_ADDRESSED_NODE_ID;
			else
				decoderState = WAIT_ON_HEADER_0;
			break;

		case WAIT_ON_ADDRESSED_NODE_ID:
			if( c  == ha.localNodeID )
			{
				decoderState = WAIT_ON_OWN_NODE_ID;
				packet->addressedNodeID = c;
			}
			else
				decoderState = WAIT_ON_HEADER_0;
			break;

		case WAIT_ON_OWN_NODE_ID:
			packet->ownNodeID = c;
			decoderState = WAIT_ON_COMMAND_ID;
			break;

		case WAIT_ON_COMMAND_ID:
			packet->commandID = c;
			decoderState = WAIT_ON_BYTECOUNT;
			break;

		case WAIT_ON_BYTECOUNT:
			packet->byteCount = c;
			byteCount = packet->byteCount;	//store for internal use
			if(byteCount > 0)
				decoderState = WAIT_ON_DATA;
			else
				decoderState = WAIT_ON_LRC;
			break;

		case WAIT_ON_DATA:
			packet->data[packet->byteCount - byteCount--] = c;
			if(byteCount == 0)
				decoderState =	WAIT_ON_LRC;
			break;

		case WAIT_ON_LRC:
			packet->lrc = c;
			decoderState = WAIT_ON_HEADER_0; 
			isWholePacket = true;
			break;
	}

	return isWholePacket;		
}




//--ISRs------------------------------------------------------------------------
#if defined(__MK66FX1M0__)

	//Dont do anything
#else

	#if defined(__AVR_ATmega8__)
	SIGNAL(USART_TX_vect)
	#elif defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_AVR_LEONARDO) 
	SIGNAL(USART_TX_vect )
	#else
	SIGNAL(USART_TX_vect)
	#endif
	{
		digitalWrite(_rs485ReDePin,LOW);	//disable transmitter
	#if defined(__AVR_ATmega8__)
		UCSRB &= ~_BV(TXCIE);			//disable transmit complete interrupt
	#elif defined(ARDUINO_AVR_MICRO) || defined(ARDUINO_AVR_LEONARDO) 
		UCSR1B &= ~_BV(TXCIE1);			//disable transmit complete interrupt
	#else
		UCSR0B &= ~_BV(TXCIE0);			//disable transmit complete interrupt
	#endif
	}
	//    UCSR1B=|(1<<TXCIE1);
#endif