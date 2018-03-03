/*
  ESP-8266-01 Bare minimum module
  SOC: Teensy 3.6 in 186 MHz

  Hardwire:
  Teensy 3.6 has 6 hardware serial ports and here Serial & Serial1 will be used
  ESP8266 ( TX ) <-> Teensy Serial ( PIN 0 ie. RX1 ) 
  ESP8266 ( RX ) <-> Teensy Serial 1 ( PIN 1 ie. TX1 )
  Yellow ( TX ) and Orange ( RX ) pins in ESP Chip image
  
  Note:
  The ESP8266 requires 3.3V power–do not power it with 5 volts!
  Redesigned the firmwire based on AT commands that work on Teensy 3.6 
  ESP AT Command list will be found in the doc section as pdf.

  Links:
	https://www.pjrc.com/teensy/td_uart.html
	https://forum.pjrc.com/threads/27850-A-Guide-To-Using-ESP8266-With-TEENSY-3
	https://alselectro.wordpress.com/2015/05/13/wifi-module-esp8266-2-tcp-client-server-mode/
	

  Author: NicoX (arch.smaitra@gmail.com)
  By: Invincible AI
*/

// Ensure this teensy library is used once.
#ifndef _ESP8266TEENSY_H_
#define _ESP8266TEENSY_H_


#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Stream.h>
#include <avr/pgmspace.h>
#include <IPAddress.h>

//AT+CWMODE : 1 = Client Station Mode, 2 = Access Point Mode, 3 = Dual Mode
#define ESP_CLIENT						0x01
#define ESP_ACCESSPOINT					0x02
#define ESP_DUAL						0x03

#define WIFI_MUX_SINGLE				0x00
#define WIFI_MUX_MULTI				0x01

// #define WIFI_TCP_DISABLE			0x00
// #define WIFI_TCP_ENABLE				0x01

// #define WIFI_CLIENT					0x00
// #define WIFI_SERVER					0x01

// #define WIFI_STATE_IDLE				0x00
// #define WIFI_STATE_UNAVAILABLE		0x01
// #define WIFI_STATE_SEND				0x02
// #define WIFI_STATE_CONNECT			0x03

// #define WIFI_NEW_NONE				0x00
// #define WIFI_NEW_MESSAGE			0x01
// #define WIFI_NEW_CONNECTED			0x02
// #define WIFI_NEW_DISCONNECTED		0x03
// #define WIFI_NEW_SEND_OK			0x04
// #define WIFI_NEW_SEND_ERROR			0x05
// #define WIFI_NEW_RESET				0x06
// #define WIFI_NEW_ALREADY_CONNECT	0x07
// #define WIFI_NEW_ETC				0x08

#define BUFFER_SIZE 1024
//Modified Library for Teensy 3.6 to use on
class ESP8266Teensy {

private:
	Stream *serial;
	Stream *serialDebug;
	bool isCheckSuccess;
	bool isDebug;
	int pinReset;
	bool TCPConnected;
	const char term[7] = "OK\r\n";

	//is RX Serial availability checking
	int available();

	//Reading
	String readData();
	String read();
	String readData(unsigned long timeout);
	String readTCPData();

	//Waiting
	char buffer[BUFFER_SIZE];
	void waitingForHardReset();
	void waitingForReset(unsigned long timeout);
	String waitingForJoin( unsigned long timeout );
	

	//Writing
	void writeOnESP(String str);
	void write(String str);
	void clearBuffer();
	void flush();
	bool setServerTimeout(int timeout);

public:
	ESP8266Teensy();
	
	//Utility
	String getValue(String data, char separator, int index);

	//Debuging
	void debugPrint(String str);	 							//Print general meggase 
	void debugPrintln(String str);	 							//Print general meggase in new line
	void debugPrintESP(String str);  							//Used to identify ESP response in new line

	//Connecting
	void start( Stream *serial, Stream *serialDebug, int pinReset, bool isDebug );
	bool setMode(int mode);
	bool setMux(int mux);
	void check();
	bool isCheckingSuccess();
	
	//Resetting
	void reset();												//Reset
	void hardReset(); 											//TODO: need to check if ready is being read correctly

	//Connecting to Access Point with SSID and Passcode 
	//ESP Mode: ESP_CLIENT / ESP_ACCESSPOINT/ ESP_DUAL
	String connectToAccessPoint(String ssid, String pass, int mode);

	//TCP Server Handling
	bool startTCPConnection(int channel, String ip, int port); 	//Start TCP Connection
	bool send(int channel, String message);						//Send Message to TCP Connection
	void closeTCPConnection(int id);							//Close TCP Connection

	bool startTCPServer(int port, int timeout);					//Start TCP server at port with timeout

	// bool closeTCPServer();
	// //bool isTCPEnabled();
	// void openAccessPoint(String ssid, String pass, int channel);
	
	// int isNewDataComing(byte type);
	// bool setAP(String ssid, String pass, int channel);
	// bool isNewAPSetting(String ssid, String pass, int channel);
	// int findChar(String str, int start, char c);
	//
};

#endif