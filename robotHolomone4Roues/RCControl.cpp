#include "RCControl.h"
#include <EEPROM.h>
#include "PinChangeInt.h" // for RC reciver
#include "pinsout.h"


extern RCControl control;


//------------------------------------------------------
//****************************************************************
void calcAvantArriere()
{
  control.rchannel2[0].calcInterrupt();
}
//**********************************************************************
void calcGaucheDroite()
{
    control.rchannel2[1].calcInterrupt();
}
// //**********************************************************************
void calcLateralGaucheDroite()
{
    control.rchannel2[2].calcInterrupt();
}
// //**********************************************************************
void calcNouvelleVoie()
{
    control.rchannel2[3].calcInterrupt();
}
// void calcSW5()
// {
// }


// void calcLatLeftRight()
// {
// }

//**************************************************************
RCControl::RCControl()
{
  nbChannels = NB_CHANNELS;
  for(char i = 0; i < nbChannels; i++){
    rchannel2[i].setChannelNumber(i);  
  }
  rchannel2[0].setPinValue(LEFT_RIGHT_IN_PIN);
  rchannel2[0].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  rchannel2[1].setPinValue(UP_DOWN_IN_PIN);
  rchannel2[1].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  rchannel2[2].setPinValue(LAT_LEFT_RIGHT_PIN);
  rchannel2[2].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  rchannel2[3].setPinValue(SW5_PIN);
  rchannel2[3].setChannelType(Channel_type::TYPE_SWITCH_2_POSITIONS);

}
//****************************************************************
void RCControl::init() 
{
  for(int i = 0; i < nbChannels; i++){
    rchannel2[i].init();
  }

  PCintPort::attachInterrupt(rchannel2[0].getPinValue(), calcAvantArriere,CHANGE);
  PCintPort::attachInterrupt(rchannel2[1].getPinValue(), calcGaucheDroite,CHANGE);
  PCintPort::attachInterrupt(rchannel2[2].getPinValue(), calcLateralGaucheDroite,CHANGE);
  PCintPort::attachInterrupt(rchannel2[3].getPinValue(), calcNouvelleVoie,CHANGE);
}

//***********************************************************************
void RCControl::calibrateZero() {
	int nSamples = 5000;
	for (uint16_t k = 0; k < nSamples; k++)
	{
		delay(1);
    for(int i =0; i<nbChannels; i++){
      rchannel2[i].calibrateZero();
    }
	}
  for(int i =0; i<nbChannels; i++){
    rchannel2[i].calibrateZeroAverage(nSamples);
  }
}
//**************************************************************
void RCControl::calibrateInputs()
{	
	//Compute Minimum and maximum values during x samples
	for (uint16_t k = 0; k < 100; k++)
	{
    for(int i = 0; i < nbChannels; i++){
      rchannel2[i].calibrateInputs();
    }
		delay(50);
	}
	
    for(int i = 0; i < nbChannels; i++){
      rchannel2[i].calibrateInputsEnd();
    }
}
//******************************************************************

//*************************************************************************
void RCControl::initializeFromEEPROM() 
{
  // get() can be used with custom structures too.
  int eeAddress = 0; //Move address to the next byte after float 'f'.
  CalibrationData customVar; //Variable to store custom object read from EEPROM.
  EEPROM.get( eeAddress, customVar );

  Serial.println( "Read custom object from EEPROM: " );
  char j = 0; 
  for( char i = 0; i < NB_CHANNELS; i++ ){
    rchannel2[i].zero_timing = customVar.data[j++];
    rchannel2[i].min_coeff = customVar.data[j++];
    rchannel2[i].max_coeff = customVar.data[j++];
  }
  debugCalibration();
}
//************************************************
void RCControl::writeToEEPROM() 
{
  CalibrationData customVar;
  int eeAddress = 0;
  char j        = 0; 

  for( char i = 0; i < NB_CHANNELS; i++ ){
    customVar.data[j++] = rchannel2[i].zero_timing;
    customVar.data[j++] = rchannel2[i].min_coeff;
    customVar.data[j++] = rchannel2[i].max_coeff;
  }

  EEPROM.put(eeAddress, customVar);

}

//************************************************
void RCControl::debugInterrupt()
{
  Serial.println("--------------------------------");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(" ");Serial.print(i);Serial.print("\t | ");     
  }
  Serial.println("");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print((int)rchannel2[i].getEnd());Serial.print("\t | "); 
  }
  Serial.println("");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print((int)rchannel2[i].getValue());Serial.print("\t | "); 
  }
  Serial.println("");
}
//************************************************
void RCControl::debugCalibration()
{
  Serial.println("--------------------------------");
  Serial.println("[min, zero, max]");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(" ");Serial.print(i);Serial.print("\t\t | ");     
  }
  Serial.println("");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print((int)rchannel2[i].getMinTiming());Serial.print(" "); 
    Serial.print((int)rchannel2[i].getZeroTiming());Serial.print(" "); 
    Serial.print((int)rchannel2[i].getMaxTiming());Serial.print("\t | "); 
  }
  Serial.println("");
  Serial.println("[min_coeff, max_coeff]");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(" ");Serial.print(i);Serial.print("\t | ");     
  }
  Serial.println("");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(rchannel2[i].getMinCoeff());Serial.print(" "); Serial.print(rchannel2[i].getMaxCoeff());Serial.print("\t | "); 
  }

  Serial.println("");
}
//*****************************************************************
float RCControl::getThrottle()
{
  return rchannel2[0].getValue();
}
//******************************************************************
float RCControl::getSteering() 
{
  return rchannel2[1].getValue();
}

//******************************************************************
float RCControl::getLatSteering() 
{
  return rchannel2[2].getValue();
}

//******************************************************************
float RCControl::getSW5() 
{
  return rchannel2[3].getValue();
}

