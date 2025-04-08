/*
 * Laboratorio4.c
 *
 * Created: 4/1/2025 9:35:53 AM
 * Author : Rodrigo Rozotto
 * Descripcion: Laboratorio 4 donde se trata de utilizar un potenciometro para leer una señal analogica con el ADC para 
 * convertirlo a informacion de 10 bits. Se cuenta con un contador de 8 bits que se maneja con botones, y representado
 * con leds. Se activa un led de alarma cuando el valor del ADC sea mayor al del contador de 8 bits.
 */ 

// Libraries
#include <avr/io.h>
#include <avr/interrupt.h>

//Declarando variables que se utilizaran
uint8_t ConteoBo;
uint8_t PrevState;
uint8_t Multi;
uint16_t adc_value; // Guardará el valor del ADC

// Informacion de pines a encender para cada digito que existe en el sistema hexadecimal
const uint8_t hex_digits[16] = { 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70,0x7F, 0x73, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47 };
	
/****************************************/
// Function prototypes
void setup();

/****************************************/
// Main Function
int main(void)
{
	setup(); // Configurar MCU
	
	//Aqui es tecnicamente un void loop
	while (1)
	{
		// Se extraen los primeros 4 bits del ADC y se multiplican por 16, y se suman a los siguientes 4 bits. De esta forma se consigue el valor real
		uint8_t ValorReal = ((adc_value >> 6) & 0x0F) * 16 + ((adc_value >> 2) & 0x0F);
		//Este if se encargara de hacer la comparativa entre informacion del ADC y el conteo con botones
		if (ConteoBo < ValorReal)
		{
			PORTC |= (1 << PC3); //Si es mayor se enciende el led
		}else{
			PORTC &= ~(1 << PC3); //Si es menor se apaga el led
		}
		
		switch (Multi) //Switch case para realizar la multiplexacion
		{
		case 0:
			PORTC |= (1 << PC0); // Activar transistor en PC0
			PORTC &= ~(1 << PC2); // Desactivar transistor en PC2
			PORTD = ConteoBo; // Mostrar valor en el display
			break;
		case 1:
			PORTC &= ~(1 << PC0); // Desactivar transistor en PC0
			PORTC |= (1 << PC1); // Activar transistor en PC1
			PORTD = hex_digits[(adc_value >> 6) & 0x0F]; // Extraer los 4 bits altos del ADC y mapear
			break;
		case 2:
			PORTC |= (1 << PC2); // Activar transistor en PC2
			PORTC &= ~(1 << PC1); // Desactivar transistor en PC1
			PORTD = hex_digits[(adc_value >> 2) & 0x0F]; // Extraer los 4 bits bajos del ADC y mapear
			break;
		}
	}
}

/****************************************/
// NON-Interrupt subroutines
void setup()
{
	cli(); // Desactivar interrupciones globales

	DDRD  = 0xFF; // Puerto D como salida
	PORTD = 0x00;
	DDRC  = 0xFF; // Puerto C como salida
	PORTC = 0x00;
	DDRB  = 0x00; // Puerto B como entrada
	PORTB = 0xFF; // Pull-ups activados

	// Configuración del ADC
	ADMUX  = (1 << REFS0) | (1 << MUX2) | (1 << MUX1); // Ref: AVCC, Canal ADC6
	ADCSRA = (1 << ADEN)  | (1 << ADIE)  | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC); // Iniciar conversión ADC

	// Configuración de interrupciones de pines
	PCICR  = (1 << PCIE0);
	PCMSK0 = (1 << PCINT0) | (1 << PCINT1); //PB0 y PB1

	// Configuración del Timer 0
	TCCR0A  = 0x00;
	TCCR0B  = (1 << CS01); // Prescaler reducido para mayor velocidad
	TCNT0   = 100;
	TIMSK0  = (1 << TOIE0);

	PrevState = 0;
	sei(); // Habilitar interrupciones globales
}

/****************************************/
// Interrupt routines
ISR(TIMER0_OVF_vect)
{
	//Esta interrupcion del timer es unicamente para la multiplexacion
	// cambiando a alta velocidad el valor de Multi pero que no pase de 2
	TCNT0 = 100;
	Multi++;
	if(Multi==3){
		Multi=0;
	}
}

ISR(PCINT0_vect)
{
	uint8_t currentState = PINB; // Leer estado actual de los pines

	// Detectar botones en flanco de bajada
	if ((PrevState & (1 << PINB0)) && !(currentState & (1 << PINB0))) {
		ConteoBo++; // Incrementar contador
	}
	if ((PrevState & (1 << PINB1)) && !(currentState & (1 << PINB1))) {
		ConteoBo--; // Decrementar contador
	}

	PrevState = currentState;
}

ISR(ADC_vect)
{
	adc_value = ADC; // Se lee el valor completo de 10 bits
	ADCSRA |= (1 << ADSC); // Nueva conversión ADC
}