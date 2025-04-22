/*
 * Laboratorio5 - 231306.c
 *
 * Created: 4/9/2025 10:14:54 AM
 * Author : Rodrigo Rozotto
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LibreriaServo.h"

uint16_t LeerADC(uint8_t Pin)
{
	ADMUX = (ADMUX & 0xF0) | (Pin & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void Setup(void)
{
	cli();
	DDRD    = 0xFF;
	PORTD   = 0x00;
	
	DDRB |= (1 << DDB3);
	
	ADMUX = (1 << REFS0) | (1 << MUX1); // 5v como ref y A2 como entrada del ADC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
	
	TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20); // Modo Fast PWM
	TCCR2B = (1 << CS21);  // Prescaler de 8
	OCR2A = 0;  // Inicializar ciclo de trabajo a 0 (apagado)
	
	sei();
}

int main(void)
{
	uint16_t adcValue, adcValue2, adcValue3, Grados, Grados2, brilloLED;
	Setup();
	IniciarServo();
	
	//void loop
	while (1)
	{
		adcValue = LeerADC(2); // A2
		adcValue2 = LeerADC(1); // A1
		adcValue3 = LeerADC(0); // A0
		
		Grados = ((uint32_t)adcValue * 4000) / 1023 + 1000;
		Grados2 = ((uint32_t)adcValue2 * 4000) / 1023 + 1000;
		brilloLED = ((uint32_t)adcValue3 * 255UL) / 1023;
		//brilloLED = (brilloLED * 64) / 8;  // Ajuste proporcional al cambio de prescaler
		
		
		AsignarValor2(Grados2);
		//_delay_ms(20);
		AsignarValor(Grados);
		//_delay_ms(20);
		OCR2A = (uint8_t)brilloLED;
		//PWM_Software(brilloLED); // Simula el PWM manualmente
	}
}


