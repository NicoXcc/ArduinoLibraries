/*
 *                      
 ****************************************************************************
 ***************************                       **************************
 *                                                                          *
 * [!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!] *
 * [!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!] *
 * [---      ----            !!!!!!!!!!!!!!!!!!             -----      ---] *
 * [---      ------             !!!!!!!!!!!!!!            -------      ---] *
 * [---      --------             !!!!!!!!!             ---------      ---] *
 * [---      --------!!             !!!!!             !!---------      ---] *
 * [---      ---------!!!            !!!            !!!----------      ---] *
 * [---      ----------!!!!           !           !!!!-----------      ---] *
 * [---      ---------------!                   !----------------      ---] *
 * [---      ------------------               -------------------      ---] *
 * [---      --------------------           ---------------------      ---] *
 * [---      -----------------------      -----------------------      ---] *
 * [---      -------------------------  -------------------------      ---] *
 * [---      ------------------------- --------------------------      ---] *
 * [---      ----------------------------------------------------      ---] *
 * [!!!!!!!!!!!!!!]                                        [!!!!!!!!!!!!!!] *
 * [!!!!!!!!!!!!!!]                                        [!!!!!!!!!!!!!!] *
 *                                                                          *
 **************** MAXIMUS By NicoX [arch.smaitra@gmail.com] ***************** 
 * 
 * Version: 2.0                         
 * ******************************************************************************************************************** *
 * ******************************************************************************************************************** *
*/

#include "Arduino.h"
#include "ActuatorSMS.h"

/*
 For 01Mechatronics Super Modified Servo or simply SMS read pin is attached to Teensy's pin 14
 */
/*
 ZO_HW_SERIAL is Serial for Teensy 3.6 not Serial1 or other
 */
const int _readPin = 14;
ZerooneSupermodified motor( ZO_HW_SERIAL, _readPin );

ActuatorSMS :: ActuatorSMS( int actuatorId  ){
    
    
//
    _actuatorId = actuatorId;
//    //Error reset
    motor.resetErrors( _actuatorId );
}


double ActuatorSMS :: ticks2deg(signed int ticks){
    return (double)360*ticks/(double)Max_Absolute_Position;
}

signed int ActuatorSMS :: deg2ticks(double deg){
    return (signed int)deg*Max_Absolute_Position/360;
}

/*
 Methos will set actuator with default gain
 01Mechatronics default value P = 180, I = 20, D = 140
 */
//void ActuatorSMS :: setDefaultPIDGain (  ) {
//    
//    motor.setPIDgainP(_actuatorId, 180);
//    motor.setPIDgainI(_actuatorId, 20);
//    motor.setPIDgainD(_actuatorId, 140);
//}

/*
 Methos will set actuator with a spacial PID gain for soft stiffness
 01Mechatronics default value P = 4, I = 26, D = 10
 TODO: Need to test with actuall graph out put .. need help of Ioannis
 */
//void ActuatorSMS :: setSoftStiffnessPIDGain ( ) {
//    
//    motor.setPIDgainP(_actuatorId, 4);  //Best value Kp = 180 >> 4
//    motor.setPIDgainI(_actuatorId, 26);  //Best value Ki = 20  >> 26
//    motor.setPIDgainD(_actuatorId, 10);  //Best value Kd = 140 >>10
//}


//void ActuatorSMS :: setHomeAndStart(  ){
//    motor.start( _actuatorId );
//    //int homeTickPosition = motor.getAbsolutePosition( _actuatorId );      //Absolute position of zero point
//    motor.resetIncrementalPosition( _actuatorId );
//    motor.setProfiledAbsolutePositionSetpoint(_actuatorId, 0);
//    motor.broadCastDoMove();
//    motor.stop( _actuatorId );
//}
//
//void ActuatorSMS :: setStart( ){
//    motor.start(_actuatorId);
//}
//
//void ActuatorSMS :: setStop( ){
//    motor.stop(_actuatorId);
//}



/*
 Methos will set actuator with its id and start position and end position
 We will call this method on actuator oject to set all actuated joints
 Params: id: int, startPostion: int, endPosition: int
 01Mechatronics default value P = 180, I = 20, D = 140
 */
//void ActuatorSMS :: testActuatorSpan ( char command,  int spanCWInDegree, int spanCCWInDegree ) {
//    
//    int Kp = motor.getPIDgainP( _actuatorId );                         //Kp
//    int Ki = motor.getPIDgainI( _actuatorId );                         //Ki
//    int Kd = motor.getPIDgainD( _actuatorId );                         //Kd
//    
//    int wakeUpAbsPosition = motor.getAbsolutePosition( _actuatorId );  //Absolute position of zero point
//    int curPosition = motor.getPosition( _actuatorId );                //Curent position of zero point
//    
//    int curVelocity = motor.getVelocity( _actuatorId );                //Unit ticks/sec
//    int curCurrent = motor.getCurrent( _actuatorId );                  //Unit mA
////    Serial.print( " Current = " ); Serial.print( curCurrent, DEC );
////    Serial.println();
//    
////    if ( Serial.available()) {
////        int command = Serial.read();
//        uint16_t homeAbsolutePosition = EEPROMReadlong( eeAddress );        //Last known position of zero point
//        
//        
//        //Set Zero Position for the servo, can be anywhere but it considers as zero position
//        if ( command == '0' ) {
//            
//            /*
//             * When Start is received incremental position is reset [resetIncrementalPosition(_actuatorId) ],
//             * memory is initialized and the controller enters position control mode
//             * with position setpoint = 0.
//             */
//            motor.start(_actuatorId);
//            int homeTickPosition = motor.getAbsolutePosition( _actuatorId );      //Absolute position of zero point
//            homeAbsolutePosition = ceil( ( homeTickPosition + wakeUpAbsPosition ) / 2 );
//            EEPROMWritelong(eeAddress, homeAbsolutePosition);                    //Writing first long.
//            
//            motor.resetIncrementalPosition( _actuatorId );
//            motor.setProfiledAbsolutePositionSetpoint(_actuatorId, 0);
//            motor.broadCastDoMove(); //?????? may this be called after all SMS set to its position
//            
//            
////            Serial.println();
////            Serial.print("\t Wake up homeTickPosition = ");
////            Serial.println(homeAbsolutePosition);
//            
//            
//            //Reading first long.
//            uint16_t value = EEPROMReadlong(eeAddress);
//            homeAbsolutePosition = value;
////            Serial.print("*** EPROM Address = ");
////            Serial.print(eeAddress);
////            Serial.print("\t Last Known Abs Position = ");
////            Serial.println(value);
////            Serial.print(" ***");
//            
//            motor.stop(_actuatorId);
//        }
//        
//        //Start servo
//        if ( command == '1' ) {
//            motor.start(_actuatorId);
//        }
//        
//        if ( command == '2' ) {
//            motor.moveToAbsolutePosition(_actuatorId, 0 );
//        }
//        
//        if ( command == '3' ) {
//            motor.moveWithVelocity(_actuatorId, 10000);
//            
//            /*motor.setPIDgainP(_actuatorId, 2);  //Best value Kp = 180 >> 2
//             motor.setPIDgainD(_actuatorId, 200);  //Best value Kd = 140 >>200
//             motor.setPIDgainI(_actuatorId, 26);  //Best value Ki = 20  >> 26*/
//        }
//        
//        if ( command == '4' ){
//            motor.moveToAbsolutePosition(_actuatorId, ( spanCWInDegree / actuatorSingleTickInDegree) );    //Clock Wise
//        }
//        
//        if ( command == '5' ){
//            
//            for (int i = 0; i< 50; i++ ) {
//                motor.profiledMoveToAbsolutePosition(_actuatorId, ( spanCCWInDegree / actuatorSingleTickInDegree ) );  //Couter Clock Wise
//                delay(400);
//                motor.profiledMoveToAbsolutePosition(_actuatorId, ( spanCWInDegree / actuatorSingleTickInDegree) );    //Clock Wise
//                delay(400);
//            }
//            motor.moveToAbsolutePosition(_actuatorId, 0 );
//            
//        }
//        
//        if ( command == '6' ) {
//            setSoftStiffnessPIDGain();
//        }
//        
//        if ( command == '7' ) {
//            setDefaultPIDGain();
//        }
//        
//        if ( command == '8' ){
//            motor.resetErrors(_actuatorId);//send a command
//        }
//        
//        if ( command == '9' ){
//            motor.halt(_actuatorId);
//        }
//        if ( command == 's' ){
//            motor.stop(_actuatorId);
//        }
//        
////        Serial.println();
////        Serial.print( " #[ " ); Serial.print( _actuatorId, DEC );
////        Serial.print( " ] Kp= " ); Serial.print( Kp, DEC );
////        Serial.print( " Ki= " ); Serial.print( Ki, DEC );
////        Serial.print( " Kd= " ); Serial.print( Kd, DEC );
////        Serial.print( " Abs Position = " ); Serial.print( wakeUpAbsPosition, DEC );
////        Serial.print( " Home Position = " ); Serial.print( homeAbsolutePosition, DEC );
////        Serial.print( " <Ticks moved?>  = " ); Serial.print( curPosition, DEC );
////        Serial.print( " Velocity = " ); Serial.print( curVelocity, DEC );
////        Serial.print( " Current = " ); Serial.print( curCurrent, DEC );
////        Serial.println();
//    
//        if ( !motor.getCommunicationSuccess() ) {
////            Serial.println();
////            Serial.print("Communication Warning :");
////            Serial.print(motor.getWarning());
//        }
//        else {
////            Serial.println();
////            Serial.print("Communication Success.");
//        }
////    }
//    
//    
//    // motor.halt( _actuatorId );
//    // motor.profiledMoveToRelativePosition( _actuatorId, 50 );
//    // motor.profiledMoveToRelativePosition( _actuatorId, -50 );
//    
//}
//
//
//void ActuatorSMS :: performComand () {
//    
////    int Kp = motor.getPIDgainP( _actuatorId );                         //Kp
////    int Ki = motor.getPIDgainI( _actuatorId );                         //Ki
////    int Kd = motor.getPIDgainD( _actuatorId );                         //Kd
////    
////    int wakeUpAbsPosition = motor.getAbsolutePosition( _actuatorId );  //Absolute position of zero point
////    int curPosition = motor.getPosition( _actuatorId );                //Curent position of zero point
////    
////    int curVelocity = motor.getVelocity( _actuatorId );                //Unit ticks/sec
////    int curCurrent = motor.getCurrent( _actuatorId );                  //Unit mA
////    Serial.print( " Current = " ); Serial.print( curCurrent, DEC );
////    
////    // if there's any serial available, read it:
////    while (Serial.available() > 0) {
////        
////        //Look for the next valid integer in the incoming serial stream:
////        //Get Command ID
////        int commandID = Serial.parseInt();
////        
////        //Get actuator ID
////        int motorId = Serial.parseInt();
////        
////        //Get command
////        int goalPositionValue = Serial.parseInt();
////        
////        Serial.println();
////        Serial.print("\t id = ");
////        Serial.println(motorId);
////        Serial.print("\t position = ");
////        Serial.println(goalPositionValue);
////        
////        if ( commandID == MAXIMUS_SET_HOME ) {
////            setHomeAndStart( );
////            Serial.print("******* Set home ***** #");
////        }
////        else if ( commandID == MAXIMUS_SET_START ) {
////            setStart( );
////            Serial.print("******* Started ***** #");
////        }
////        else if ( commandID == MAXIMUS_RESET_ERROR ){ //Should be switch :todo
////            motor.resetErrors(motorId);//send a command
////            Serial.print("******* Reset Error Of ***** #");
////        }
////        else if ( commandID == MAXIMUS_STOP_SERVO ) {
////            setStop( );
////            Serial.print("******* Stopped ***** #");
////        }
////        else {
////            motor.profiledMoveToAbsolutePosition(motorId, deg2ticks(goalPositionValue)  );  //fed degree converted to tick
////        }
////        Serial.print( motorId, DEC );
////        
////        if ( !motor.getCommunicationSuccess() ) {
////            Serial.println();
////            Serial.print("Communication Warning :");
////            Serial.print(motor.getWarning());
////            motor.resetErrors(motorId);//send a command
////        }
////        else {
////            Serial.println();
////            Serial.print("Communication Success.");
////        }
////    }
////    Serial.println();
////    Serial.print( " #[ " ); Serial.print( _actuatorId, DEC );
////    Serial.print( " ] Kp= " ); Serial.print( Kp, DEC );
////    Serial.print( " Ki= " ); Serial.print( Ki, DEC );
////    Serial.print( " Kd= " ); Serial.print( Kd, DEC );
////    Serial.print( " Abs Position = " ); Serial.print( wakeUpAbsPosition, DEC );
////    Serial.print( " <Ticks moved?>  = " ); Serial.print( curPosition, DEC );
////    Serial.print( " Velocity = " ); Serial.print( curVelocity, DEC );
////    Serial.print( " Current = " ); Serial.print( curCurrent, DEC );
////    Serial.println();
//}
//
//
////This function will write a 4 byte (32bit) long to the eeprom at
////the specified address to address + 3.
//void ActuatorSMS :: EEPROMWritelong(int address, long value) {
//    //Decomposition from a long to 4 bytes by using bitshift.
//    //One = Most significant -> Four = Least significant byte
//    byte four = (value & 0xFF);
//    byte three = ((value >> 8) & 0xFF);
//    byte two = ((value >> 16) & 0xFF);
//    byte one = ((value >> 24) & 0xFF);
//    
//    //Write the 4 bytes into the eeprom memory.
//    EEPROM.write(address, four);
//    EEPROM.write(address + 1, three);
//    EEPROM.write(address + 2, two);
//    EEPROM.write(address + 3, one);
//}
//
//
////This function will return a 4 byte (32bit) long from the eeprom
////at the specified address to address + 3.
//long ActuatorSMS :: EEPROMReadlong(long address) {
//    //Read the 4 bytes from the eeprom memory.
//    long four = EEPROM.read(address);
//    long three = EEPROM.read(address + 1);
//    long two = EEPROM.read(address + 2);
//    long one = EEPROM.read(address + 3);
//    
//    //Return the recomposed long by using bitshift.
//    return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
//}
