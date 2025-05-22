/*
 * 4ADCyPWM.c
 *
 * Created: 5/4/2025 5:54:02 PM
 * Author : rodri
 */ 
//Notas
/*El ADC tiene 8 canales ADC0-ADC7 (A0-A7 en pines)
Se genera un dato resultado de 10 bits guardados en ADCL y ADCH

Registros a utilizar:
ADMUX registro de multiplexado
ADCSRA registro de control A
ADCSRB registro de control B
ADCL y ADCH donde se guarda la informacion

Hablando del PWM, este depende de los timers y se tiene en pines especificos:
PB3 - OC2A
PB2 - OC1B
PB1 - OC1A
PD6 - OC0A
PD5 - OC0B
PD3 - OC2B
*/

//Inclusion de librerias
#include <avr/io.h>
#include <avr/interrupt.h>
//#include "LibreriasADCPWM.h"

//Declaracion de variables
uint8_t Modo = 0;
uint8_t Posicion = 0;
int16_t ServoPorBoton = 0;
uint8_t	PrevState;
uint8_t	PrevState1;
uint8_t PrevState2;



//Funcion principal
int main(void)
{
	
    Setup();
    while (1) 
    {
		uint8_t currentState = PINB;
		if ((PrevState1 & (1 << PINB0)) && !(currentState & (1 << PINB0))) {
			ServoPorBoton = ServoPorBoton+10; // Incrementar cual es el ServoPorBoton a leer
			if (ServoPorBoton >200){
				ServoPorBoton = 200;
			}
		}
		
		PrevState = currentState;
		
		uint8_t currentState2 = PIND;
		if ((PrevState2 & (1 << PIND7)) && !(currentState2 & (1 << PIND7))) {
			ServoPorBoton = ServoPorBoton-10; // Decrementar cual es el ServoPorBoton a leer
			if (ServoPorBoton < 0){
				ServoPorBoton = 0;
			}
		}
		PrevState2 = currentState2;
		
		switch(Modo){
			case 0: //Modo manual
			escrituraServos(0, LecturaADC(0));
			escrituraServos(1, LecturaADC(1));
			escrituraServos(2, LecturaADC(2));
			escrituraServos(3, LecturaADC(3));
			escrituraServos(4, ServoPorBoton);
			
			
			//Si se presiona el boton PB4 se guarda el estado actual del servo y se corren las posiciones anteriores
			currentState = PINB;
			if ((PrevState & (1 << PINB4)) && !(currentState & (1 << PINB4))) {
				//Funcion de escritura de posicion en el EEPROM por boton(debe correr los datos)
				GuardarPosicion(LecturaADC(0), LecturaADC(1), LecturaADC(2), LecturaADC(3), ServoPorBoton);
			}
		
			PrevState1 = currentState;
			
			break;
			
			case 1: //Modo EEPROM
			switch(Posicion){
				case 0:
				escrituraServos(0, LecturaEEPROM(0));
				escrituraServos(1, LecturaEEPROM(1));
				escrituraServos(2, LecturaEEPROM(2));
				escrituraServos(3, LecturaEEPROM(3));
				escrituraServos(4, LecturaEEPROM(4));
				break;
				
				case 1:
				escrituraServos(0, LecturaEEPROM(5));
				escrituraServos(1, LecturaEEPROM(6));
				escrituraServos(2, LecturaEEPROM(7));
				escrituraServos(3, LecturaEEPROM(8));
				escrituraServos(4, LecturaEEPROM(9));
				break;
				
				case 2:
				escrituraServos(0, LecturaEEPROM(10));
				escrituraServos(1, LecturaEEPROM(11));
				escrituraServos(2, LecturaEEPROM(12));
				escrituraServos(3, LecturaEEPROM(13));
				escrituraServos(4, LecturaEEPROM(14));
				break;
				
				case 3:
				escrituraServos(0, LecturaEEPROM(15));
				escrituraServos(1, LecturaEEPROM(16));
				escrituraServos(2, LecturaEEPROM(17));
				escrituraServos(3, LecturaEEPROM(18));
				escrituraServos(4, LecturaEEPROM(19));
				break;
				
				case 4:
				escrituraServos(0, LecturaEEPROM(20));
				escrituraServos(1, LecturaEEPROM(21));
				escrituraServos(2, LecturaEEPROM(22));
				escrituraServos(3, LecturaEEPROM(23));
				escrituraServos(4, LecturaEEPROM(24));
				break;
			}
			break;
			
			case 2: //Modo Serial
			
			break;
		}
		

		
		/*uint8_t Lectura0 = LecturaADC(0);
		uint8_t Lectura1 = LecturaADC(1);
		uint8_t Lectura2 = LecturaADC(2);
		uint8_t Lectura3 = LecturaADC(3);
		
		switch(ServoPorBoton){
			case 0:
			PORTD = Lectura0;
			break;
			case 1:
			PORTD = Lectura1;
			break;
			case 2:
			PORTD = Lectura2;
			break;
			case 3:
			PORTD = Lectura3;
			break;
		}*/
		
		/*for (uint8_t i = 0; i < 4; i++) {
            uint16_t adc = LecturaADC(i); // A0–A3
            uint16_t duty = ((uint32_t)adc * 4000UL) / 1023UL + 1000UL;
            escrituraServos(i + 1, duty);
            //_delay_ms(10); // Pequeño retardo entre cada servo
        }*/
		    
    }
}

// Rutinas normales


//Rutinas de interrupcion