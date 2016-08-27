/* 
 * File:   Main.h
 * Author: Carlos
 * Author: Juan Diego
 *
 * Created on August of 2016
 */

extern int mode;
extern int modeLoad;
extern int dot1;
extern int dot2;
extern int dot3;
extern int celsiusLCD;
extern int1 blink;

int16 countEncoder = 0; // Estado del encoder

/*
 * Configura el color del LED de control
 * @param int1  red      -1 No varía
 *                        0 Encendido
 *                        1 Apagado
 * @param int1  green    -1 No varía
 *                        0 Encendido
 *                        1 Apagado
 * @param int1  blue     -1 No varía
 *                        0 Encendido
 *                        1 Apagado 
 * @return void
 */
void changeRGB (int1 red, int1 green, int1 blue) {
    if (blink == 0) { // Led encendido
        // Rojo
        if (red == 1) {
            output_high(LEDR);
        } else if (red == 0) {
            output_low(LEDR);
        }

        // Verde
        if (green == 1) {
            output_high(LEDG);
        } else if (green == 0) {
            output_low(LEDG);
        }

        // Azul
        if (blue == 1) {
            output_high(LEDB);
        } else if (blue == 0) {
            output_low(LEDB);
        }   
    } else { // Led apagado
        output_low(LEDR);
        output_low(LEDG);
        output_low(LEDB);
    }
}


/*
 * Inicializa la configuracion de la Dummy
 * @param void
 * @return void
 */
void initMode(void) {
    // Configuración de ADC
    setup_adc_ports(sAN1 | sAN2 | sAN3, VSS_VREF);
    setup_adc(ADC_CLOCK_DIV_32 | ADC_TAD_MUL_31);
    
    // Interrupciones
    // -- BCD
    setup_timer1(TMR_INTERNAL | TMR_DIV_BY_256, 0x00C3);
    set_timer1(0x0000);
    enable_interrupts(INT_TIMER1);
    // -- Temporizador Botón
    setup_timer2(TMR_DISABLED);
    enable_interrupts(INT_TIMER2);  
    // -- Temporizador LED
    setup_timer3(TMR_DISABLED);
    enable_interrupts(INT_TIMER3);     
    // -- INT2 (Switch)
    enable_interrupts(INT_EXT2);
    ext_int_edge(2, H_TO_L); // Alto a bajo
    // -- ADCs
    enable_interrupts(INT_ADC1);
    
    // Global
    enable_interrupts(GLOBAL); 
    
    // Encoder
    // Maximo 2660 uds
    //setup_qei(QEI_MODE_X2 | QEI_SWAP_AB, QEI_FILTER_DIV_64 , QEICOUNT); // Invertir
    setup_qei(QEI_MODE_X2, QEI_FILTER_DIV_64 , QEICOUNT);
    qei_set_count(0); // Inicializa a 0 el contador
    
    // PWM
    PTCON = 0x8000; // Enable
    PWMCON1 = 0x10; // PWM1 independiente
    PTPER = 0x029A; // Frequency 15Khz
    PDC1 = 0x0000; // Duty 0%   
    
}

/*
 * Lee el contador del encoder y actualiza el PWM
 * @return void
 */
void readEncoder() {
    unsigned int16 enc = (unsigned int16) qei_get_count();

    if ((enc > QEICOUNT) && (enc <= 0x7FFF)) { // Limite superior: QEICOUNT
        qei_set_count(QEICOUNT);
        enc = QEICOUNT;
    } else if ((enc <= 0xFFFF) && (enc > 0x7FFF)) { // Limite inferior: 0
        qei_set_count(0);
        enc = 0;
    }

    PDC1 = enc/2;
    delay_us(10);
}

/*
 * Controlador del BCD
 * @param int16 number   Número a representar en el display
 * @param int8  pos      Posición del número a representar
 * @param int1  dot      1 si se muestra el punto, 0 e.o.c.
 * @param int1  celsius  1 si se muestra "c", 0 e.o.c.
 * @return void
 */
void bcd(int16 number, int8 pos, int1 dot, int1 celsius) {
    // Máscara para reset
    int16 R_PORTE=0b1111111111000011;
    int16 R_PORTE_DIGIT=0b0000000100000000;
    int16 R_PORTE_POINT=0b1111111111111110;
    int16 R_PORTF=0b0000000000001100;

    // Puertos
    #word PORTE=getenv("SFR:PORTE")
    #word PORTF=getenv("SFR:PORTF")

    // Puertos de salida
    #word TRISE=getenv("SFR:TRISE")
    #word TRISF=getenv("SFR:TRISF")
    TRISE=0b0000000000000000;
    TRISF=0b0000000000000000;

    int16 position[] = {0xFFF7, 0xFFFB, 0xFEFF}; // Selector de dígito: RF3, RF2, RE8
    // Valores binarios de los números para PORTE: 0-9, c, ?, u, c barra baja, t 
    int16 numbers[] = {0x0000, 0x0020, 0x0004, 0x0024, 0x0008, 0x0028, 0x000C, 0x002C, 0x0010, 0x0030, 0x0014, 0x0034, 0x0018, 0x0038, 0x001C};
    const int8 longBCD = 3; // Número de dígitos del LCD

    int8 digit;
    int8 tmp = longBCD;
    
    digit = 0;
    while (number > 0) {
        if (tmp == pos) {
            digit = number % 10;
            number = 0;
        }
        tmp--;
        number /= 10;
    }
    

    // Reiniciamos los dígitos
    PORTF = PORTF | R_PORTF; // Reset RF2 y RF3
    PORTE = PORTE | R_PORTE_DIGIT; // Reset RE8

    switch (pos) { // Selección de posición
        case 1:
            PORTF = PORTF & position[(pos-1)];
            break;
        case 2:
            PORTF = PORTF & position[(pos-1)];
            break;
        case 3:
            PORTE = PORTE & position[(pos-1)];
            break;
        default:
            break;
    }

    // Fijamos el dígito
    if (celsius) {  
        PORTE = PORTE & R_PORTE | numbers[10];  
    } else {
        PORTE = PORTE & R_PORTE | numbers[digit];
    }


    // Activamos el punto si es necesario
    if (dot)
        PORTE = PORTE & R_PORTE_POINT; // Punto activado
    else
        PORTE = PORTE | ~R_PORTE_POINT; // Punto desactivado
}

/*
 * Actualiza el modo de funcionamiento según el tipo de pulsación
 * @param int1  duration    0 Pulsación corta
 *                          1 Pulsación larga
 * @return void
 */
void changeMode (int1 duration) {
    if (duration == 0) { // Pulsación corta
        if (modeLoad == STOP) { // Iniciar carga
            modeLoad = RUN;
            qei_set_count(countEncoder); // Establecer contador
            //setup_timer3(TMR_INTERNAL | TMR_DIV_BY_256, 19532); // Parpadeo LED @ 500ms
            setup_timer3(TMR_INTERNAL | TMR_DIV_BY_256, 39100); // Parpadeo LED @ 1s
        } else {
            modeLoad = STOP; // Parar carga
            PDC1 = 0x00;
            countEncoder = qei_get_count(); // Guardar contador
            setup_timer3(TMR_DISABLED); // Desactivar parpadeo
            blink = FALSE;
        }        
    } else { // Pulsación larga, cambiamos de modo
        mode++;
        if (mode > LASTMODE) {
            mode = 0x00;
        }
        
        // Operaciones en el modo
        switch(mode) {
            case LOAD:
               
                break;
            case VOLTAGE:

                break;
            case TEMPC:

                break; 
        }        
    }
}

/*
 * Convierte la lectura del ADC de tensión en un número entero para el LCD
 * @param int32  volt    Medida del ADC de 10 bits
 * @return int16         Entero convertido para el LCD
 */
int16 convertVoltage(int32 volt) {
    float vFloat = (float) (volt * (3.0/1023));

    // Multiplicador por el divisor de senseo de tension
    vFloat *= (VOLTDIVUP+VOLTDIVDOWN)/VOLTDIVDOWN;
    
    // Aplicamos un factor de corrección
    if (vFloat >= 0.0029)
        vFloat = vFloat*0.998452 + 0.231382; // Hasta 500mV

    if (vFloat >= 10) {
        vFloat *= 10;
        dot1 = FALSE;
        dot2 = TRUE;
        dot3 = FALSE;
    } else {
        vFloat *= 100;
        dot1 = TRUE;
        dot2 = FALSE;
        dot3 = FALSE;
    }
    return (int16) vFloat;
}

/*
 * Convierte la lectura del ADC de corriente en un número entero para el LCD
 * @param int32  curr    Medida del ADC de 10 bits
 * @return int16         Entero convertido para el LCD
 */
int16 convertCurrent(int32 curr) {
    float iFloat = (float) (curr * (3.0/1023)) * 1.25; // INA250 de 0.8V por A
    // Aplicamos un factor de corrección
    if (iFloat >= 0.003)
        iFloat = iFloat*0.992481203 + 0.0302255639;
    
    if (iFloat >= 10) {
        iFloat *= 10;
        dot1 = FALSE;
        dot2 = TRUE;
        dot3 = FALSE;
    } else {
        iFloat *= 100;
        dot1 = TRUE;
        dot2 = FALSE;
        dot3 = FALSE;
    }    
    return (int16) iFloat;
}

/*
 * Convierte la lectura del ADC de temperatura en un número entero para el LCD
 * @param int32  temp      Medida del ADC de 10 bits
 * @return int16           Entero convertido para el LCD
 */
int16 convertTemperature(int32 temp) {
    float tFloat = (float) (temp * (3.0/1023)) * 100; // 10mV por grado
    return (int16) tFloat * 10; // Desplazamos hacia la izquierda
}