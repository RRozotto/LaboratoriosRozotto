;
; PostLab02HexTimerRozotto.asm
;
; Created: 2/18/2025 12:50:27 PM
; Author : Rodrigo Rozotto
; Descripcion: Diseño de un contador binario de 4 bits en el que cuenta cada segundo 
; hasta un limite, determinado por otro contador hexadecimal mostrado con el 7 segmentos
; que aumenta y disminuye con dos botones. Cuando el valor del primer contador el igual
; al limite se enciende un led de alarma.
;

 .include "M328PDEF.inc"
 .cseg
 .org 0x0000

 //configuracion de la pila
	LDI R16, LOW(RAMEND)
	OUT SPL, R16
	LDI R16, HIGH(RAMEND)
	OUT SPH, R16

//Configuracion del Microcontrolador
SETUP:
	// Se ajusta la frecuencia del micro a 125 kHz
	LDI R16, (1 << CLKPCE)
	STS CLKPR, R16
	LDI R16, 0x07 
	STS CLKPR, R16

	// Inicializamos el Timer0 con prescaler 256
	LDI R16, (1 << CS02) // Prescaler = 256
	OUT TCCR0B, R16  
	LDI R16, 0xCF  // Cargamos TCNT0 = 207
	OUT TCNT0, R16 

	// Configurar puertos (DDRx, PORTx, PINx)
    // Configurar puerto C como entrada con pull-ups habilitados
    LDI     R16, 0x00
    OUT     DDRC, R16   // Configuramos puerto C como entrada
    LDI     R16, 0xFF
    OUT     PORTC, R16  // Habilitamos pull-ups
	//Configuramos el PORTD como salida
	LDI		R16, 0XFF
	OUT		DDRB, R16 //establecimos los puertos como salida.
	OUT		DDRD, R16 //establecimos los puertos como salida.  

	LDI		XH, 0x02        // Parte alta de puntero inicial 
    LDI		XL, 0x20        // Parte baja de puntero inicial (para obtener 0x0220)
    LDI		ZH, HIGH(MYDATA<<1)  
    LDI		ZL, LOW(MYDATA<<1)	//Puntero de los datos guardados en MYDATA

CARGAR_DATOS:
    LPM		R18, Z+  // Leer un dato de MYDATA
    CPI		R18, 0   // Si el dato es 0 (se termina de copiar)
    BREQ	DONE     // Terminar copia
    ST		X+, R18  // Guardarlo en la RAM
    RJMP	CARGAR_DATOS // Repetir
DONE:
; Datos en memoria de programa. Estos son todos los valores en hex de 
; como se configuro los numeros y palabras en el 7 segmentos en relacion
; a los pines del puerto D.
MYDATA: .DB 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70
        .DB 0x7F, 0x73, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0 //Este cero es para finalizar copia
	LDI		R18, 0x00
	LDI		R24, 0x00
//Programa principal
LOOP:

	IN      R22, PINC   // Leer el puerto C a 0ms
    CP      R23, R22
    BREQ    Continuar
    CALL    DELAY
    IN      R22, PINC   // Leer el puerto C a 20ms
    CP      R23, R22
    BREQ    Continuar
	MOV     R23, R22    // Guardando Estado nuevo de botones
	SBRC    R23, 0      // R16 = 0b11111110
	INC		R24		// Si el boton esta presionado aumenta
	SBRC    R23, 1      // R16 = 0b11111101
	DEC		R24		// Si el boton esta presionado aumenta
	ANDI	R24, 0x0F

Continuar:
	CP		R24, R18
	BREQ	AlarmaReset
Continuar1:
    IN      R16, TIFR0     // Lee el registro de interrupción del timer 0
    SBRS    R16, TOV0      // Lee el bit de desbordamiento
    RJMP    Continuar2           // Si no hay desbordamiento, sigue esperando
    SBI     TIFR0, TOV0    // Limpia el bit de desbordamiento del registro de interrupción
    LDI     R16, 0xCF      // Valor inicial del timer
    OUT     TCNT0, R16     // Carga el valor inicial del timer

    INC     R17            // Incrementa el contador extra
    CPI     R17, 10        // ¿Ha llegado a 10 desbordamientos? (1 seg)
    BRNE    Continuar2           // Si no, sigue esperando más interrupciones

    LDI     R17, 0x00      // Reinicia el contador extra
    INC     R18            // Incrementa el contador principal cada segundo
	ANDI	R18, 0x0F
    OUT     PORTB, R18     // Carga del registro al puerto D donde están los LEDs

Continuar2:
	LDI		YH, 0x02        // Parte alta de la dirección base en RAM (0x0220)
    LDI		YL, 0x20        // Parte baja de la dirección base en RAM

    ADD		YL, R24         // Sumar el desplazamiento al puntero Y
    ADC		YH, R0          // Manejar el acarreo si es necesario

    LD		R21, Y          // Cargar el dato de la RAM en R18
	OUT		PORTD, R21
    RJMP    LOOP           // Reinicio

AlarmaReset:
	LDI		R18, 0xFF	// Limpieza de R18 como reseteando (no es 0 porque sino lo primero que se mostraria seria 1)
	SBI		PORTB, 4	// Se enciende el led de alarma (no hay necesidad de limpiarlo porque PORTB se limpia con otra carga)
	RJMP	Continuar1  // Regresa a continuar el programa

// Subrutinas (que no son de interrupción)
DELAY:
    LDI     R18, 0
SUBDELAY1:
    INC     R18
    CPI     R18, 0
    BRNE    SUBDELAY1
    LDI     R18, 0
SUBDELAY2:
    INC     R18
    CPI     R18, 0
    BRNE    SUBDELAY2
    LDI     R18, 0
SUBDELAY3:
    INC     R18
    CPI     R18, 0
    BRNE    SUBDELAY3
    RET
