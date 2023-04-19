#include "RCControl.h"
#include "PinChangeInt.h" // for RC reciver

#include <EEPROM.h>

#define LEFT_RIGHT_IN_PIN  14
#define UP_DOWN_IN_PIN   15
#define PWM_RELAIS   7

#define REVERSE_THROTTLE
//#define REVERSE_STEERING

uint32_t UpDownStart;
uint32_t LeftRightStart;
uint32_t RelaisStart;

volatile uint16_t UpDownEnd = 0;
volatile uint16_t LeftRightEnd = 0;
volatile uint16_t RelaisEnd = 0;

//rc receiver interrupt routine
//------------------------------------------------------
//****************************************************************
void calcUpDown()
{
	 //Serial.println("in up down here");
	if(digitalRead(UP_DOWN_IN_PIN) == HIGH)
	{
		UpDownStart = micros();
	}
	else
	{
		UpDownEnd = (uint16_t)(micros() - UpDownStart);
		if (UpDownEnd > 2200) 
			UpDownEnd = 1500;
	}
}
//**********************************************************************
void calcLeftRight()
{
	// Serial.println("in Left Right here");
	if(digitalRead(LEFT_RIGHT_IN_PIN) == HIGH)
	{
		LeftRightStart = micros();
	}
	else
	{
		LeftRightEnd = (uint16_t)(micros() - LeftRightStart);
		if (LeftRightEnd > 2200) 
			LeftRightEnd = 1500;
	}
}
//**********************************************************************
void calcRelais()
{
  // Serial.println("in Left Right here");
  if(digitalRead(PWM_RELAIS) == HIGH)
  {
    RelaisStart = micros();
  }
  else
  {
    RelaisEnd = (uint16_t)(micros() - RelaisStart);
  }
}
// RCControl class
// --------------------------------------------------------
float RCControl::zero_throttle;
float RCControl::zero_steering;

float RCControl::max_throttle_coeff;
float RCControl::max_steering_coeff;
float RCControl::min_throttle_coeff;
float RCControl::min_steering_coeff;

int RCControl::throttle_amplitude;
int RCControl::steering_amplitude;
//**************************************************************
RCControl::RCControl()
{
	throttle_amplitude = 100;
	steering_amplitude = 100;
}
//****************************************************************
void RCControl::init() 
{
	pinMode(UP_DOWN_IN_PIN, INPUT);
	pinMode(LEFT_RIGHT_IN_PIN, INPUT);

	PCintPort::attachInterrupt(UP_DOWN_IN_PIN, calcUpDown,CHANGE);
	PCintPort::attachInterrupt(LEFT_RIGHT_IN_PIN, calcLeftRight,CHANGE);
  PCintPort::attachInterrupt(PWM_RELAIS, calcRelais,CHANGE);
}
//***********************************************************************
void RCControl::calibrateZero() {
	//Take mean value over 2s
	zero_throttle=0.0;
	zero_steering=0.0;
	
	int nSamples = 10;
	for (uint16_t k = 0; k < nSamples; k++)
	{
		delay(200);
		zero_throttle += UpDownEnd;
		zero_steering += LeftRightEnd;
	}
	zero_throttle = zero_throttle/(float)nSamples;
	zero_steering = zero_steering/(float)nSamples;
}
//**************************************************************
void RCControl::calibrateInputs()
{
	float minMeasuredThrottle=zero_throttle;
	float maxMeasuredThrottle=zero_throttle;
	float minMeasuredSteering=zero_steering;
	float maxMeasuredSteering=zero_steering;
	
	float throttle;
	float steering;
	
	//Compute Minimum and maximum values during 10s
	for (uint16_t k = 0; k < 100; k++)
	{
		throttle = UpDownEnd;
		steering = LeftRightEnd;
		if (throttle>maxMeasuredThrottle) {
			maxMeasuredThrottle=throttle;
		}
		if (throttle<minMeasuredThrottle) {
			minMeasuredThrottle=throttle;
		}
		if (steering>maxMeasuredSteering) {
			maxMeasuredSteering=steering;
		}
		if (steering<minMeasuredSteering) {
			minMeasuredSteering=steering;
		}
		delay(100);
	}
	
	max_throttle_coeff = throttle_amplitude / (maxMeasuredThrottle - zero_throttle);
	min_throttle_coeff = throttle_amplitude / (zero_throttle - minMeasuredThrottle);
	max_steering_coeff = steering_amplitude / (maxMeasuredSteering - zero_steering);
	min_steering_coeff = steering_amplitude / (zero_steering - minMeasuredSteering);

#ifdef REVERSE_THROTTLE
	max_throttle_coeff = -max_throttle_coeff;
	min_throttle_coeff = -min_throttle_coeff;
#endif
#ifdef REVERSE_STEERING
	max_steering_coeff = -max_steering_coeff;
	min_steering_coeff = -min_steering_coeff;
#endif
}
//*****************************************************************
float RCControl::getThrottle()
{
	float temp = UpDownEnd - zero_throttle;
	
	if (temp > 0) {
		return temp * max_throttle_coeff;
	}
	else {
		return temp * min_throttle_coeff;
	}
}
//******************************************************************
float RCControl::getSteering() 
{
	float temp = LeftRightEnd - zero_steering;
	
	if (temp > 0) {
		return temp * max_steering_coeff;
	}
	else {
		return temp * min_steering_coeff;
	}
}
//******************************************************************
float RCControl::getRelais() 
{
float temp = RelaisEnd;
return temp;
}
//*************************************************************************
void RCControl::initializeFromEEPROM() 
{
  // get() can be used with custom structures too.
 int eeAddress = 0; //Move address to the next byte after float 'f'.
 CalibrationData customVar; //Variable to store custom object read from EEPROM.
  EEPROM.get( eeAddress, customVar );

  Serial.println( "Read custom object from EEPROM: " );
  Serial.println( customVar.zero_throttle );
  Serial.println( customVar.zero_steering );
  Serial.println( customVar.max_throttle_coeff );
  Serial.println( customVar.max_steering_coeff );
  Serial.println( customVar.min_throttle_coeff );
  Serial.println( customVar.min_steering_coeff );

    zero_throttle = customVar.zero_throttle;
    zero_steering = customVar.zero_steering;

    max_throttle_coeff = customVar.max_throttle_coeff;
    max_steering_coeff = customVar.max_steering_coeff;
    min_throttle_coeff = customVar.min_throttle_coeff;
    min_steering_coeff = customVar.min_steering_coeff;

}
//************************************************
void RCControl::writeToEEPROM() 
{

  int eeAddress = 0;

  CalibrationData customVar = {
    zero_throttle,
    zero_steering,
    max_throttle_coeff,
    max_steering_coeff,
    min_throttle_coeff,
    min_steering_coeff
  };

  Serial.print("zero_steering");Serial.println(zero_steering);
  
EEPROM.put(eeAddress, customVar);
  Serial.print("Written custom data type! \n\nView the example sketch eeprom_get to see how you can retrieve the values!");
}
