#ifndef RCCONTROL
#define RCCONTROL

#include <Arduino.h>
#include "RCChannel.h"
#define NB_CHANNELS 8
// NB_CHANNELS * [zero, min_coeff, max_coeff]
struct CalibrationData {
  float data[NB_CHANNELS * 3];
};

class RCControl {
private: 

  char nbChannels;
//protected : 
	
	
public:
    RCChannel rchannel2[NB_CHANNELS];
	
		RCControl();

		void init();
		
		void calibrateZero();
		void calibrateInputs();

    void initializeFromEEPROM();
    void writeToEEPROM();
		    


    void debugInterrupt();
    void debugCalibration();

		float getThrottle();
		float getSteering();
    float getLatSteering();
    float getSW5();
    float getSW6();
    float getSW7();
    float getSW8();

};

#endif
