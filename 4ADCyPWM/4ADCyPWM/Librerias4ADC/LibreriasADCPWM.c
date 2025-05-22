/*
 * LibreriasADCPWM.c
 *
 * Created: 5/4/2025 10:39:34 PM
 *  Author: rodri
 */ 

 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "LibreriasADCPWM.h"
 	char buffer[10];
	uint8_t indice;
void Setup(){
	cli();
	
	DDRD    = 0xEF; //Leds de salida donde se mostrara ADCH
	PORTD   = 0b10000000;
	DDRB    = 0x00; //Solo para un boton pero se configura todo como entrada
	PORTB   = 0xFF;
	
	ADMUX = (1 << REFS0); // AVcc como referencia. Quitar la configuracion de ADLAR cuando se usen servos
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
	
	// Timer1 – Servo 1
    DDRB |= (1 << DDB1) | (1 << DDB2);  // D9 (OC1A) y D10 (OC1B)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // Prescaler 8
    ICR1 = 40000; // 20ms -> TOP = 40000 (16MHz / 8 / 50Hz)

    // Timer0 – Servo 2 y 3
    DDRD |= (1 << DDD6) | (1 << DDD5); // OC0A y OC0B
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
    //TCCR0B = (1 << CS01); // Prescaler 8

    // Timer2 – Servo 4 y 5
    DDRB |= (1 << DDB3);  // OC2A
    DDRD |= (1 << DDD3);  // OC2B
    TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
	
	//Configuracion de la comunicacion serial
	DDRD |= (1 << DDD1); // Configura TX (D1) como salida
	DDRD &= ~(1 << DDD0); // Configura RX (D0) como entrada
	UCSR0A = 0; // Modo normal, sin doble velocidad ni flags activos
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Habilita interrupción por recepción, RX y TX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Modo asíncrono, sin paridad, 1 bit de stop, 8 bits de datos
	UBRR0 = 103; // Baud rate de 9600 para F_CPU = 16 MHz en modo normal
	
	sei();
}

 uint16_t LecturaADC(uint8_t PIN){ //Cambiar a uint16_t cuando se vaya a usar servos
	 ADMUX = (ADMUX & 0xF0) | PIN; // Seleccionar canal
	 ADCSRA |= (1 << ADSC); // Iniciar conversión
	 while (ADCSRA & (1 << ADSC)); // Esperar a que termine
	 return ADC; //cambiar a solo ADC cuando se vaya a usar servos
	 
 }
 
 void escrituraServos(uint8_t nservo, uint16_t Adc ){
	
	uint16_t ciclo;
	
    switch (nservo) {
        case 0: // Timer1, 16 bits
			
			Adc = Adc - 162; //Aqui se cancelara cualquier offset mecanico que tengan los potenciometros al instalarse
			if(Adc<0){Adc=0;}
			if(Adc>1024){Adc=0;}
				
			ciclo = ((uint32_t)Adc * 4000UL) / 700UL + 1000UL; // Se usa 614 en vez de 1023 porque nos interesa solo 180 grados del servo y no los 300
		
			if (ciclo < 1000) {ciclo = 1000;}
			if (ciclo > 5000) {ciclo = 5000;}
            OCR1A = ciclo; //Pin D9 fisico
		break;
        case 1:
			
			Adc = Adc - 0; //Aqui se cancelara cualquier offset mecanico que tengan los potenciometros al instalarse
			
			ciclo = ((uint32_t)Adc * 4000UL) / 614UL + 1000UL; // Se usa 614 en vez de 1023 porque nos interesa solo 180 grados del servo y no los 300
		
			if (ciclo < 1000) ciclo = 1000;
			if (ciclo > 5000) ciclo = 5000;
			OCR1B = ciclo; //Pin D10 fisico
			break;
        case 2:
			
			Adc = Adc - 0; //Aqui se cancelara cualquier offset mecanico que tengan los potenciometros al instalarse
			if(Adc<0){Adc=0;}
			if(Adc>1024){Adc=0;}
				
			ciclo = ((uint32_t)Adc * 4000UL) / 1023UL + 1000UL; // Se usa 614 en vez de 1023 porque nos interesa solo 180 grados del servo y no los 300
			
			if (ciclo < 1000) ciclo = 1000;
			if (ciclo > 5000) ciclo = 5000;
            OCR0B = (uint8_t)((ciclo - 1000)/64); //Pin D5 fisico
            break;
        case 3:
			
			Adc = Adc - 162; //Aqui se cancelara cualquier offset mecanico que tengan los potenciometros al instalarse
			if(Adc<0){Adc=0;}
			if(Adc>1024){Adc=0;}
				
			ciclo = ((uint32_t)Adc * 4000UL) / 614UL + 1000UL; // Se usa 614 en vez de 1023 porque nos interesa solo 180 grados del servo y no los 300
			
			if (ciclo < 1000) ciclo = 1000;
			if (ciclo > 5000) ciclo = 5000;
            OCR2A = (uint8_t)((ciclo - 1000)/64); //Pin D11 fisico
            break;
        case 4:
			
			Adc = Adc - 0; //Aqui se cancelara cualquier offset mecanico que tengan los potenciometros al instalarse
			
			ciclo = ((uint32_t)Adc * 4000UL) / 614UL + 1000UL; // Se usa 614 en vez de 1023 porque nos interesa solo 180 grados del servo y no los 300
			
			if (ciclo < 1000) ciclo = 1000;
			if (ciclo > 5000) ciclo = 5000;
            OCR2B = (uint8_t)((ciclo - 1000)/64); //Pin D3 fisico
            break;
    }
 }
 
 unsigned char LecturaEEPROM(unsigned int Address){
	 cli();
	 while(EECR & (1<<EEPE)); //Esperar a que se termine alguna escritura previa
	 EEAR = Address; //Definir la direccion del registro a leer
	 EECR |= (1<<EERE); //Se inicia la lectura del EEPROM
	 return EEDR; //Retornar la informacion obtenida
	 sei();
 }
 
 void EscrituraEEPROM(unsigned int Address, unsigned char Data){
	 cli();
	 while(EECR & (1<<EEPE)); //Esperar a que se termine alguna escritura previa
	 EEAR = Address; //Definir direccion a escribir
	 EEDR = Data; //Definir informacion a escribir
	 EECR |= (1<<EEMPE) | (1<<EEPE);//Se inicia escritura en EEPROM
	 sei();
 }
 
 void GuardarPosicion(unsigned char Servo0, unsigned char Servo1, unsigned char Servo2, unsigned char Servo3, unsigned char Servo4){
	 EscrituraEEPROM(0, LecturaEEPROM(5));
	 for (int i = 0; i <= 19; i++) {
		EscrituraEEPROM(i, LecturaEEPROM(i+5));
	}
	EscrituraEEPROM(20, Servo0);
	EscrituraEEPROM(21, Servo1);
	EscrituraEEPROM(22, Servo2);
	EscrituraEEPROM(23, Servo3);
	EscrituraEEPROM(24, Servo4);
	
 }
 
 void writeChar(char caracter){
	 
	 while((UCSR0A & (1 << UDRE0))==0);
	 UDR0 = caracter;
	 
 }
 
 void agregar_char(char DatA) {
	 if (indice < 9) {
		 buffer[indice++] = DatA;
		 buffer[indice] = '\0'; 
	 }
 }
 
 ISR(USART_RX_vect){
	uint8_t SingModo;
	uint8_t bandera_posicion;
	uint8_t SingServo0;
	uint8_t SingServo1;
	uint8_t SingServo2;
	uint8_t SingServo3;
	uint8_t bandera_m5;
	
	 char temporal = UDR0;
	 
	 if(SingModo){
		 if(temporal == '0'){
			 //Se eligio modo manual
			 Modo = 0;
			 
			 }else if(temporal == '1'){
			 //Se eligio modo eeprom
			 Modo = 1;
			 
			 }else if (temporal == '2'){
			 //Se eligio modo uart
			 Modo = 2;
			 }
			SingModo = 0;
		 
		 }else if(SingServo0){
		 if (temporal == 'H') {
			 UARTServo0 = atoi(&buffer[0]);
			 SingServo0 = 0;
			 // Reiniciar para siguiente número
			 indice = 0;
			 buffer[0] = '\0';
			 } else {
			 agregar_char(temporal);
		 }
		 }else if(SingServo1){
		 if (temporal == 'H') {
			 //= atoi(&buffer[0]);
			 SingServo1 = 0;
			 // Reiniciar para siguiente número
			 indice = 0;
			 buffer[0] = '\0';
			 } else {
			 agregar_char(temporal);
		 }
		 }else if(SingServo2){
		 if (temporal == 'H') {
			 // = atoi(&buffer[0]);
			 SingServo2 = 0;
			 // Reiniciar para siguiente número
			 indice = 0;
			 buffer[0] = '\0';
			 } else {
			 agregar_char(temporal);
		 }
		 }else if(SingServo3){
		 if (temporal == 'H') {
			 // = atoi(&buffer[0]);
			 SingServo3 = 0;
			 // Reiniciar para siguiente número
			 indice = 0;
			 buffer[0] = '\0';
			 } else {
			 agregar_char(temporal);
		 }
	 }
	 
	 
	 switch (temporal){//Aqui se determina de que feed se recibe informacion
		 case 'A':
		 SingModo = 1;
		 break;
		 
		 case 'B':
		 bandera_posicion = 1;
		 break;
		 
		 case 'C':
		 SingServo0 = 1;
		 indice = 0;
		 buffer[0] = '\0';
		 break;
		 
		 case 'D':
		 SingServo1 = 1;
		 indice = 0;
		 buffer[0] = '\0';
		 break;
		 
		 case 'E':
		 SingServo2 = 1;
		 indice = 0;
		 buffer[0] = '\0';
		 break;
		 
		 case 'F':
		 SingServo3 = 1;
		 indice = 0;
		 buffer[0] = '\0';
		 break;
		 
		 case 'G':
		 bandera_m5 = 1;
		 indice = 0;
		 buffer[0] = '\0';
		 break;
	 }
	 
 }
 