;
; PostLab03 Rozotto.asm
;
; Created: 2/24/2025 12:25:27 PM
; Author : Rodrigo Rozotto
; Descripcion: Se nos pidio hacer un contador de segundos dentro de un minutos
; por ende se necesitan dos displays, uno par unidades y el otro para decenas
; de 00 a 59. Este debe estar multiplexado, y por otro lado debe seguir funcionando
; el contador de 4 bits que trabaja con dos botones. Para los conteos de ambos se usan
; interrupciones


 .include "M328PDEF.inc"
 .cseg
 .org 0x0000
	RJMP INICIO  
//Vector de interrupcion de botones
.org PCI0addr
    RJMP IntBotones
//Vector de interrupcion del timer
.org 0x0020
	RJMP TMR0_ISR

 //configuracion de la pila
INICIO:
    LDI R18, LOW(RAMEND)
    OUT SPL, R18
    LDI R18, HIGH(RAMEND)
    OUT SPH, R18

//Configuracion del Microcontrolador
CONFIG:
    CLI  // Se desactiva interrupciones globales 

	// Configurando PORTB como entrada con pull-ups habilitados
    LDI R18, 0x00
    OUT DDRB, R18 
    LDI R18, 0xFF
    OUT PORTB, R18 

    // Configurando PORTC y PORTD como salida
    LDI R18, 0xFF
    OUT DDRC, R18 
    OUT DDRD, R18

	// Se ajusta la frecuencia del micro a 125 kHz
	LDI R16, (1 << CLKPCE)
	STS CLKPR, R16
	LDI R16, 0x03 
	STS CLKPR, R16

	// Inicializamos el Timer0 con prescaler 64
	LDI R16, (1 << CS01) | (1 << CS00) // Prescaler = 64
	OUT TCCR0B, R16  
	LDI R16, 100  // Cargamos TCNT0 = 217
	OUT TCNT0, R16 

	//Se habilita la interrupcion del timer 0
	LDI R16, (1 << TOIE0)
	STS TIMSK0, R16

	//Se habilita la interrupcion de pines
	LDI R18, (1 << PCINT0) | (1 << PCINT1)
    STS PCMSK0, R18 
    LDI R18, (1 << PCIE0)
    STS PCICR, R18 

	LDI		R18, 0x00
	LDI		XH, 0x02        // Parte alta de puntero inicial 
    LDI		XL, 0x20        // Parte baja de puntero inicial (para obtener 0x0220)
    LDI		ZH, HIGH(MYDATA<<1)  
    LDI		ZL, LOW(MYDATA<<1)	//Puntero de los datos guardados en MYDATA

CARGAR_DATOS:
    LPM		R18, Z+  // Leer un dato de MYDATA
    CPI		R18, 0   // Si el dato es 0 (se termina de copiar)
    BREQ	DONE     // Terminar copia
    ST		X+, R18  // Guarda en la RAM
    RJMP	CARGAR_DATOS 
DONE:
// Datos en memoria de programa. Estos son todos los valores en hex de 
// como se configuro los numeros y palabras en el 7 segmentos en relacion
// a los pines del puerto D.
MYDATA: .DB 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70
        .DB 0x7F, 0x73, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0 //Este cero es para finalizar copia
	
	LDI		R18, 0x00
	LDI		R19, 0x00
	LDI		R23, 0x00

	SEI  // Se rehabilitan las interrupciones globales
//Programa principal
LOOP:
	LDI     YH, 0x02       // Parte alta de la dirección base en RAM (0x0220)
    LDI     YL, 0x20       // Parte baja de la dirección base en RAM

    ADD     YL, R18        // Sumar el conteo de unidades al puntero Y

    LD      R21, Y         // Se saca de la RAM el pinout de la unidad para el primer display
    OUT     PORTD, R21	   // Se carga el pinout en el puerto hacia ambos displays
	LDI		R30, 0x02	   //0b00000010 el primer display tiene tierra, por lo cual enciende
	OR		R30, R19	   //Se hace OR para usar tanto el conteo de 4 bits y los dos pines multiplexacion juntos
	OUT		PORTC, R30	   //Se carga el registro con los dos primeros bits de activacion y los sig 4 de conteo
	CALL    DELAY

	LDI     YH, 0x02       // Parte alta de la dirección base en RAM (0x0220)
    LDI     YL, 0x20       // Parte baja de la dirección base en RAM

    ADD     YL, R23        // Sumar el conteo de decenas al puntero Y

    LD      R21, Y         // Se saca de la RAM el pinout de la decena para el segundo display
    OUT     PORTD, R21	   // Se carga el pinout en el puerto hacia ambos displays
	LDI		R30, 0x01      //0b00000001 el segundo display tiene tierra, por lo cual enciende
	OR		R30, R19	   //Se hace OR para usar tanto el conteo de 4 bits y los dos pines multiplexacion juntos
	OUT		PORTC, R30	   //Se carga el registro con los dos primeros bits de activacion y los sig 4 de conteo
	CALL    DELAY

	LDI		R30, 0x00
	OUT     PORTD, R30	   //Se lleva a cero los pines del display
	OR		R30, R19
	OUT		PORTC, R30	   //Se lleva a cero las tierras de multiplexacion (pero se mantiene el conteo de R19)
    RJMP    LOOP           // Reinicio

//Rutina de interrupcion de timer
TMR0_ISR:

    LDI     R16, 100       // Cargar nuevamente el valor de TCNT0
    OUT     TCNT0, R16
    INC     R17            // Incrementa el contador extra
    CPI     R17, 200       // ¿Ha llegado a 200 desbordamientos? (1 seg)
    BRNE    Continuar2     // Si no, sigue esperando más interrupciones
    LDI     R17, 0x00      // Reinicia el contador extra
    INC     R18            // Incrementa el contador de unidades cada segundo
    CPI     R18, 0x0A      // Comparar R18 con 10 (0x0A)
    BRGE    Overflow1      // Si R18 >= 10, saltar a Overflow1

Continuar2:
    RETI

Overflow1:
    LDI     R18, 0x00      // Reinicia el contador de unidades
	INC		R23			   // Incrementa el contador de decenas
	CPI     R23, 0x06      // Comparar R23 con 6 (0x06)
    BRGE    Overflow2      // Si R23 >= 6, saltar a Overflow2
    RJMP    Continuar2     // Continúa con la ejecución normal

Overflow2:
	LDI		R23, 0x00	   // Reinicia el contador de decenas
	RJMP	Continuar2

// Rutina de interrupción de botones
IntBotones:    
    LSR R19
	LSR R19
	IN R24, PINB	

    SBRS R24, 0  // Si PB0 está presionado aumenta el contador
    RJMP DecContador
	INC	R19
	ANDI R19, 0x0F // Mascara para limpiar los bits innecesarios

DecContador:
	SBRS R24, 1  // Si PB1 está presionado decrementa el contador
	RJMP Finalizar
    DEC R19
    ANDI R19, 0x0F // Mascara para limpiar los bits innecesarios

Finalizar:
    LSL R19
	LSL R19
    RETI

DELAY:
    LDI     R25, 0
SUBDELAY1:
    INC     R25
    CPI     R25, 0
    BRNE    SUBDELAY1
    LDI     R25, 0
SUBDELAY2:
    INC     R25
    CPI     R25, 0
    BRNE    SUBDELAY2
    LDI     R25, 0
SUBDELAY3:
    INC     R25
    CPI     R25, 0
    BRNE    SUBDELAY3
    RET