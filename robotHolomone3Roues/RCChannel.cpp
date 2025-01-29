#include "RCChannel.h"

// Constructeur
RCChannel::RCChannel(char initialChannelTypeValue, char initialPinValue)
    : channelType(initialChannelTypeValue), pinValue(initialPinValue) {}


void RCChannel::init(){
  //1500 valeur du zero avec RC MJC 
  min_timing = 1500;
  pinMode(channelNumber, INPUT);
}

//attention fonction appelée dans le handler d interruptions
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
// Getter pour le numéro du GPIO
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
    else if (temp < 0){
      return temp * min_coeff;
    }
  } else if(channelType == Channel_type::TYPE_SWITCH_2_POSITIONS){
    if(temp < 0 && abs(temp) > (amplitude / 5) )
      return -amplitude;
    else
      return amplitude;    

  } else if(channelType == Channel_type::TYPE_SWITCH_3_POSITIONS){
    if(temp > amplitude/3)
      return amplitude;
    else if(temp < -amplitude / 3){
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
//setGeneric
void RCChannel::setValues(int newChannelNumberValue, char newPinValue, char newChannelTypeValue) {
  setPinValue(newPinValue);
  setChannelType(newChannelTypeValue);
  setChannelNumber(newChannelNumberValue);
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

void RCChannel::calibrateZero(){
  		zero_timing += end;
}

void RCChannel::calibrateZeroAverage(int nSamples){
  		zero_timing = zero_timing/nSamples;
      min_timing = zero_timing; //pour donner une valeur à min sinon min reste eqal à zero.
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

  Serial.println("channel \t| amplitude \t | min_timing \t| zero_timing \t| max_timing \t| min_coeff \t| max_coeff");
  Serial.print(channelNumber);Serial.print("\t\t| ");
  Serial.print(amplitude);Serial.print("\t\t| ");
  Serial.print((int)min_timing);Serial.print("\t\t| ");
  Serial.print((int)zero_timing);Serial.print("\t\t| ");
  Serial.print((int)max_timing);Serial.print("\t\t| ");
  Serial.print(min_coeff);Serial.print("\t\t| ");
  Serial.println(max_coeff);



}