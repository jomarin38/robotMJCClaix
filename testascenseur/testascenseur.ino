#include <TM1637.h>
#include <SpeedyStepper.h>
// version du 28/01/2025 
// commande du moteur pas à pas pour l'ascenseur à courroie
// avec commande des servo via ada1411 en I2c
// RAF : 4 canaux pour telecommande
// voies A1, 11, 9 et 10
// comment concilier commandes directes avec sequenceur ?
// etat d'attente avec lecture etat des 5 pattes de la RC
// si patte pince ==1 et etat== ouvertes,   fermeture pince, memorisation état,  retour à l'état d'attente.
// si patte pince ==0 et etat== fermées,   ouverture pince, memorisation état,  retour à l'état d'attente.
// idem pour inclinaison
//si patte 3==1  sequence de construction commence avec attrapage planche, inclinaison, 
// attente patte4 == 1 pour eccartement, montée, centrage, pose,...
// attente patte4==0 pour retour en position initiale.



// voie 1 ouverture/fermeture pinces
// voie 2 inclinaison
// voie 3 declenchement construction

//RC canal 2      bleu      Hold            A1
//RC canal 3      orange    End Stop -Z     11
// RC canal 2  jaune     End Stop -X     9
// RC canal 2  blanc      End Stop -Y     10
// branchements sur CNC :
//RC canal 1      gris     Abort            A0
//RC canal 2      bleu      Hold            A1
//RC canal 3      orange    End Stop -Z     11
//RC canal 4      jaune     ZDir            7
// afficheur CLK  jaune     End Stop -X     9
// afficheurDIO  blanc      End Stop -Y     10
// servo 0 = inclinaison mat
// servo 1 = pince intérieurG
// servo 2 = pince intérieurD
// servo 10 = pince éxtérieurG // voie 4 HS
// servo 3 = pince éxtérieurD
// servo 5 = pince planche G
// servo 6 = pince planche D
// servo 7 = rotation pince éxtérieur 
#define inclinaison_mat 0
#define pince_interieurG 1
#define pince_interieurD 2
#define pince_exterieurG 10  
#define pince_exterieurD 3
#define pince_plancheG 5
#define pince_plancheD 6
#define rotation_pinces_exterieursG 7
#define rotation_pinces_exterieursD 8
const int buttonPin = 7;     // voie inter fin de course  = Z DIR
// les voies X et Z ne sont pas utilisés
//   libre pour COMMUNO2
// X Dir (5) , ca servait pour l'inter de démarrage
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
//#define DEBUG
//#include "robot.h"
//#include "RCControl.h"
//#include "TM1637.h"
//#define CDE_RELAIS   4 // patte arduino branchement relais commande lumière
#define PWM_RELAIS   7 // patte arduino branchement telcommande voie hG
#define PWM_AFFICHEUR   11
#define CMD_MIN_MAX_FILTRE 5 //valeur pour filtrer autour du zero pour av/ar et G/D
#define COMMUNO1 4  // patte UNo pour comm vers arduino pince avant
#define COMMUNO2 9    // autres pinces valeur à trouver
#define TEMPS_AVANT_DEGUISEMENT 100000
const int MOTOR_STEP_PIN = 12;
const int MOTOR_DIRECTION_PIN = 13;
const int ENABLE =8;
const int stepfor1cm = 333;

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

#define SERVOPINCE 11  //bouton gauche de la RC
#define SERVOINCLINE 9
#define SERVOX A1
#define SERVOY 10

//X STEP et Z STEP
//#define CLK 9//pins definitions for TM1637 and can be changed to other ports
//#define DIO 10
//RCControl control;
//Robot r;
// create the stepper motor object
SpeedyStepper stepper;
//TM1637 tm1637(CLK,DIO);
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
float throttle;
float steering;
float throttle0;
float steering0;
float deltav= 1; // valeur d'acceleration maxi
float commande_pinces;
//float commande_afficheur;
float commande_score;
int rcCalibrationPin = 2;  //  X sur CNC, à boucler au 5V
int score = 42;
uint32_t temps_dernier_score = 0;
int drapeau=0;
int buttonState = 0;
bool premierefois=true;
#define TEMPS_ENTRE_2_AFFICHAGES_MS 150   //300
// INITIALIZATION **************************************************
void setup()
{
//tm1637.init();
//tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;displayInt(42);
//displayInt(1515); // pour test
//pinMode(rcCalibrationPin, INPUT_PULLUP);
//pinMode(COMMUNO1, OUTPUT); // comm avec arduino pinces
//pinMode(COMMUNO2, OUTPUT); // comm avec arduino pinces
//pinMode(PWM_RELAIS, INPUT);// lecture manette RC pour leds
//pinMode(PWM_AFFICHEUR, INPUT);// lecture manette RC pour leds
Serial.begin(115200); // Serial output to console
delay(100);
Serial.println("Programme Testascenseur version du 23/01/2025");
Serial.println("dans Gaviou/MJC/coupe robot 2025");
Serial.println("Départ");
// STEPPER MOTORS INITIALIZATION
Serial.println("Steper motors initialization...");
pinMode(buttonPin, INPUT_PULLUP);
pinMode(ENABLE, OUTPUT);
pinMode(MOTOR_STEP_PIN, OUTPUT);
pinMode(MOTOR_DIRECTION_PIN, OUTPUT);

// declaration des 4 entrees RC
pinMode(SERVOPINCE, INPUT_PULLUP);
pinMode(SERVOINCLINE, INPUT_PULLUP);
pinMode(SERVOX, INPUT_PULLUP);
pinMode(SERVOY, INPUT_PULLUP);



// pour le controleur des servo
pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(5);
// initialisation
// CHASSE AUX POTS
// CAD attrape planche en position passante (+ 10mm par rapport à la butée basse)
// pinces à pots non verrouillées
// pinces à pots extérieures rentrées
// pivot ascenceur vers l'avant
Serial.println("chasse aux pots");
    pwm.setPWM(inclinaison_mat, 0, 290);// 290 postion avant 430 position arrière
    pwm.setPWM(pince_interieurG, 0, 300);// 210 verrouillé 300 deverrouillé
    pwm.setPWM(pince_interieurD, 0, 210);// 300 verrouillé 210 deverrouillé 
    pwm.setPWM(pince_exterieurG, 0, 210);// 300 verrouillé210 deverrouillé 
    pwm.setPWM(pince_exterieurD, 0, 300);// 210 verrouillé 300 deverrouillé
    pwm.setPWM(pince_plancheG, 0, 140);// 360 verrouillé 140 deverrouillé 
    pwm.setPWM(pince_plancheD, 0, 360);//140 verrouillé 360 deverrouillé
    pwm.setPWM(rotation_pinces_exterieursG, 0, 260);// 260 centré 375 sorti
    pwm.setPWM(rotation_pinces_exterieursD, 0, 330);// 330 centré 220 sorti
// init pos ascenseur, max en bas
init_stepper();
Serial.println("sortie :");

// on monte un peu, pour etre en postion d'attrapage des planches
monte(5500);
//while(1);// attente pour test
delay(1500);
}
//****************************************************************************************
//**********************************************************************
void loop()
{
int etatlu;

etatlu = digitalRead(SERVOX);
Serial.print("automate :");
Serial.print(etatlu);
if(etatlu==1)  // on lance la construction
  {
    automate();
}
etatlu = digitalRead(SERVOX);
Serial.print("fin construction :");
Serial.print(etatlu);
if(etatlu==1)  // on lance la construction
  {
delay(1500);
}
//--------
etatlu = digitalRead(SERVOPINCE);
Serial.print("pinces :");
Serial.print(etatlu);
if(etatlu==1)  // on verrouille
  {
    pwm.setPWM(pince_interieurG, 0, 210);// 210 verrouillé 300 deverrouillé
    pwm.setPWM(pince_interieurD, 0, 300);// 300 verrouillé 210 deverrouillé 
    pwm.setPWM(pince_exterieurG, 0, 300);// 300 verrouillé210 deverrouillé  
    pwm.setPWM(pince_exterieurD, 0, 210);// 210 verrouillé 300 deverrouillé
}
else
  {
    pwm.setPWM(pince_interieurG, 0, 300);// 210 verrouillé 300 deverrouillé
    pwm.setPWM(pince_interieurD, 0, 210);// 300 verrouillé 210 deverrouillé 
    pwm.setPWM(pince_exterieurG, 0, 210);// 300 verrouillé210 deverrouillé 
    pwm.setPWM(pince_exterieurD, 0, 300);// 210 verrouillé 300 deverrouillé 
       // descente ascenseur suite à depose baderolle
    if(premierefois==true)
    {premierefois=false;
    delay(3000);
    descend(5000);
      }
}
delay(50);
etatlu = digitalRead(SERVOINCLINE);
Serial.print(" incline :");
Serial.println(etatlu);
if(etatlu==1)  // on incline
  {
    pwm.setPWM(inclinaison_mat, 0, 430);// 290 postion avant 430 position arrière
}
else
  {
    pwm.setPWM(inclinaison_mat, 0, 290);// 290 postion avant 430 position arrière
}
delay(50);





}

void loop_old()
{
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//ATTRAPER PLANCHE
// descente attrape planche,position basse 
// verrouillage pinces à planche
// verrouillage pinces à pots
// pinces à pots extérieures rentrées
// pivot ascenceur vers l'avant
delay(2000);
descend(400);
Serial.println("attrape planche");
delay(1500);
pwm.setPWM(pince_plancheG, 0, 360);// 360 verrouillé 140 deverrouillé   
pwm.setPWM(pince_plancheD, 0, 140);//  140 verrouillé 360 deverrouillé
delay(1000);
pwm.setPWM(pince_interieurG, 0, 210);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_interieurD, 0, 300);// 300 verrouillé 210 deverrouillé 
pwm.setPWM(pince_exterieurG, 0, 300);//  
pwm.setPWM(pince_exterieurD, 0, 210);// 210 verrouillé 300 deverrouillé

//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// PIVOTER POUR TRANSPORT
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);
//monte(666);//?????
Serial.println("pivotement arrière");
pwm.setPWM(inclinaison_mat, 0, 430);// 290 postion avant 430 position arrière
delay(500);
//while(1);// attente pour test


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ECCARTER LES POTS EXTERIEURS
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);

Serial.println("eccartement pinces ext");
pwm.setPWM(rotation_pinces_exterieursG, 0, 375);// 260 centré 375 sorti
pwm.setPWM(rotation_pinces_exterieursD, 0, 220);// 330 centré 220 sorti    
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// PLACER LE GRADIN
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'avant
delay(1000);
Serial.println("pivotement avant");
pwm.setPWM(inclinaison_mat, 0, 290);// 290 postion avant 430 position arrière
delay(500);
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// MONTER LES POTS EXTERIEURS ET LA PLANCHE SUP
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures sorties

delay(1000);
 // vers le haut
Serial.println("montée");
monte(5500);
delay(1000);
//r.setMovingSpeeds(0,0);
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// RECENTRER LES POTS EXTERIEURS
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);

Serial.println("re centrage pinces ext");
pwm.setPWM(rotation_pinces_exterieursG, 0, 260);// 260 centré 375 sorti
pwm.setPWM(rotation_pinces_exterieursD, 0, 330);// 330 centré 220 sorti
//while(1);// attente pour test



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// POSER LES POTS EXTERIEURS ET LA PLANCHE SUP
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);
Serial.println("pose etage sup");
pwm.setPWM(pince_interieurG, 0, 300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_interieurD, 0, 210);// 300 verrouillé 210 deverrouillé
pwm.setPWM(pince_exterieurG, 0, 210);// 210 verrouillé 300 deverrouillé 
pwm.setPWM(pince_exterieurD, 0,300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_plancheG, 0, 140);// 360 verrouillé 140 deverrouillé  
pwm.setPWM(pince_plancheD, 0,360);//  140 verrouillé 360 deverrouillé


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// MONTER UN PEU POUR POUVOIR RECULER
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures sorties

delay(1000);
Serial.println("montée sup");
//monte(100);  on suppose que ca tombe

delay(50);


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// redescente
delay(1000);
descend(5200);
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// CHASSE AUX POTS
// CAD attrape planche en position passante (+ 10mm par rapport à la butée basse)
// pinces à pots non verrouillées
// pinces à pots extérieures rentrées
// pivot ascenceur vers l'avant
delay(1000);
Serial.println("chasse aux pots");
pwm.setPWM(inclinaison_mat, 0, 430);// 290 postion avant 430 position arrière
pwm.setPWM(pince_interieurG, 0, 300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_interieurD, 0, 210);// 300 verrouillé 210 deverrouillé 
pwm.setPWM(pince_exterieurG, 0, 210);// 
pwm.setPWM(pince_exterieurD, 0, 300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_plancheG, 0, 140);// 360 verrouillé 140 deverrouillé  
pwm.setPWM(pince_plancheD, 0, 360);//  140 verrouillé 360 deverrouillé
pwm.setPWM(rotation_pinces_exterieursG, 0, 260);// 260 centré 375 sorti
pwm.setPWM(rotation_pinces_exterieursD, 0, 330);// 330 centré 220 sorti
// init pos ascenseur, max en bas
init_stepper();
monte(500);
Serial.println("sortie :");
delay(500);
while(1);// attente
}
//******************************************************************
//******************************************************
void displayInt(int value) {
  int reste = 0;
  int milliers = value/1000;
  reste = value - milliers*1000;
  int centaine = reste/100;
  reste = reste - centaine*100;
  int dixaine = reste/10;
  int unite = reste - dixaine * 10;
 // tm1637.display(0,milliers);
  //tm1637.display(1,centaine);
  //tm1637.display(2,dixaine);
  //tm1637.display(3,unite);
}
//********************************************************************
void init_stepper(void)  // pour aller cherche le fin course de l'ascenseur
{  
buttonState = digitalRead(buttonPin);
Serial.println("init stepper");
digitalWrite(ENABLE,0);
digitalWrite(MOTOR_DIRECTION_PIN, LOW);
while(buttonState ==1) // recherche butée basse ascenseur
  {
   buttonState = digitalRead(buttonPin); 
  digitalWrite(MOTOR_STEP_PIN, HIGH);
  //delayMicroseconds(2000);//150
  delay_1us();
  digitalWrite(MOTOR_STEP_PIN, LOW);
  delayMicroseconds(2000);//150
}
}
//**********************************************************************
void delay_1us()
{
__asm__ __volatile__ (
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop" "\n\t"
      "nop");
}
//**********************************************************
void monte(int pas)
{
  // 333 pas pour 1 cm
long numberOfSteps = pas;
int n;
digitalWrite(ENABLE,0);
digitalWrite(MOTOR_DIRECTION_PIN, HIGH);  // high motée
for(n = 0; n < (numberOfSteps/2); n++) 
{
  digitalWrite(MOTOR_STEP_PIN, HIGH);
  //delayMicroseconds(2000);//150
  delay_1us();
  digitalWrite(MOTOR_STEP_PIN, LOW);
  //delayMicroseconds(1000);//150
  //delayMicroseconds(300);//150
  delayMicroseconds(abs(500-n/2)+500);
  //Serial.println(n);
}
for(n = (numberOfSteps/2); n >0 ; n--) 
{
  digitalWrite(MOTOR_STEP_PIN, HIGH);
  //delayMicroseconds(2000);//150
  delay_1us();
  digitalWrite(MOTOR_STEP_PIN, LOW);
  //delayMicroseconds(1000);//150
  //delayMicroseconds(300);//150
  delayMicroseconds(abs(500-n/2)+500);
  //Serial.println(n);
}
}
//**********************************************************
void descend(int pas)
{
  // 333 pas pour 1 cm
long numberOfSteps = pas;
int n;
digitalWrite(ENABLE,0);
digitalWrite(MOTOR_DIRECTION_PIN, LOW);  // high motée
for(n = 0; n < (numberOfSteps/2); n++) 
{
  digitalWrite(MOTOR_STEP_PIN, HIGH);
  delay_1us();
  digitalWrite(MOTOR_STEP_PIN, LOW);
  delayMicroseconds(abs(500-n/2)+500);
}
for(n = (numberOfSteps/2); n >0 ; n--) 
{
  digitalWrite(MOTOR_STEP_PIN, HIGH);
  delay_1us();
  digitalWrite(MOTOR_STEP_PIN, LOW);
  delayMicroseconds(abs(500-n/2)+500);

}
}

void automate(void)
{
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//ATTRAPER PLANCHE
// descente attrape planche,position basse 
// verrouillage pinces à planche
// verrouillage pinces à pots
// pinces à pots extérieures rentrées
// pivot ascenceur vers l'avant
delay(500);
descend(200);
Serial.println("attrape planche");
delay(500);
pwm.setPWM(pince_plancheG, 0, 360);// 360 verrouillé 140 deverrouillé   
pwm.setPWM(pince_plancheD, 0, 140);//  140 verrouillé 360 deverrouillé
delay(1000);
pwm.setPWM(pince_interieurG, 0, 210);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_interieurD, 0, 300);// 300 verrouillé 210 deverrouillé 
pwm.setPWM(pince_exterieurG, 0, 300);//  
pwm.setPWM(pince_exterieurD, 0, 210);// 210 verrouillé 300 deverrouillé

//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// PIVOTER POUR TRANSPORT
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);
//monte(666);//?????
Serial.println("pivotement arrière");
pwm.setPWM(inclinaison_mat, 0, 430);// 290 postion avant 430 position arrière
delay(500);
//while(1);// attente pour test


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ECCARTER LES POTS EXTERIEURS
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);

Serial.println("eccartement pinces ext");
pwm.setPWM(rotation_pinces_exterieursG, 0, 375);// 260 centré 375 sorti
pwm.setPWM(rotation_pinces_exterieursD, 0, 220);// 330 centré 220 sorti    
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// PLACER LE GRADIN
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'avant
delay(1000);
Serial.println("pivotement avant");
pwm.setPWM(inclinaison_mat, 0, 290);// 290 postion avant 430 position arrière
delay(500);
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// MONTER LES POTS EXTERIEURS ET LA PLANCHE SUP
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures sorties

delay(1000);
 // vers le haut
Serial.println("montée");
monte(5500);
delay(1000);
//r.setMovingSpeeds(0,0);
//while(1);// attente pour test

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// RECENTRER LES POTS EXTERIEURS
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);

Serial.println("re centrage pinces ext");
pwm.setPWM(rotation_pinces_exterieursG, 0, 260);// 260 centré 375 sorti
pwm.setPWM(rotation_pinces_exterieursD, 0, 330);// 330 centré 220 sorti
//while(1);// attente pour test



//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// POSER LES POTS EXTERIEURS ET LA PLANCHE SUP
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures rentrées
// pivotement ascenceur vers l'arrière
delay(1000);
Serial.println("pose etage sup");
pwm.setPWM(pince_interieurG, 0, 300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_interieurD, 0, 210);// 300 verrouillé 210 deverrouillé
pwm.setPWM(pince_exterieurG, 0, 210);// 210 verrouillé 300 deverrouillé 
pwm.setPWM(pince_exterieurD, 0,300);// 210 verrouillé 300 deverrouillé
pwm.setPWM(pince_plancheG, 0, 140);// 360 verrouillé 140 deverrouillé  
pwm.setPWM(pince_plancheD, 0,360);//  140 verrouillé 360 deverrouillé


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// MONTER UN PEU POUR POUVOIR RECULER
// attrape planche en position basse (+ 10mm par rapport à la butée basse)
// pinces à pots  verrouillées
// pinces à pots extérieures sorties

delay(1000);
Serial.println("montée sup");
//monte(100);  on suppose que ca tombe
}
