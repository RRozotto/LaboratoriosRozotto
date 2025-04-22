#include <avr/io.h>
#include "LibreriaServo.h"
#define F_CPU 16000000UL

void IniciarServo(void)//Funcion para configurar el PWM que manejara al servo
{
	// Se configura el pin PB1 para el servo
	DDRB |= (1 << PB1) | (1 << PB2);
	
	DDRD |= (1 << PD7);
	
	// Se configura el Timer1 en modo Fast PWM
	TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Se configura OC1A y el modo PWM
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Modo Fast con prescaler 8

	ICR1 = 40000;  // Valor maximo para que el ciclo sea de 20ms (especifico para el servo)
	OCR1A = SerBottom;
	OCR1B = SerBottom;
}

void AsignarValor(uint16_t Grados)
{
	if (Grados < SerBottom){
		Grados = SerBottom;
	}
	
	if (Grados > SerTop){
		Grados = SerTop;
	}
	
	OCR1A = Grados;
}

void AsignarValor2(uint16_t Grados2)
{
	if (Grados2 < SerBottom) Grados2 = SerBottom;
	if (Grados2 > SerTop) Grados2 = SerTop;
	OCR1B = Grados2;
}

