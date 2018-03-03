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
#ifndef ActuatorSMS_h
#define ActuatorSMS_h

#include "Arduino.h"


#include <ZerooneSupermodified_T3X_Wire.h>
#include <EEPROM.h>

#define Max_Absolute_Position 16384

//Todo : change
#define MAXIMUS_NORMAL_OPERATION      0
#define MAXIMUS_SET_HOME              -600
#define MAXIMUS_SET_START             -700
#define MAXIMUS_STOP_SERVO            -800
#define MAXIMUS_RESET_ERROR           -999
//Todo



class ActuatorSMS
{
    public:
        ActuatorSMS( int actuatorId );
        double ticks2deg(signed int ticks);
        signed int deg2ticks(double deg);
//        void setDefaultPIDGain ( );
//        void setSoftStiffnessPIDGain ( );
//        void setHomeAndStart( );
//        void setStart( );
//        void setStop( );
//        void testActuatorSpan ( char command,  int spanCWInDegree, int spanCCWInDegree );
//        void performComand ( );
    
    private:
    
        int _actuatorId;
    

//        const float actuatorSingleTickInDegree =  0.02197265625; //->  360/16384 (i4 bit contactless magnetic encoder)
//        /** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
//        unsigned int eeAddress = 0;
//    
//        void EEPROMWritelong(int address, long value);
//        long EEPROMReadlong(long address);
};

#endif
