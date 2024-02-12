#ifndef ARDUINOSTEPPERCONTROLER
#define ARDUINOSTEPPERCONTROLER
 
#define MOTORENABLE 8
#define MICROSTEPPING 8   // 8 or 16 for 1/8 or 1/16 driver microstepping (default:16)
 
#define ZERO_SPEED 65535
#define MAX_ACCEL 7        // Maximun motor acceleration (MAX RECOMMENDED VALUE: 8) (default:7)
#define MAX_ACCEL_SAFE 1  
#define MAX_RESET_CLK 20000

#include <Arduino.h>

class StepperControler 
{
	
	protected : 
	
		static int8_t motorEnable;
		static int8_t microstepping;
		static int8_t zeroSpeed;
		static int8_t maxAccel;
		static int8_t maxAccellSafe;
		static int8_t pinMotorDir[2];
		static int8_t pinMotorStep[2];
		static float max_accel;
		static int16_t motorSpeed[2]; // Actual speed of motors
    static long lastSpeedChange[2]; // Actual speed of motors
	
	public:
	
		StepperControler();
		~StepperControler() {}

		static void init();
		static void enableMotor(bool enable);
	
		static void setMotorSpeed(int i, int16_t tspeed);
		
		static void setPinMotorStep(int i, int8_t p) {pinMotorStep[i]=p;}
		static void setPinMotorDir(int i, int8_t p) {pinMotorDir[i]=p;}
		static void setMaxAccel(float maxValue) {max_accel=maxValue;}

		static int8_t getPinMotorStep(int i) {return pinMotorStep[i];}
	
};

#endif
