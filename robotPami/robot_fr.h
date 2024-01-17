#ifndef ROBOTFR
#define ROBOTFR


#include "robot.h"

class RobotFR : Robot {

	public :
		RobotFR(){smooth=false;accdur=200;}
		~RobotFR(){}
		
		void initialiser_robot();
    void initialiser_capteurs();

		void avancer(int16_t vitesse, int16_t duree);
    void reculer(int16_t vitesse, int16_t duree);

		void tourner_gauche(int16_t vitesse, int16_t duree);
    void tourner_droite(int16_t vitesse, int16_t duree);
    
		void attendre(int16_t duree);

		void bouger(float vitesse, float rotation);

    int mesurer_distance();

};

#endif
