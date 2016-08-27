/* 
 * File:   Main.h
 * Author: Carlos
 * Author: Juan Diego
 *
 * Created on August of 2016
 */

#include <30F2010.h>

/*Function Declaration*/

/******************************************************************************/
// CLOCK
/******************************************************************************/
#DEVICE ADC = 10
#DEVICE ICSP = 1 
#USE delay(crystal=40MHz)

/******************************************************************************/
// FUSES               
/******************************************************************************/
#FUSES NOPROTECT // Código no protegido de la lectura
#FUSES NOWDT // Watchdog desactivado
#FUSES NOCKSFSM // Monitor de fallos desactivado
#FUSES NOPUT // Temporizador power up desactivado
#FUSES NOBROWNOUT // Desactivado reset de apagado
#FUSES XT_PLL4 // Modo oscilador XT con 4X PLL

/******************************************************************************/
// REGISTERS
/******************************************************************************/

// PWM Registers
#WORD PTCON = 0x01C0
#BIT PETEN = PTCON.15
#WORD PWMCON1 = 0x01C8
#WORD PWMCON2 = 0x01CA
#WORD OVDCON = 0x01D4
#WORD PTPER = 0x01C4
#WORD PDC1 = 0x01D6
#WORD INTCON1 = 0x0080
#BIT NSTDIS = INTCON1.15
#WORD INTCON2 = 0x0082

// ADC Registers
#WORD ADPCFG = 0x02A8
#WORD ADCON1 = 0x02A0
#WORD ADCON2 = 0x02A2
#WORD ADCON3 = 0x02A4
#WORD ADCHS = 0x02A6
#WORD ADCSSL = 0x02AA
#BIT VCFG0 = ADCON2.13
#BIT VCFG1 = ADCON2.14
#BIT VCFG2 = ADCON2.15
#BIT ADON = ADCON1.15
#BIT SAMC4 = ADCON3.12
#BIT SAMC3 = ADCON3.11
#BIT SAMC2 = ADCON3.10
#BIT SAMC1 = ADCON3.9
#BIT SAMC0 = ADCON3.8
#BIT ADRC = ADCON3.7
#BIT ADCS0 = ADCON3.0
#BIT ADCS1 = ADCON3.1
#BIT ADCS2 = ADCON3.2
#BIT ADCS3 = ADCON3.3
#BIT ADCS4 = ADCON3.4
#BIT ADCS5 = ADCON3.5
#WORD ADCBUFF0 = 0x0280
#WORD ADCBUFF1 = 0x0282
#WORD ADCBUFF2 = 0x0284
#WORD ADCBUFF3 = 0x0286

/******************************************************************************/
// DEFINES
/******************************************************************************/
#define HIGH 1
#define LOW 0

// -- Defines PINS

// -- -- PINS for LED
#define LEDR PIN_C13
#define LEDG PIN_C14
#define LEDB PIN_D0

// -- -- PINS for Switch
#define SW PIN_D1

// -- -- PINS for Display
#define A PIN_E5
#define B PIN_E2
#define C PIN_E3
#define D PIN_E4
#define DP PIN_E0
#define D1 PIN_F3
#define D2 PIN_F2
#define D3 PIN_E8

/******************************************************************************/
// CONFIGURE PINOUT
/******************************************************************************/
#USE FIXED_IO (C_outputs=PIN_C14,PIN_C13)
#USE FIXED_IO (D_outputs=PIN_D0)
#USE FIXED_IO (E_outputs=PIN_E0,PIN_E2,PIN_E3,PIN_E4,PIN_E5,PIN_E8)
#USE FIXED_IO (F_outputs=PIN_F2,PIN_F3)

/******************************************************************************/
// MODES
/******************************************************************************/
#define STOP 0x00 // Carga OFF
#define RUN 0x01 // Carga ON
#define LOAD 0x00 // Carga
#define VOLTAGE 0x01 // Voltage
#define TEMPC 0x02 // Temperatura
#define LASTMODE 0x02 // Último modo

/******************************************************************************/
// VARIABLES
/******************************************************************************/
#define QEICOUNT 0xA64 // Máximo contador encoder
#define TIMEPUSH 0x03 // Tiempo de pulsación del botón
#define COUNTAVG 120 // Número de muestras para la media de visualización
#define TEMPPROT 70 // Temperatura de protección para paro de carga
#define VOLTDIVUP 9.31 // Valor de la resistencia superior del divisor de senseo de tensión
#define VOLTDIVDOWN 1 // Valor de la resistencia inferior del divisor de senseo de tensión
