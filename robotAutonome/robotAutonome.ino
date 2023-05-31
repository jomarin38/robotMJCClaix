#include "robot_fr.h"

RobotFR r;

// INITIALIZATION
void setup()
{

  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Robot Autonome en Francais");
  
  // Initialisation des moteurs
  Serial.println("Initialisation des moteurs..");
  r.initialiser_robot();

  Serial.println("Debut du programme :");

  r.avancer(10,1);
  r.attendre(1);
  r.tourner_gauche(10,1);
  r.tourner_droite(10,1);
  r.attendre(1);
  r.reculer(10,1);

}




//**********************************************************************
void loop()
{
  //code à exécuter en boucle
  /*
  int distance = r.mesurer_distance();
  Serial.print("Distance mesuree :");Serial.println(distance);
  if (distance>10) {
    r.avancer(10,1);
  }
  else {
    r.attendre(1);
  }
  */

}
