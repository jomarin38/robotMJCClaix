

//*******************************************************************************//
// Association des entrées du L298N, aux sorties utilisées sur notre Arduino Uno //
//*******************************************************************************//
// moteur avant
#define borneAvENA        11      // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneAvIN1        12      // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneAvIN2        10      // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
#define borneAvIN3        7       // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
#define borneAvIN4        8       // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
#define borneAvENB        9       // On associe la borne "ENB" du L298N à la pin D5 de l'arduino

// moteur arriere
#define borneArENA        5       // On associe la borne "ENA" du L298N à la pin D10 de l'arduino
#define borneArIN1        16      // On associe la borne "IN1" du L298N à la pin D9 de l'arduino
#define borneArIN2        17      // On associe la borne "IN2" du L298N à la pin D8 de l'arduino
//#define borneArIN3        16      // On associe la borne "IN3" du L298N à la pin D7 de l'arduino
//#define borneArIN4        15      // On associe la borne "IN4" du L298N à la pin D6 de l'arduino
//#define borneArENB        3      // On associe la borne "ENB" du L298N à la pin D5 de l'arduino

//radio
#define LEFT_RIGHT_IN_PIN     3//6
#define UP_DOWN_IN_PIN        18//4
#define LAT_LEFT_RIGHT_PIN    2
#define SW5_PIN               19//A1//14
#define SW6_PIN               20//13
#define SW7_PIN               21//20//15
#define SW8_PIN               13//21//16

//tick moteur
#define TICK_AVANT_GAUCHE     63

//GPIO vers pinces
#define OUT_CHANNEL_0_0 35
#define OUT_CHANNEL_0_1 37
#define OUT_CHANNEL_1 39
#define OUT_CHANNEL_2 41//3
#define OUT_CHANNEL_3_0 29
#define OUT_CHANNEL_3_1 31

//2 3 18 19 20 21
