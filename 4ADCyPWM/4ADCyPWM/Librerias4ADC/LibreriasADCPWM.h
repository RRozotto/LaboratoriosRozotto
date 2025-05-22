/*
 * LibreriasADCPWM.h
 *
 * Created: 5/4/2025 10:35:18 PM
 *  Author: rodri
 */ 


#ifndef LIBRERIASADCPWM_H_
#define LIBRERIASADCPWM_H_

#include <stdint.h>

uint16_t UARTServo0 = 0;
uint16_t UARTServo1 = 0;
uint16_t UARTServo2 = 0;
uint16_t UARTServo3 = 0;
uint16_t UARTServo4 = 0;
uint8_t Modo;

void Setup();
uint16_t LecturaADC(uint8_t PIN);
void escrituraServos(uint8_t nservo, uint16_t Adc );
unsigned char LecturaEEPROM(unsigned int Address);
void EscrituraEEPROM(unsigned int Address, unsigned char Data);
void GuardarPosicion(unsigned char Servo0, unsigned char Servo1, unsigned char Servo2, unsigned char Servo3, unsigned char Servo4);
void writeChar(char caracter);

#endif /* LIBRERIASADCPWM_H_ */