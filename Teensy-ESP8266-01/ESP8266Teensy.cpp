/*
  ESP-8266-01 Bare minimum module
  SOC: Teensy 3.6 in 186 MHz

  Note:
  The ESP8266 requires 3.3V power–do not power it with 5 volts!
  Redesigned the firmwire based on AT commands to work on Teensy 3.6
  https://alselectro.wordpress.com/2015/05/13/wifi-module-esp8266-2-tcp-client-server-mode/
  ESP AT Command list will be found in the doc section as pdf

  Author: NicoX (arch.smaitra@gmail.com)
  By: Invincible AI

  test ====> check
  this->serial ====> this->serial
  begin  ====>  start
  
*/

#include "ESP8266Teensy.h"


ESP8266Teensy::ESP8266Teensy() {

}

//Utility
String ESP8266Teensy::getValue(String data, char separator, int index){
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;
  
	for(int i=0; i<=maxIndex && found<=index; i++){
	  if(data.charAt(i)==separator || i==maxIndex){
		  found++;
		  strIndex[0] = strIndex[1]+1;
		  strIndex[1] = (i == maxIndex) ? i+1 : i;
	  }
	}
  
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
  }



//Debug Printing
void ESP8266Teensy::debugPrint(String str) {
	if(this->isDebug)
		serialDebug->print(str);
}

void ESP8266Teensy::debugPrintln(String str) {
	if(this->isDebug) {
		//serialDebug->println("__|¨¨|__|¨¨|__|¨¨|___|¨¨|____|¨¨|___|¨¨|_|¨¨|__|¨¨|_|¨¨|_|¨¨|_|¨¨|__");
		serialDebug->println(""); serialDebug->print("[Teensy 3.6] "); serialDebug->println(str);
	}	
}

void ESP8266Teensy::debugPrintESP(String str) {
	if(this->isDebug) {
		//serialDebug->println("__|¨¨|__|¨¨|__|¨¨|___|¨¨|____|¨¨|___|¨¨|_|¨¨|__|¨¨|_|¨¨|_|¨¨|_|¨¨|__");
		serialDebug->println("__________________________[ESP 8266ex 01]__________________________");
		serialDebug->println(str);
		serialDebug->println("____________________________________________________________________");
	}	
}

void ESP8266Teensy::start( Stream *serial, Stream *serialDebug, int pinReset, bool isDebug ) {
	pinMode(pinReset, OUTPUT);
	digitalWrite(pinReset, HIGH);
	this->isCheckSuccess = false;
	this->pinReset = pinReset;
	this->serial = serial;
	this->serialDebug = serialDebug;
	this->isDebug = isDebug;
	this->TCPConnected = false;
	delay(2000);
}

bool ESP8266Teensy::setMode(int mode) {
	clearBuffer();

	writeOnESP("AT+CWMODE=" + String(mode));
	String data = readData(50);
	debugPrintESP(data);
	if(data.indexOf("OK") >= 0) {
		debugPrintln("Mode change success");
		return true; 
	} else {
		debugPrintln("Mode change failed");
		return false; 
	}
}

bool ESP8266Teensy::setMux(int mux) {
	clearBuffer();
	writeOnESP("AT+CIPMUX=" + String(mux));
	
	String data = readData(500);
	debugPrintln(data);
	data = readData(500);
	debugPrintln(data);
	return (data.indexOf("OK") >= 0); 
}

int ESP8266Teensy::available() {
	return this->serial->available();
}


String ESP8266Teensy::readData() {
	unsigned long timeout = 50;
	return readData(timeout);
}

String ESP8266Teensy::readData(unsigned long timeout) {

	unsigned long t = millis();
	int i=0;
	int len=strlen(this->term);

    while(millis()<t+timeout) {
        if(available() > 0) { 
			buffer[i++]=this->serial->read();
            if(i>=len) {
				if(strncmp(buffer+i-len, term, len)==0) {
				  break;
				}
			  }
        }
	}

	buffer[i]=0;
    return buffer;
}
	
//TODO
String ESP8266Teensy::read() {
	debugPrintln("********* reading ***********");
	String data = readTCPData();
	debugPrintln( data );
	if((data.indexOf("Unlink") >= 0)) {
		this->TCPConnected = false;
		clearBuffer();
		return "";
	}
	return data;
}

//TODO
String ESP8266Teensy::readTCPData() {
	unsigned long timeout = 100;
	unsigned long t = millis();
    String data = "";
    while(millis() - t < timeout) {
    	if(available() > 0) {
	        char r = serial->read();
	        if((data.indexOf("Unlink") >= 0)) {
				return data;
	        } else {
	            data += r;  
                t = millis();
	        }
	    }
    }
    return data;
}

void ESP8266Teensy::writeOnESP(String command) {
	this->debugPrintln("Checking "+command);
	this->write(command);
}

void ESP8266Teensy::write(String str) {
	this->serial->println(str);
	flush();
	delay(50);
}

void ESP8266Teensy::clearBuffer() {
	while(available() > 0) {
		serial->read();
	}
}

void ESP8266Teensy::flush() {
	this->serial->flush();
}

void ESP8266Teensy::check() {
	delay(50);
	debugPrintln("********* Checking ESP-8266 01 ***********");
	reset();
	delay(50);
	clearBuffer();
	String command = "AT";
	writeOnESP(command);
	String data = readData();
	debugPrintESP(data);
/*
	if((data.indexOf("") >= 0)) {
    	debugPrintln("RESET");
		hardReset();
		this->isCheckSuccess = true;
		return;
	}
	if((data.indexOf("busy now ...") >= 0)) {
    	debugPrintln("RESET");
		hardReset();
		this->isCheckSuccess = true;
		return;
	}
*/
	//If we get AT command read success we will read the firmwire and baud rate.
	bool isReadSuccess = (data.indexOf("OK") >= 0) ?true :false; 
	
	if ( isReadSuccess == true ) {
		command = "AT+GMR";
		writeOnESP(command);
		debugPrintESP(readData(1000));

		command = "AT+CIOBAUD?";
		writeOnESP(command);
		debugPrintESP(readData(3000));

		//Mode : 1 = client , 2 = ap mode, 3 = dual mode
		command = "AT+CWMODE?"; //'AT+CWMODE?' 
		writeOnESP(command);
		debugPrintESP(readData(1000));

		//Surrounding access points
		command = "AT+CWLAP";
		writeOnESP(command);
		debugPrintESP(readData(5000));
		
		debugPrintln("********* Checking success ***********");
	}
	this->isCheckSuccess = isReadSuccess;
}
bool ESP8266Teensy::isCheckingSuccess(){
	return this->isCheckSuccess;
}
//TODO
void ESP8266Teensy::hardReset() {
	debugPrintln("********* Hard Resetting ESP-8266 01 ***********");
	digitalWrite(this->pinReset, LOW);
	delay(1000);
	digitalWrite(this->pinReset, HIGH);
	waitingForHardReset();
	debugPrintln("********* Hard Resetting Done ***********");
}

void ESP8266Teensy::reset() {
	debugPrintln("********* Soft Resetting ESP-8266 01 ***********");
	clearBuffer();
	writeOnESP("AT+RST");
	waitingForReset(2000);
	debugPrintln("********* Soft Resetting Done ***********");
}

//Waiting...
void ESP8266Teensy::waitingForHardReset() {
	debugPrintln("********* waitingForHardReset ***********");
	while(true) {
		if(available() > 0) {
			String data = readData();
			if(data.indexOf("ready") >= 0) {
				debugPrintln("RESET");
				delay(2000);
				clearBuffer();
				return;
			}
		}
	}
}

void ESP8266Teensy::waitingForReset(unsigned long timeout) {
	debugPrintln( timeout );
	debugPrintln("********* waitingForReset with time *********** ");
	String data = readData( timeout );
	debugPrintESP(data);
	if(data.substring(1, 4).equals("ets") ) {
		debugPrintln("RESET");
		delay(2000);
		clearBuffer();
		debugPrintln("********* waitingForReset with time Done ***********");
		return;
	}
	debugPrintln("********* waitingForReset with time Done ***********");
}

String ESP8266Teensy::waitingForJoin( unsigned long timeout ) {
	debugPrintln("********* waitingForJoin ***********");
	String espIPAddress = "x-x-x-x"; //Should be populated with good IP address collected from AT+CIFSR
	unsigned long t = millis();
	while(millis() - t < timeout) {
		clearBuffer();
		writeOnESP("AT+CIFSR"); //Get me the ESP IP Address
		String data = readData();
		
		if( (data.indexOf("+CIFSR:APIP") >= 0) ) {
			
			espIPAddress = getValue(data,',',1);  //Get the IP address of ESP
			debugPrintln("ESP IP Address");
			debugPrint(espIPAddress);
			debugPrintESP(data);
		}
		debugPrint("wait to reset or timeout..........");
		delay(500);
	}
	debugPrintln("********* waitingForJoin Done ***********");
	return espIPAddress;
}

//Connecting to Access Point with SSID and Passcode
String ESP8266Teensy::connectToAccessPoint(String ssid, String pass, int mode) {
	debugPrintln("********* Connecting to Accecc Point ***********");
	String ip = "Y-Y-Y-Y";
	
	//Set mode: Client or AP or Both
	if ( setMode( mode ) ) { //Joining as Client mode: ESP_CLIENT / ESP_ACCESSPOINT/ ESP_DUAL

		delay(50);
		writeOnESP("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"");	
		String data = readData(8000); //8 seconds to connect to AP
		debugPrintESP(data);
		if (data.indexOf("OK") >= 0) {
			debugPrintln("********* Accecc Point Connection Success ***********");

			//Reset ESP and wait to join
			waitingForReset(2000);		 //2 seconds to reset
			ip = waitingForJoin( 2000 ); //2 seconds to join
			debugPrintln(ip);
			
			//writeOnESP("AT+CWQAP"); 	//To forget use AT+CWQAP

		} else {
			debugPrintln("********* Accecc Point Connection Failed ***********");
		}
	} 
	return ip;
}

//TCP Server Handling
//Client....
//Start TCP connecttion
bool ESP8266Teensy::startTCPConnection(int channel, String ip, int port) { 
	debugPrintln("********* Connecting to TCP Server ***********");
	clearBuffer();
	writeOnESP("AT+CIPSTART=" + String(channel) + ",\"TCP\",\"" + ip + "\"," + String(port) );

	String data = readData(4000); //4 seconds to connect to AP
	debugPrintESP( data );
	if (data.indexOf("OK") >= 0) {
		debugPrintln("********* Connected to TCP Server ***********");
		return true;

	} else {
		debugPrintln("********* Failed Connecting to TCP Server ***********");
		return false;
	}
}

//Sending data
bool ESP8266Teensy::send(int channel, String message) {
	bool retVal = false;
	debugPrintln("********* Sending message to TCP ***********");
	clearBuffer();
	writeOnESP("AT+CIPSEND=" + String(channel) + "," + String(message.length()));
	String data = readData(6000); //6 seconds to send
	debugPrintESP( data );
	data = readData(500); 
	debugPrintESP( data );
	if(  (data.indexOf(">") >= 0)  ) { 
		debugPrintln("********* ESP is listning to incoming data ***********");

		debugPrintln("********* Sending packets ***********");
		writeOnESP(message);

		String sentData = readData(6000); //6 seconds to send
		debugPrintESP( sentData );
		if(  (sentData.indexOf("OK") >= 0) || (sentData.indexOf("SEND OK") >= 0) ) {
			debugPrintln("********* Successfully sent packet *********");
			retVal = true;
		} else {
			debugPrintln("********* Unable to send packet *********");
		}
	} else {
		debugPrintln("********* ESP is not listning to incoming data ***********");
	}
	return retVal;
}

//Close TCP Server
void ESP8266Teensy::closeTCPConnection(int id) {
	debugPrintln("********* Closing TCP Connection ***********");
	delay(1000);
	clearBuffer();
	writeOnESP("AT+CIPCLOSE=" + String(id));
	debugPrintESP(readData(500));
	debugPrintln("********* Closed TCP Connection ***********");
}

//Server....
//Start TCP server
bool ESP8266Teensy::startTCPServer(int port, int timeout) {
	debugPrintln("********* Starting TCP Server ***********");
	bool retVal = false;
	clearBuffer();
	writeOnESP("AT+CIPSERVER=1," + String(port));
	String data = readData(2000);
	debugPrintESP(data);

	if (data.indexOf("OK") >= 0) {
		retVal = setServerTimeout(timeout);
	} 
	if (retVal == true) {
		debugPrintln("********* Successfully started TCP Server ***********");

		//Ask ESP about the started server info
		writeOnESP("AT+GMR");
		data = readData(1000);
		debugPrintESP( data );

		writeOnESP("AT+CWJAP?");
		data = readData(1000);
		debugPrintESP( data );

		//Get the Station IP address 
		writeOnESP("AT+CIPSTA?");
		data = readData(1000);
		debugPrintESP( data );

		writeOnESP("AT+CWMODE?");
		data = readData(1000);
		debugPrintESP( data );

		//Get the AP IP address 
		writeOnESP("AT+CIFSR");
		data = readData(5000);
		debugPrintESP( data );

		writeOnESP("AT+CWLAP");
		data = readData(5000);
		debugPrintESP( data );

		debugPrintln("********* Collect details for [+CIFSR:APIP], [+CWMODE:?], [+CIPSTA:ip:] ***********");
	}
	else
		debugPrintln("********* Failed starting TCP Server ***********");
		
	return retVal;
}

//Set timeout for Server
bool ESP8266Teensy::setServerTimeout(int timeout) {
	debugPrintln("********* Setting TCP Server timeout ***********");
	bool retVal = false;
	clearBuffer();
	writeOnESP("AT+CIPSTO=" + String(timeout));
	String data = readData(1000);
	debugPrintln( data );
	if (data.indexOf("OK") >= 0) {
		retVal = true;
	}
	if (retVal == true) 
		debugPrintln("********* Successfully set TCP Server timeout ***********");
	else
		debugPrintln("********* Failed setting TCP Server timeout ***********");

	return retVal;
}


//TODO
// void ESP8266Teensy::closeTCPConnection() {
// 	delay(1000);
// 	clearBuffer();
// 	writeOnESP("AT+CIPCLOSE");
// 	debugPrintln(readData(500));
// }



//TODO
// bool ESP8266Teensy::closeTCPServer() {
// 	clearBuffer();
// 	writeOnESP("AT+CIPSERVER=0");
// 	debugPrintln(readData(500));
// 	debugPrintln(readData(500));
// 	String data = readData(500);
// 	debugPrintln(data);
// 	return (data.indexOf("OK") >= 0);
// }



//TODO
// bool ESP8266Teensy::setAP(String ssid, String pass, int channel) {
// 	clearBuffer();
// 	writeOnESP("AT+CWSAP=\"" + ssid + "\",\"" + pass + "\"," + String(channel) + ",4");
// 	debugPrintln(readData(500));
// 	delay(4000);
// 	debugPrintln(readData(500));
// 	String data = readData(500);
// 	debugPrintln(data);
// 	return (data.indexOf("OK") >= 0); 
// }

//TODO
// void ESP8266Teensy::openAccessPoint(String ssid, String pass, int channel) {
// 	if(!isNewAPSetting(ssid, pass, channel)) {
// 		debugPrintln("SET NEW AP");
// 		setAP(ssid, pass, channel);
// 		reset();
// 	}
// 	setMode(ESP_ACCESSPOINT);
// }

//TODO
// bool ESP8266Teensy::isNewAPSetting(String ssid, String pass, int channel) {
// 	clearBuffer();
// 	writeOnESP("AT+CWSAP?");
// 	debugPrintln(readData(500));
// 	String data = readData(500);
// 	debugPrintln(data);
// 	data = data.substring(8, data.length());
// 	byte ssidLength = findChar(data, 0, '\"');
// 	String ssidTmp = data.substring(0, ssidLength);
// 	byte passLength = findChar(data, ssidLength + 3, '\"');
// 	String passTmp = data.substring(ssidLength + 3, passLength);
// 	byte channelLength = findChar(data, passLength + 2, ',');
// 	int channelTmp = data.substring(passLength + 2, channelLength).toInt();
// 	if(ssidTmp.equals(ssid) && passTmp.equals(pass) && channelTmp == channel) {
// 		return true;
// 	} 
// 	return false;
// }

// /*
// bool ESP8266Teensy::isTCPEnabled() {
// 	clearBuffer();
// 	writeOnESP("AT+CIPMUX?");
// 	debugPrintln(readData(500));
// 	String data = readData(500);
// 	debugPrintln(data);
// 	return (data.indexOf("+CIPMUX:1") >= 0); 
// }*/



//????????????????????
// int ESP8266Teensy::isNewDataComing(byte type) {
// 	if(type == WIFI_CLIENT) {
		
// 		String data = read();
// 		if(!(data.indexOf("") >= 0)) {
// 			if(data.substring(2, 6).equals("+IPD")) {
// 				int lastPosition = findChar(data, 7, ':');
// 				int length = data.substring(7, lastPosition).toInt() - 1;
// 				lastPosition += 2;
// 				this->clientMessage = data.substring(lastPosition, lastPosition + length);
// 				debugPrintln( "In if block check for +ipd, SOM ====> ");debugPrintln( this->clientMessage );debugPrintln( " <=== EOM" );
// 				return WIFI_NEW_MESSAGE;
// 			} else if(data.substring(3, 6).equals("ets")) {
// 				debugPrintln("RESET");
// 				delay(2000);
// 				clear();
// 				debugPrintln( "In else-if block reset ====> ");
// 				return WIFI_NEW_RESET;
// 			} else if(data.substring(6, 12).equals("Unlink") 
// 					|| (data.substring(9, 15).equals("Unlink") &&  data.substring(2, 7).equals("ERROR"))) {
// 				debugPrintln( "In else-if block Unlink ====> ");
// 				debugPrintln("Disconnected");
// 				this->TCPConnected = false;
// 				setRunningState(WIFI_STATE_UNAVAILABLE);
// 				return WIFI_NEW_DISCONNECTED;
// 			} else if(data.substring(2, 4).equals("OK") && data.substring(6, 12).equals("Linked")) {
// 				setRunningState(WIFI_STATE_IDLE);
// 				debugPrintln( "In else-if block OK or Link ====> ");				
// 				debugPrintln("Connected");
// 				this->TCPConnected = true;
// 				return WIFI_NEW_CONNECTED;
// 			} else if(data.substring(2, 9).equals("SEND OK")) {
// 				setRunningState(WIFI_STATE_IDLE);
// 				debugPrintln( "In else-if block sent ====> ");								
// 				debugPrintln("Sent!!");
// 				return WIFI_NEW_SEND_OK;
// 			} else if(data.substring(0, 14).equals("ALREAY CONNECT") 
// 					&& data.substring(16, 17).equals("OK") 
// 					&& data.substring(19, 24).equals("Unlink")) {
// 				this->TCPConnected = false;
// 				setRunningState(WIFI_STATE_UNAVAILABLE);
// 				debugPrintln( "In else-if block already connected ====> ");												
// 				return WIFI_NEW_DISCONNECTED;
// 			} else if(data.substring(0, 14).equals("ALREAY CONNECT")) {
// 				return WIFI_NEW_ALREADY_CONNECT;
// 			} else {
// 				debugPrintln( "We are in client else block ====> ");
// 				debugPrintln("******** start **********");
// 				debugPrintln(data);
// 				debugPrintln("********** end *********");
// 				this->clientMessage = data;
// 				return WIFI_NEW_ETC;
// 			}
// 		}
// 		return WIFI_NEW_NONE;
// 	} else if(type == WIFI_SERVER) {

// 		String data = read();
// 		if(!(data.indexOf("") >= 0)) {
// 			if(data.substring(2, 6).equals("+IPD")) {
// 				this->clientId = data.substring(7, 8).toInt();
// 				int lastPosition = findChar(data, 9, ':');
// 				int length = data.substring(9, lastPosition).toInt() + 1;

// 				if(data.charAt(lastPosition + 1) == '\n') {
// 					this->clientMessage = data.substring(lastPosition + 2, lastPosition + length + 1);
// 				} else {
// 					this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
// 				}
				
// 				//this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
// 				return WIFI_NEW_MESSAGE;
// 			} else if(data.substring(3, 6).equals("ets")) {
// 				debugPrintln("RESET");
// 				delay(2000);
// 				clear();
// 				return WIFI_NEW_RESET;
// 			} else if((data.length() == 1 && data.charAt(0) == 0x0A) 
// 					|| (data.substring(2, 4).equals("OK") && data.substring(6, 12).equals("Unlink"))) {
// 				debugPrintln("Disconnected");
// 				this->TCPConnected = false;
// 				setRunningState(WIFI_STATE_UNAVAILABLE);
// 				return WIFI_NEW_DISCONNECTED;
// 			} else if(data.substring(0, 4).equals("Link")
// 					|| data.substring(2, 4).equals("Link") 
// 					|| data.substring(5, 11).equals("Link")) {
// 				setRunningState(WIFI_STATE_IDLE);
// 				debugPrintln("Connected");
// 				this->TCPConnected = true;
// 				return WIFI_NEW_CONNECTED;
// 			} else if(data.substring(2, 9).equals("SEND OK")) {
// 				delay(2000);
// 				setRunningState(WIFI_STATE_IDLE);
// 				debugPrintln("Sent!!");
// 				return WIFI_NEW_SEND_OK;
// 			} else {
// 				debugPrintln("******");
// 				debugPrintln(data);
// 				//debugPrintln(data.substring(2, 9));
// 				debugPrintln("******");
// 				//debugPrintln(data);
// 				this->clientMessage = data;
// 				debugPrintln(data.substring(2, 4));
// 				debugPrintln(data.substring(6, 12));
// 				return WIFI_NEW_ETC;
// 			}
// 		} else {
//             debugPrintln( "We are in server else block ====> ");
//             debugPrintln("******** start **********");
//             debugPrintln(data);
//             debugPrintln("********** end *********");
// 		}
// 		return WIFI_NEW_NONE;

// 		/*
// 			String data = read();
// 			if(!(data.indexOf("") >= 0)) {
// 				if(data.substring(2, 6).equals("+IPD")) {
// 					this->clientId = data.substring(7, 8).toInt();
// 					int lastPosition = findChar(data, 9, ':');
// 					int length = data.substring(9, lastPosition).toInt();
// 					lastPosition += 1;
// 					this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
// 					return true;
// 				} else if(data.substring(3, 6).equals("ets")) {
// 					debugPrintln("RESET");
// 					delay(2000);
// 					clear();
// 					return true;
// 				} else if(data.substring(6,12).equals("Unlink")) {
// 					debugPrintln("Disconnected");
// 					this->TCPConnected = false;
// 					return true;
// 				} else if(data.substring(0, 4).equals("Link")) {
// 					debugPrintln("Connected");
// 					this->TCPConnected = true;
// 					//getClientList();
// 					return true;
// 				} else {
// 					debugPrintln("ETC");
// 					debugPrintln(data);
// 					debugPrintln(data.substring(0, 3));
// 					return true;

// 				}
// 			}
// 			*/
// 		return false;
// 	}
// }

//?????????????????
// int ESP8266Teensy::findChar(String str, int start, char c) {
// 	for(int i = start ; i < str.length() ; i++) {
// 		if(str.charAt(i) == c)
// 			return i;
// 	}
// 	return -1;
// }
