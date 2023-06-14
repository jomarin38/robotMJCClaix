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
  r.initialiser_capteurs();

  Serial.println("Debut du programme :");

  /*
  r.avancer(20,11);
  r.attendre(1);
  r.tourner_gauche(20,2);
  r.attendre(1);
  r.avancer(20,7);
  r.attendre(20);
  */
}



//**********************************************************************
void loop()
{
  //code à exécuter en boucle
  
  int distance = r.mesurer_distance();
  Serial.print("Distance mesuree :");Serial.println(distance);
  if (distance>10) {
    r.avancer(20,100);
  }
  else {
    r.attendre(1);
  }
  

}
