#ifndef LibreriaServo
#define LibreriaServo
#include <stdint.h> 

// **Definición de los valores de pulso para el servo (1ms a 2.5ms)**
#define SerBottom 1000   // Pulso mínimo (1ms)
#define SerTop 5000   // Pulso máximo (2.5ms)
void IniciarServo(void);	// Función para inicializar el servo
void AsignarValor(uint16_t Grados); // Función para ajustar el ciclo de trabajo del servo
void AsignarValor2(uint16_t Grados2);


#endif