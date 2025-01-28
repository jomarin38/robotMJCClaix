#include "RCControl.h"
#include <EEPROM.h>
#include "PinChangeInt.h" // for RC reciver
#include "pinsout.h"


extern RCControl control;


//------------------------------------------------------
// handlers d interruptions
//****************************************************************
void calcAvantArriere()
{
  control.rchannels[0].calcInterrupt();
}
//**********************************************************************
void calcGaucheDroite()
{
    control.rchannels[1].calcInterrupt();
}
//**********************************************************************
void calcLateralGaucheDroite()
{
    control.rchannels[2].calcInterrupt();
}
//**********************************************************************
void calcSW5()
{
    control.rchannels[3].calcInterrupt();
}
//**********************************************************************
void calcSW6()
{
    control.rchannels[4].calcInterrupt();
}
//**********************************************************************
void calcSW7()
{
    control.rchannels[5].calcInterrupt();
}
//**********************************************************************
void calcSW8()
{
    control.rchannels[6].calcInterrupt();
}

//**************************************************************
RCControl::RCControl()
{
  // nbChannels = NB_CHANNELS;
  // for(char i = 0; i < nbChannels; i++){
  //   rchannels[i].setChannelNumber(i);  
  // }
  // rchannels[0].setPinValue(UP_DOWN_IN_PIN);
  // rchannels[0].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  // rchannels[1].setPinValue(LEFT_RIGHT_IN_PIN);
  // rchannels[1].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  // rchannels[2].setPinValue(LAT_LEFT_RIGHT_PIN);
  // rchannels[2].setChannelType(Channel_type::TYPE_PROPORTIONNEL);
  // rchannels[3].setPinValue(SW5_PIN);
  // rchannels[3].setChannelType(Channel_type::TYPE_SWITCH_3_POSITIONS);

  // rchannels[4].setPinValue(SW6_PIN);
  // rchannels[4].setChannelType(Channel_type::TYPE_SWITCH_2_POSITIONS);

  // rchannels[5].setPinValue(SW7_PIN);
  // rchannels[5].setChannelType(Channel_type::TYPE_SWITCH_2_POSITIONS);

  // rchannels[6].setPinValue(SW8_PIN);
  // rchannels[6].setChannelType(Channel_type::TYPE_SWITCH_3_POSITIONS);

}
//****************************************************************
void RCControl::init() 
{
  nbChannels = NB_CHANNELS;

  rchannels[0].setValues(0, UP_DOWN_IN_PIN,     Channel_type::TYPE_PROPORTIONNEL);
  rchannels[1].setValues(1, LEFT_RIGHT_IN_PIN,  Channel_type::TYPE_PROPORTIONNEL);
  rchannels[2].setValues(2, LAT_LEFT_RIGHT_PIN, Channel_type::TYPE_PROPORTIONNEL);
  rchannels[3].setValues(3, SW5_PIN,            Channel_type::TYPE_SWITCH_3_POSITIONS);
  rchannels[4].setValues(4, SW6_PIN,            Channel_type::TYPE_SWITCH_2_POSITIONS);
  rchannels[5].setValues(5, SW7_PIN,            Channel_type::TYPE_SWITCH_2_POSITIONS);
  rchannels[6].setValues(6, SW8_PIN,            Channel_type::TYPE_SWITCH_3_POSITIONS);
  
  for(int i = 0; i < nbChannels; i++){
    rchannels[i].init();
  }

  PCintPort::attachInterrupt(rchannels[0].getPinValue(), calcAvantArriere,CHANGE);
  PCintPort::attachInterrupt(rchannels[1].getPinValue(), calcGaucheDroite,CHANGE);
  PCintPort::attachInterrupt(rchannels[2].getPinValue(), calcLateralGaucheDroite,CHANGE);
  PCintPort::attachInterrupt(rchannels[3].getPinValue(), calcSW5,CHANGE);
  PCintPort::attachInterrupt(rchannels[4].getPinValue(), calcSW6,CHANGE);
  PCintPort::attachInterrupt(rchannels[5].getPinValue(), calcSW7,CHANGE);
  PCintPort::attachInterrupt(rchannels[6].getPinValue(), calcSW8,CHANGE);
}

//***********************************************************************
void RCControl::calibrateZero() {
	int nSamples = 100;
	for (uint16_t k = 0; k < nSamples; k++)
	{
		delay(50);
    for(int i =0; i<nbChannels; i++){
      rchannels[i].calibrateZero();
    }
	}
  for(int i =0; i<nbChannels; i++){
    rchannels[i].calibrateZeroAverage(nSamples);
  }
}

//**************************************************************
void RCControl::calibrateInputs()
{	
	//Compute Minimum and maximum values during x samples
	for (uint16_t k = 0; k < 150; k++)
	{
    for(int i = 0; i < nbChannels; i++){
      rchannels[i].calibrateInputs();
    }
		delay(50);
	}
	
    for(int i = 0; i < nbChannels; i++){
      rchannels[i].calibrateInputsEnd();
    }
}

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
    rchannels[i].zero_timing = customVar.data[j++];
    rchannels[i].min_coeff = customVar.data[j++];
    rchannels[i].max_coeff = customVar.data[j++];
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
    customVar.data[j++] = rchannels[i].zero_timing;
    customVar.data[j++] = rchannels[i].min_coeff;
    customVar.data[j++] = rchannels[i].max_coeff;
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
    Serial.print((int)rchannels[i].getEnd());Serial.print("\t | "); 
  }
  Serial.println("");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print((int)rchannels[i].getValue());Serial.print("\t | "); 
  }
  Serial.println("");
}
//************************************************
void RCControl::debugCalibration()
{
  Serial.println("--------------------------------");
  Serial.println("[min, zero, max]");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(" ");Serial.print(i);Serial.print("\t\t\t | ");     
  }
  Serial.println("");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print((int)rchannels[i].getMinTiming());Serial.print(" "); 
    Serial.print((int)rchannels[i].getZeroTiming());Serial.print(" "); 
    Serial.print((int)rchannels[i].getMaxTiming());Serial.print("\t\t | "); 
  }
  Serial.println("");
  Serial.println("[min_coeff, max_coeff]");
  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(" ");Serial.print(i);Serial.print("\t\t | ");     
  }
  Serial.println("");

  for(int i =0; i<NB_CHANNELS; i++){
    Serial.print(rchannels[i].getMinCoeff());Serial.print(" "); Serial.print(rchannels[i].getMaxCoeff());Serial.print("\t | "); 
  }

  Serial.println("");
}
//*****************************************************************
float RCControl::getThrottle()
{
  return rchannels[0].getValue();
}
//******************************************************************
float RCControl::getSteering() 
{
  return rchannels[1].getValue();
}

//******************************************************************
float RCControl::getLatSteering() 
{
  return rchannels[2].getValue();
}

//******************************************************************
float RCControl::getSW5() 
{
  return rchannels[3].getValue();
}

//******************************************************************
float RCControl::getSW6() 
{
  return rchannels[4].getValue();
}

//******************************************************************
float RCControl::getSW7() 
{
  return rchannels[5].getValue();
}
//******************************************************************
float RCControl::getSW8() 
{
  return rchannels[6].getValue();
}
