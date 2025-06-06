#include "RCChannel.h"
//#include "PinChangeInt.h" // for RC reciver


// Constructeur
RCChannel::RCChannel(char initialChannelTypeValue, char initialPinValue)
    : channelType(initialChannelTypeValue), pinValue(initialPinValue) {}


void RCChannel::init(){
//  pinMode(pinValue, INPUT);
//	PCintPort::attachInterrupt(pinValue, calc,CHANGE);
  min_timing = 1500;
}

void RCChannel::calcInterrupt()
{
	 //Serial.println("in up down here");
	if(digitalRead(getPinValue()) == HIGH)
	{
		start = micros();
	}
	else
	{
		end = (uint16_t)(micros() - start);
		if (end > 2200) 
			end = 1500;
	}
}

void RCChannel::calc(){
  	 //Serial.println("in up down here");
	if(digitalRead(pinValue) == HIGH)
	{
		start = micros();
	}
	else
	{
		end = (uint16_t)(micros() - start);
		if (end > 2200) 
			end = 1500;
	}
}

// Getter pour le numéro de la voie
int RCChannel::getChannelNumber() const {
    return channelNumber;
}

int RCChannel::getPinValue() const {
    return pinValue;
}


// Getter pour la valeur de la voie
float RCChannel::getValue() {
  float temp = end - zero_timing;
	if(channelType == Channel_type::TYPE_PROPORTIONNEL){
    if (temp > 0) {
      return temp * max_coeff;
    }
    else {
      return temp * min_coeff;
    }
  } else if(channelType == Channel_type::TYPE_SWITCH_2_POSITIONS){
    if(temp < 0)
      return -amplitude;
    else
      return amplitude;    

  } else if(channelType == Channel_type::TYPE_SWITCH_3_POSITIONS){
    if(temp > amplitude/10)
      return amplitude;
    else if(temp < -amplitude / 10){
      return -amplitude;
    } else {
      return 0;
    }
  } else {Serial.println("channel type not manage");}
}

float RCChannel::getZeroTiming() {
    return zero_timing;
}

float RCChannel::getMinTiming() {
    return min_timing;
}

float RCChannel::getMaxTiming() {
    return max_timing;
}

float RCChannel::getMinCoeff(){
    return min_coeff;
}
float RCChannel::getMaxCoeff(){
    return max_coeff;
}


float RCChannel::getEnd() {
    return end;
}

// Setter pour la valeur de la voie
void RCChannel::setValue(float newValue) {
    value = newValue;
}
// Setter pour la valeur de la pin/GPIO
void RCChannel::setPinValue(char newValue) {
    pinValue = newValue;
}
void RCChannel::setChannelType(char newValue) {
    channelType = newValue;
}
void RCChannel::setChannelNumber(int newValue) {
    channelNumber = newValue;
}


// Réinitialise la voie à sa valeur par défaut
void RCChannel::reset() {
    value = 0.0;
}

void RCChannel::calibrateZero(){
  		zero_timing += end;
}

void RCChannel::calibrateZeroAverage(int nSamples){
  		zero_timing = zero_timing/nSamples;
}


void RCChannel::calibrateInputs(){
  if ( end > max_timing ) {
    max_timing = end;
  }
  if ( end < min_timing ) {
    min_timing = end;
  }
}

void RCChannel::calibrateInputsEnd(){
  max_coeff = (float)amplitude / (float)(max_timing - zero_timing);
	min_coeff = (float)amplitude / (float)(zero_timing - min_timing);

  // Serial.println("channel \t| amplitude \t | min_timing \t| zero_timing \t| max_timing \t| min_coeff \t| max_coeff");
  // Serial.print(channelNumber);Serial.print("\t\t| ");
  // Serial.print(amplitude);Serial.print("\t\t| ");
  // Serial.print((int)min_timing);Serial.print("\t\t| ");
  // Serial.print((int)zero_timing);Serial.print("\t\t| ");
  // Serial.print((int)max_timing);Serial.print("\t\t| ");
  // Serial.print(min_coeff);Serial.print("\t\t| ");
  // Serial.println(max_coeff);



}