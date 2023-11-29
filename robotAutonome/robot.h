#ifndef ROBOT
#define ROBOT

#include <Arduino.h>

class Robot {

	protected :
		bool smooth;
		int16_t accdur;

	public :
		Robot(){smooth=false;accdur=200;}
		~Robot(){}
		
		void init();
		void move(int16_t speed, int16_t duration);
		void turn(int16_t speed, int16_t duration);
		void stop(int16_t duration);
		void setMovingSpeeds(float speedNeed, float turnNeed);
		
		void setSmooth(bool b) {smooth = b;}
		
	protected : 
		void moveSmooth(int16_t speed, int16_t duration, int16_t accdur);
		void moveAgressive(int16_t speed, int16_t duration);

};

#endif
