/* 
 * File:   Main.h
 * Author: Carlos
 * Author: Juan Diego
 *
 * Created on August of 2016
 */

#include <Main.h>
#include <Functions.h>

int mode = 0x00; // Modo de funcionamiento
int modeLoad = 0x00; // Modo de la carga
int8 time; // Tiempo de pulsación
int1 blink = 0x00; // Parpadeo LED
int16 LCD = 0x378; // Display 888
int dot1 = 0x01; // Punto en posición 1
int dot2 = 0x01; // Punto en posición 2
int dot3 = 0x01; // Punto en posición 3
int celsiusLCD = FALSE; // Mostrar símbolo de Celsius
volatile int display = 0x00; // Digito a pintar
int32 currSum = 0x00; // Sumatorio de corriente
int32 voltSum = 0x00; // Sumatorio de tensión
int32 tempSum = 0x00; // Sumatorio de temperatura
int16 countSum = 0x00; // Contador de muestras
int16 curr = 0x00; // Corriente
int16 volt = 0x00; // Tensión
int16 temperature = 0x00; // Temperatura

/* 
 * Interrupción para el BCD
 * Recorre todos las posiciones del número
 */
#int_TIMER1
void TIMER1_isr(void)
{
    clear_interrupt(int_TIMER1);
    set_timer1(0x0000);
    switch(display){
        case 0:
            bcd(LCD,1,dot1,0);
            display = 1;
            break;
        case 1:
            bcd(LCD,2,dot2,0);
            display = 2;
            break;
        case 2:
            bcd(LCD,3,dot3,celsiusLCD);
            display = 0;
            break;
    }
}

/* 
 * Interrupción para Switch
 */
#int_EXT2 
void  EXT2_isr(void) { 
    clear_interrupt(int_EXT2);
    
    if (!input_state(SW)) { // Pulsado
        ext_int_edge(2, L_TO_H); // De bajo a alto
        time = 0x01; // Reset del temporizador
        setup_timer2(TMR_INTERNAL | TMR_DIV_BY_256, 39100); // Desbordamiento cada segundo
    } else { // Sin pulsar
        ext_int_edge(2, H_TO_L); // De alto a bajo
        setup_timer2(TMR_DISABLED); // Paramos el contador
        changeMode(0); // Cambiamos de modo
    }
} 

/* 
 * Interrupción para el temporizador del Switch
 */
#int_TIMER2
void TIMER2_isr(void) {
    clear_interrupt(int_TIMER2);
    time++; // Sumar un segundo

    if (time >= TIMEPUSH) {
        ext_int_edge(2, H_TO_L); // Flanco del botón de alto a bajo
                                 // Así ignoramos la interrupción cuando se suelte
        setup_timer2(TMR_DISABLED); // Paramos el contador
        changeMode(1); // Cambiamos de modo
    }    
}

/* 
 * Interrupción para el temporizador del parpadeo del LED
 */
#int_TIMER3
void TIMER3_isr(void) {
    clear_interrupt(int_TIMER3);
    
    blink = !blink;
}

/* 
 * Interrupción del ADC
 */
#int_ADC1
void ADC_isr(void) {
    clear_interrupt(int_ADC1);
    // Corriente
    set_adc_channel(2); 
    currSum += read_adc();
    // Voltaje
    set_adc_channel(3);
    voltSum += read_adc();
    // Temperatura
    set_adc_channel(1);
    tempSum += read_adc();
    
    countSum++;
    
    if (countSum == COUNTAVG) {
        curr = currSum / COUNTAVG;
        currSum = 0;
        volt = voltSum / COUNTAVG;
        voltSum = 0;
        temperature = tempSum / COUNTAVG;
        tempSum = 0;
        
        countSum = 0;
    }
}

void main() {
    initMode(); // Inicializa la carga

    while(1) {
        // Carga activa
        if (modeLoad == RUN) {
            readEncoder();
            // Protección sobre temperatura
            if (convertTemperature(temperature) >= (TEMPPROT * 10)) {
                changeMode(0);
                qei_set_count(0); // Ponemos el contador a 0 por seguridad
            }
        }
        
        // Leer ADCs
        read_adc();
        
        switch(mode) {
            case LOAD: // Mostrar lectura Amperios
                changeRGB(0,1,-1); // Verde ON
                LCD = convertCurrent(curr);
                celsiusLCD = FALSE;
                break;
            case VOLTAGE: // Mostrar Tensión en Voltios
                changeRGB(-1,0,1); // Azul ON
                LCD = convertVoltage(volt);
                celsiusLCD = FALSE;
                break;
            case TEMPC: // Mostrar Temperatura en ºC
                changeRGB(1,-1,0); // Rojo ON
                LCD = convertTemperature(temperature);
                dot1 = FALSE;
                dot2 = FALSE;
                dot3 = FALSE;                
                celsiusLCD = TRUE;
                break; 
        }
    }
}