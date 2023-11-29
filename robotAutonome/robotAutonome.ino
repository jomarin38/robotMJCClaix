#include "robot_fr.h"

RobotFR r;

  int detections = 0;

// INITIALIZATION
int buttonpin;
int buttonpin1;
void setup()
{
//buttonpin=14;
//buttonpin1=15;
buttonpin=12;
buttonpin1=13;
pinMode(buttonpin,INPUT);
pinMode( +,INPUT);
  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Robot Autonome en Francais");
  
  // Initialisation des moteurs
  Serial.println("Initialisation des moteurs..");
  r.initialiser_robot();
 // r.initialiser_capteurs();

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
{bool val1;
bool val;
  //code à exécuter en boucle


  val=digitalRead(buttonpin);
val1=digitalRead(buttonpin1);
  //int distance = r.mesurer_distance();
  
Serial.print("val:");Serial.println(val);
Serial.print("val1:");Serial.println(val1);
  if ((val==LOW)&&(val1==LOW)) {
    r.reculer(50,100);
  }
  else { 
      if (((val==HIGH)&&(val1==LOW))){
        Serial.print("droite");
          r.tourner_droite(20,2100);

          detections = detections + 1;
        
      }
      if ((val==LOW)&&(val1==HIGH)) {
          Serial.print("gauche");
          r.tourner_gauche(20,2100);

          detections = detections + 1;
        
      }
    
  }
  

}
