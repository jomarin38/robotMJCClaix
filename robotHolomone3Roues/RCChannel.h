#ifndef RCCHANNEL_H
#define RCCHANNEL_H
#include <Arduino.h>

enum Channel_type {
    TYPE_PROPORTIONNEL,         // Valeur par défaut = 0
    TYPE_SWITCH_2_POSITIONS,    // Valeur suivante = 1
    TYPE_SWITCH_3_POSITIONS,    // Valeur suivante = 2
    TYPE_NOT_DEFINE    // Valeur suivante = 3
};


class RCChannel {
private:
    int channelNumber;          // Numéro de la voie (ex. 1, 2, 3, etc.)
    char channelType;
    float value;                // Valeur actuelle de la voie (ex. entre -1.0 et 1.0 ou 0 à 100%)
    uint32_t start;             //timing de debut de pwm
    volatile uint16_t end = 0;  //timing de fin de pwm
    char pinValue;
    int amplitude         = 100;//valeur que l on cherche a avoir en mesure ex [-100, 100] 
    
    void calc();
public:
    float zero_timing     = 0;  //valeur du zero mesure sur la pwm
    float max_timing      = 0;  //valeur du min mesure sur la pwm
    float min_timing      = 0;  //valeur du max mesure sur la pwm
    float max_coeff       = 0;  //Valeur max mesure correspondant à amplitude
    float min_coeff       = 0;  //Valeur min mesure correspondant à -amplitude

    // Constructeur
    RCChannel(char channelType = Channel_type::TYPE_PROPORTIONNEL, char initialPinValue = 0);

    void init();                //init des pins et de l interruption

    void calcInterrupt();

    // Getters
    int getChannelNumber() const;
    int getPinValue() const;
    float getValue();
    float getZeroTiming();
    float getMinTiming();
    float getMaxTiming();
    float getMinCoeff();
    float getMaxCoeff();

    float getEnd();
  

    // Setters
    void setValue(float newValue);
    void setPinValue(char newValue);
    void setChannelType(char newValue);

    void setChannelNumber(int newValue);

    // Autres méthodes (exemple)
    void reset();  // Réinitialise la voie à sa valeur par défaut
    void calibrateZero();
    void calibrateZeroAverage(int nSamples);
    void calibrateInputs();
    void calibrateInputsEnd();
};

#endif // RCCHANNEL_H
