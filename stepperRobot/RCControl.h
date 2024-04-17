#ifndef RCCONTROL
#define RCCONTROL

#include <Arduino.h>

struct CalibrationData {
  float zero_throttle;
  float zero_steering;
  float max_throttle_coeff;
  float max_steering_coeff;
  float min_throttle_coeff;
  float min_steering_coeff;
};

class RCControl 
{
	
	protected : 
	
		static float zero_throttle;
		static float zero_steering;

		static float max_throttle_coeff;
		static float max_steering_coeff;
		static float min_throttle_coeff;
		static float min_steering_coeff;
		
		static int throttle_amplitude;
		static int steering_amplitude;
	
	public:
	
		RCControl();
		~RCControl() {}

		static void init();
		
		static void calibrateZero();
		static void calibrateInputs();

    static void initializeFromEEPROM();
    static void writeToEEPROM();
		
		static void setTrottleAmplitude(int a) {throttle_amplitude = a;}
		static void setSteeringAmplitude(int a) {steering_amplitude = a;}
		
		static float getThrottle();
		static float getSteering();
    static float getRelais();
    static float getAfficheur();

};

#endif
