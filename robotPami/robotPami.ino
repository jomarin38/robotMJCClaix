#include "robot_fr.h"
#include "PinChangeInt.h" // for RC reciver
//#include <Arduino.h>

RobotFR r;

#define ETAT_INDEFINI 0
#define ETAT_TOURNER_GAUCHE 1
#define ETAT_TOURNER_GAUCHE_LEGER 2
#define ETAT_TOURNER_GAUCHE_FORT 3

#define ETAT_ALLER_TOUT_DROIT 4

#define ETAT_TOURNER_DROITE 5
#define ETAT_TOURNER_DROITE_LEGER 6
#define ETAT_TOURNER_DROITE_FORT 7

unsigned char etat_nouveau;
unsigned char etat_courant; 
int detections = 0;

// INITIALIZATION
int pinDetecteurGauche;
int pinDetecteurDroite;

volatile  bool valDetecteurDroite;
volatile  bool valDetecteurGauche;
int sensorPin;
int sensorValue = 0;
int vitesse = -40;

void determiner_direction_analog(int value)
{


  if ((value <= 30)) {
    Serial.println("erreur ?");

  }
  else if ((value <= 200)) {
    etat_nouveau = ETAT_TOURNER_GAUCHE_FORT;
  //  Serial.println("tourner gauche fort");

  } else if ((value <= 400)) {
    etat_nouveau = ETAT_TOURNER_GAUCHE_LEGER;
  //  Serial.println("tourner gauche leger");
  }else if ((value <= 600)) {
    etat_nouveau = ETAT_ALLER_TOUT_DROIT;
  //  Serial.println("tout droit");
  }else if ((value <= 800)) {
    etat_nouveau = ETAT_TOURNER_DROITE_LEGER;
  //  Serial.println("tourner droite leger");

  }else {
    etat_nouveau = ETAT_TOURNER_DROITE_FORT;
  //  Serial.println("tourner droite fort");

  }
  
}

int determiner_direction_analog2(int value){
  int dir = 0;
  double facteur = 4;
  double dirTemp1 = 0;
  double dirTemp2 = 0;
  
  if (value < 30 ){
    dir = -255;
  }else{
//    dir = (value * 180 / 1024) - 90 ; 
//    dir = (value - 512) * 180 /1024 ;
dirTemp1 = (value - 512);
dirTemp2 = dirTemp1 * facteur *100 /1024.0 ;
dir = dirTemp2;
    Serial.print("value = ");Serial.print(value);
    Serial.print(" dirTemp1 = ");Serial.print(dirTemp1);Serial.print(" dirTemp2 = ");Serial.print(dirTemp2);
    Serial.print(" dir = ");Serial.println(dir);
 
    //  delay(200);
    if(-30 < dir && dir < 30){
      vitesse = -20 * facteur;
    } else {
      vitesse = -20 *facteur;
    }
//    Serial.print("Valeurs calculees: vitesse = ");Serial.print(vitesse);Serial.print(" dir = ");Serial.print(dir);
//    Serial.print(" value = ");Serial.println(value);
  }
  return dir;
}
void setup()
{
  etat_nouveau = ETAT_ALLER_TOUT_DROIT;
  etat_courant = ETAT_INDEFINI; 
  sensorPin = A5;
//  pinDetecteurGauche=14;
//  pinDetecteurDroite=15;
//
//  pinMode(pinDetecteurGauche,INPUT);
//  pinMode( pinDetecteurDroite,INPUT);



  Serial.begin(115200); // Serial output to console
  delay(200);
  Serial.println("Programme Robot Autonome en Francais");
  
  // Initialisation des moteurs
  Serial.println("Initialisation des moteurs..");
  r.initialiser_robot();
  
  Serial.println("Debut du programme :");

//  PCintPort::attachInterrupt((pinDetecteurGauche), appelGaucheEvent,CHANGE);
//  PCintPort::attachInterrupt((pinDetecteurDroite), appelDroiteEvent,CHANGE);
}

void loop()
{
  int dir = 0;
  //code à exécuter en boucle
  delay(10);

  
//  Serial.print("Valeur analogique:");Serial.println(sensorValue);
  sensorValue = analogRead(sensorPin);

//  valDetecteurGauche=digitalRead(pinDetecteurGauche);
//  valDetecteurDroite=digitalRead(pinDetecteurDroite);



  dir = determiner_direction_analog2(sensorValue);
//  Serial.print("Valeur analogique:");Serial.println(sensorValue);
//  Serial.print("Valeur direction:");Serial.println(dir);

  if(dir > -255){
     r.bouger(vitesse, dir);
  }

}


//void loop()
//{
//  //code à exécuter en boucle
//  //delay(100);
//
//  
////  Serial.print("Valeur analogique:");Serial.println(sensorValue);
//  sensorValue = analogRead(sensorPin);
//
////  valDetecteurGauche=digitalRead(pinDetecteurGauche);
////  valDetecteurDroite=digitalRead(pinDetecteurDroite);
//
//
//
//  determiner_direction_analog(sensorValue);
//  if(etat_nouveau != etat_courant)
//  {
//    if(detections < 10) {
//      detections = detections + 1;
//    } else {
//      etat_courant = etat_nouveau;  
//      detections = 0;
//
//      if ((etat_nouveau == ETAT_ALLER_TOUT_DROIT)) {
//        r.bouger(-25, 0);
//        //Serial.println("avance");
//   
//      }
//      else { 
//        if (etat_nouveau == ETAT_TOURNER_GAUCHE_LEGER){
//          //Serial.println("Gauche leger");
//          r.bouger(-25, 40);
//        }else if (etat_nouveau == ETAT_TOURNER_GAUCHE_FORT){
//          //Serial.println("Gauche fort");
//          r.bouger(-25, 70);
//        }else if (etat_nouveau == ETAT_TOURNER_DROITE_LEGER) {
//          //Serial.println("droite leger");
//          r.bouger(-25, -40);
//        }else if (etat_nouveau == ETAT_TOURNER_DROITE_FORT) {
//          //Serial.println("droite fort");
//          r.bouger(-25, -70);
//        } else {
//          //Serial.println("erreur");
//        }
//      }
//    }
//  }

  
//  determiner_direction();
//  if(etat_nouveau != etat_courant)
//  {
//    etat_courant = etat_nouveau;  
//
//    if ((etat_nouveau == ETAT_ALLER_TOUT_DROIT)) {
//      r.bouger(-50, 0);
//      Serial.println("avance");
//      detections = detections + 1;
//
//    }
//    else { 
//      if (etat_nouveau == ETAT_TOURNER_GAUCHE){
//        Serial.println("Gauche");
//        r.bouger(-40, 50);
//        detections = detections + 1;
//      }
//      if (etat_nouveau == ETAT_TOURNER_DROITE) {
//        Serial.println("droite");
//        r.bouger(-40, -50);
//        detections = detections + 1;
//      }
//    }
//  }
