/*
 * File:   PreLab12.c
 * Author: jorge
 *
 * Created on 17 de mayo de 2022, 10:37 PM
 */
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>             // int8_t, unit8_t

#define _XTAL_FREQ 4000000
int APAGADO = 0;

void __interrupt() isr (void){
    if(PIR1bits.ADIF){                  // Interrupción por ADC
        if(ADCON0bits.CHS == 0){        // Interrupción por AN0
            PORTC = ADRESH;             // Mostrar los bits más signifiativos
        }
        PIR1bits.ADIF = 0;              // Limpiar la bandera de ADC
    }
    if(INTCONbits.RBIF){        // Verificar si se activa la bandera de interrupcion de TMR0
        if(!PORTBbits.RB0){     // Verificar si fue actividad por presionar RB0
            APAGADO = 1;        // Se detiene la conversión
            PORTEbits.RE0 = 1;  // Indicador de bajo consumo
            SLEEP();            // Se duerme el PIC
        }
        else if(!PORTBbits.RB1){// Verificar si se activa la bandera de interrupcion de PORTB 
            APAGADO = 0;        // Se activa la conversión
            PORTEbits.RE0 = 0;  // Indicador de consumo normal
        }
        INTCONbits.RBIF = 0;    // Limpiar la bandera de interrupcion de PORTB
    }
    return;
}

void setup(void){
    ANSEL = 0b00000001;         // AN0 como entrada analógica
    ANSELH = 0;                 // I/O digitales
    
    TRISA = 0b00000001;         // RA0 como entrada analógica y demás como salidas
    PORTA = 0x00;               // Se limpia PORTA
    
    TRISBbits.TRISB0 = 1;       // RBO como entrada
    TRISBbits.TRISB1 = 1;       // RB1 como entrada
    
    TRISC = 0b00000000;         // PORTC como salida
    PORTC = 0x00;               // Se limpia PORTC
    TRISE = 0b00000000;         // PORTE como salida
    PORTE = 0x00;               // Se limpia PORTE
    
    OPTION_REGbits.nRBPU = 0;   // Habilitar Pull-ups
    WPUBbits.WPUB0 = 1;         // Pull-up en RB0
    WPUBbits.WPUB1 = 1;         // Pull-up en RB1

    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;   // 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno

    // Configuración de interrupciones
    PIR1bits.ADIF = 0;          // Limpiar la bandera de ADC
    PIE1bits.ADIE = 1;          // Habilitar interrupciones de ADC
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de periféricos
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.RBIE= 1;         // Habilitar interrupciones de PORTB
    IOCBbits.IOCB0 = 1;         // Habilitar interrupciones de cambio de estado en RB0
    IOCBbits.IOCB1 = 1;         // Habilitar interrupciones de cambio de estado en RB1
    
    // Configuración ADC
    ADCON0bits.ADCS = 0b11;         // FRC -> Funcione como SLEEP
    ADCON1bits.VCFG0 = 0;           // VDD
    ADCON1bits.VCFG1 = 0;           // VSS
    ADCON0bits.CHS = 0b0000;        // Selecciona el AN0
    ADCON1bits.ADFM = 0;            // Justificador a la izquierda
    ADCON0bits.ADON = 1;            // Habilitar modulo ADC
    __delay_us(40);                 // Sample time
    
    return;
}


void main(void) {
    setup();
    while(1){
        if (!APAGADO){              // Funciona solo cuando está encendido
            if(ADCON0bits.GO == 0){             
                ADCON0bits.GO = 1;  // Iniciar proceso de conversión
            }
        }
    }    
    return;
}   