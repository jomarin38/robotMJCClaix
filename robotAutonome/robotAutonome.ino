#include "robot_fr.h"

RobotFR r;

  int detections = 0;

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
  if (distance>30) {
    r.avancer(50,100);
  }
  else {
      if (detections>=3){

          r.tourner_droite(20,2100);

          detections = detections + 1;
        
      }
      if (detections<3) {
          r.tourner_gauche(20,2100);

          detections = detections + 1;
        
      }
    
  }
  

}
