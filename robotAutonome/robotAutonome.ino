#include "robot_fr.h"

RobotFR r;

  int detections = 0;

// INITIALIZATION
int pinDetecteurGauche;
int pinDetecteurDroite;
void setup()
{
  pinDetecteurGauche=15;
  pinDetecteurDroite=14;

  pinMode(pinDetecteurGauche,INPUT);
  pinMode( pinDetecteurDroite,INPUT);
  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Robot Autonome en Francais");
  
  // Initialisation des moteurs
  Serial.println("Initialisation des moteurs..");
  r.initialiser_robot();
  // r.initialiser_capteurs();
  
  Serial.println("Debut du programme :");
}



//**********************************************************************
void loop()
{
  bool valDetecteurDroite;
  bool valDetecteurGauche;
  //code à exécuter en boucle

  delay(10);

  valDetecteurGauche=digitalRead(pinDetecteurGauche);
  valDetecteurDroite=digitalRead(pinDetecteurDroite);
  //int distance = r.mesurer_distance();

//  Serial.print("detection:");Serial.print(detections); 
//  Serial.print("| valGauche:");Serial.print(valDetecteurGauche);
//  Serial.print("| valDroite:");Serial.println(valDetecteurDroite);

  if ((valDetecteurGauche == valDetecteurDroite)) {
    r.avancer(20,5);
    Serial.println("avance");
  }
  else { 
    if (((valDetecteurGauche==HIGH)&&(valDetecteurDroite==LOW))){
      Serial.print("Gauche");
      r.tourner_gauche(20,5);
      detections = detections + 1;
    }
    if ((valDetecteurGauche==LOW)&&(valDetecteurDroite==HIGH)) {
      Serial.print("droite");
      r.tourner_droite(20,5);
      detections = detections + 1;
    }
  }
}
