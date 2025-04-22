#ifndef LibreriaServo
#define LibreriaServo
#include <stdint.h> 

// **Definici�n de los valores de pulso para el servo (1ms a 2.5ms)**
#define SerBottom 1000   // Pulso m�nimo (1ms)
#define SerTop 5000   // Pulso m�ximo (2.5ms)
void IniciarServo(void);	// Funci�n para inicializar el servo
void AsignarValor(uint16_t Grados); // Funci�n para ajustar el ciclo de trabajo del servo
void AsignarValor2(uint16_t Grados2);


#endif