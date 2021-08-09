#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/rom_map.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"

#define NUM_I2C_DATA 2
#define SLAVE_ADDRESS 0x3C
uint8_t g_master_buff[NUM_I2C_DATA];
unsigned char LCD_screen_buffer[1024];
char LCD_line1[18];
char LCD_line2[18];
char LCD_line3[18];
char LCD_line4[18];
int sensor;

void InitI2C0()
{
    // The I2C0 peripheral must be enabled before use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    // For this example I2C0 is used with PortB[3:2].
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);
    //clear I2C FIFOs
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;

    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);
}

void LCD_send_Command(unsigned char* commands,unsigned int command_length )
{

    int x=0;
    for (x=0; x<command_length; x++)
    {
        I2CMasterDataPut(I2C0_BASE, 0x00);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        // Wait until MCU is done transferring.
        //while(I2CMasterBusy(I2C0_BASE));
        //I2CMasterDataPut(I2C0_BASE, commands[x]) ;
        //I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
        while(I2CMasterBusy(I2C0_BASE));
        I2CMasterDataPut(I2C0_BASE, commands[x]) ;
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(I2C0_BASE));
    }
}

void LCD_send_Data(unsigned char* data_, unsigned int data_length)
{
    int x=0;
    for (x=0; x<data_length; x++)
    {
        I2CMasterDataPut(I2C0_BASE, 0x40);
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C0_BASE));
        I2CMasterDataPut(I2C0_BASE, data_[x]) ;
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy(I2C0_BASE));
    }
}

void LCD_init()
{
    unsigned char init_sequence[27]={0xAE,0xD5,0x80,0xA8,63,0xD3,0x00,0x40,0x8D,
            0xD5,0x14,0x20,0x00,0xA1,0xC8,0xDA,0x12,0x81,0xCF,0xD9,0xF1,  0xDB,0x40,
            0xA4,0xA6,0x2E,0xAF};
    LCD_send_Command(init_sequence,27);
}

void LCD_print()
{
    unsigned char start_screen[6]={0x22,0x00,0xFF,0x21,0x00,127};
    LCD_send_Command(start_screen,6);
    LCD_send_Data(LCD_screen_buffer,1024);
}

void LCD_parse (char* letra, unsigned char caracter)
{
    switch (caracter){
            case 'a': case 'A':
                letra[0] = 0x00;
                letra[1] = 0xFC;
                letra[2] = 0x12;
                letra[3] = 0x11;
                letra[4] = 0x12;
                letra[5] = 0xFC;
                letra[6] = 0x00;
                break;

            case 'b': case 'B':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x89;
                letra[3] = 0x89;
                letra[4] = 0x8E;
                letra[5] = 0x70;
                letra[6] = 0x00;
                break;

            case 'c': case 'C':
                letra[0] = 0x00;
                letra[1] = 0x7E;
                letra[2] = 0x81;
                letra[3] = 0x81;
                letra[4] = 0x81;
                letra[5] = 0x42;
                letra[6] = 0x00;
                break;

            case 'd': case 'D':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x81;
                letra[3] = 0x81;
                letra[4] = 0x42;
                letra[5] = 0x3C;
                letra[6] = 0x00;
                break;

            case 'e': case 'E':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x89;
                letra[3] = 0x89;
                letra[4] = 0x89;
                letra[5] = 0x81;
                letra[6] = 0x00;
                break;

            case 'f': case 'F':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x09;
                letra[3] = 0x09;
                letra[4] = 0x09;
                letra[5] = 0x01;
                letra[6] = 0x00;
                break;

            case 'g': case 'G':
                letra[0] = 0x00;
                letra[1] = 0x7E;
                letra[2] = 0x81;
                letra[3] = 0x91;
                letra[4] = 0x91;
                letra[5] = 0x70;
                letra[6] = 0x00;
                break;

            case 'h': case 'H':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x08;
                letra[3] = 0x08;
                letra[4] = 0x08;
                letra[5] = 0xFF;
                letra[6] = 0x00;
                break;

            case 'i': case 'I':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x81;
                letra[3] = 0xFF;
                letra[4] = 0x81;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case 'j': case 'J':
                letra[0] = 0x00;
                letra[1] = 0x40;
                letra[2] = 0x80;
                letra[3] = 0xFF;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case 'k': case 'K':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x18;
                letra[3] = 0x24;
                letra[4] = 0x42;
                letra[5] = 0x81;
                letra[6] = 0x00;
                break;

            case 'l': case 'L':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0xFF;
                letra[3] = 0x80;
                letra[4] = 0x80;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case 'm': case 'M':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x06;
                letra[3] = 0x0C;
                letra[4] = 0x06;
                letra[5] = 0xFF;
                letra[6] = 0x00;
                break;

            case 'n': case 'N':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x06;
                letra[3] = 0x08;
                letra[4] = 0x30;
                letra[5] = 0xFF;
                letra[6] = 0x00;
                break;

            case 'o': case 'O':
                letra[0] = 0x00;
                letra[1] = 0x7E;
                letra[2] = 0x81;
                letra[3] = 0x81;
                letra[4] = 0x81;
                letra[5] = 0x7E;
                letra[6] = 0x00;
                break;

            case 'p': case 'P':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x09;
                letra[3] = 0x09;
                letra[4] = 0x09;
                letra[5] = 0x06;
                letra[6] = 0x00;
                break;

            case 'q': case 'Q':
                letra[0] = 0x00;
                letra[1] = 0x7E;
                letra[2] = 0x85;
                letra[3] = 0x89;
                letra[4] = 0xB1;
                letra[5] = 0x7E;
                letra[6] = 0x40;
                break;

            case 'r': case 'R':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x19;
                letra[3] = 0x29;
                letra[4] = 0x49;
                letra[5] = 0x86;
                letra[6] = 0x00;
                break;

            case 's': case 'S':
                letra[0] = 0x00;
                letra[1] = 0x4E;
                letra[2] = 0x89;
                letra[3] = 0x89;
                letra[4] = 0x89;
                letra[5] = 0x72;
                letra[6] = 0x00;
                break;

            case 't': case 'T':
                letra[0] = 0x00;
                letra[1] = 0x01;
                letra[2] = 0x01;
                letra[3] = 0xFF;
                letra[4] = 0x01;
                letra[5] = 0x01;
                letra[6] = 0x00;
                break;

            case 'u': case 'U':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x80;
                letra[3] = 0x80;
                letra[4] = 0x80;
                letra[5] = 0xFF;
                letra[6] = 0x00;
                break;

            case 'v': case 'V':
                letra[0] = 0x00;
                letra[1] = 0x3F;
                letra[2] = 0x40;
                letra[3] = 0x80;
                letra[4] = 0x40;
                letra[5] = 0x3F;
                letra[6] = 0x00;
                break;

            case 'w': case 'W':
                letra[0] = 0x00;
                letra[1] = 0xFF;
                letra[2] = 0x40;
                letra[3] = 0x20;
                letra[4] = 0x40;
                letra[5] = 0xFF;
                letra[6] = 0x00;
                break;

            case 'x': case 'X':
                letra[0] = 0x81;
                letra[1] = 0x42;
                letra[2] = 0x24;
                letra[3] = 0x18;
                letra[4] = 0x24;
                letra[5] = 0x42;
                letra[6] = 0x81;
                break;

            case 'y': case 'Y':
                letra[0] = 0x00;
                letra[1] = 0x03;
                letra[2] = 0x04;
                letra[3] = 0xFC;
                letra[4] = 0x04;
                letra[5] = 0x03;
                letra[6] = 0x00;
                break;

            case 'z':  case 'Z':
                letra[0] = 0x00;
                letra[1] = 0xE1;
                letra[2] = 0x91;
                letra[3] = 0x89;
                letra[4] = 0x85;
                letra[5] = 0x83;
                letra[6] = 0x00;
                break;

            case '-':  //ñ
                letra[0] = 0x00;
                letra[1] = 0xFC;
                letra[2] = 0x09;
                letra[3] = 0x11;
                letra[4] = 0x21;
                letra[5] = 0xFC;
                letra[6] = 0x00;
                break;

            case '.':
                letra[0] = 0xC0;
                letra[1] = 0xC0;
                letra[2] = 0x00;
                letra[3] = 0x00;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case ':':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x00;
                letra[3] = 0x66;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '=':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x28;
                letra[3] = 0x28;
                letra[4] = 0x28;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '1':
                letra[0] = 0x00;
                letra[1] = 0x04;
                letra[2] = 0x02;
                letra[3] = 0xFF;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '2':
                letra[0] = 0x00;
                letra[1] = 0xE2;
                letra[2] = 0x91;
                letra[3] = 0x89;
                letra[4] = 0x86;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '3':
                letra[0] = 0x00;
                letra[1] = 0x42;
                letra[2] = 0x81;
                letra[3] = 0x99;
                letra[4] = 0x66;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '4':
                letra[0] = 0x00;
                letra[1] = 0x18;
                letra[2] = 0x14;
                letra[3] = 0x12;
                letra[4] = 0xFF;
                letra[5] = 0x10;
                letra[6] = 0x00;
                break;

            case '5':
                letra[0] = 0x00;
                letra[1] = 0x4F;
                letra[2] = 0x89;
                letra[3] = 0x89;
                letra[4] = 0x89;
                letra[5] = 0x70;
                letra[6] = 0x00;
                break;

            case '6':
                letra[0] = 0x00;
                letra[1] = 0x7E;
                letra[2] = 0x89;
                letra[3] = 0x89;
                letra[4] = 0x89;
                letra[5] = 0x70;
                letra[6] = 0x00;
                break;

            case '7':
                letra[0] = 0x00;
                letra[1] = 0x01;
                letra[2] = 0x09;
                letra[3] = 0x09;
                letra[4] = 0xFF;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '8':
                letra[0] = 0x00;
                letra[1] = 0x66;
                letra[2] = 0x99;
                letra[3] = 0x99;
                letra[4] = 0x99;
                letra[5] = 0x66;
                letra[6] = 0x00;
                break;

            case '9':
                letra[0] = 0x00;
                letra[1] = 0x06;
                letra[2] = 0x09;
                letra[3] = 0x09;
                letra[4] = 0xFE;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case '0':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x7E;
                letra[3] = 0x81;
                letra[4] = 0x7E;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            case ' ':
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x00;
                letra[3] = 0x00;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;

            default:
                letra[0] = 0x00;
                letra[1] = 0x00;
                letra[2] = 0x00;
                letra[3] = 0x00;
                letra[4] = 0x00;
                letra[5] = 0x00;
                letra[6] = 0x00;
                break;
    }

}

void LCD_print_text()
{
    int y,z;

    char buffer_letra[8];
    for(y=0; y<18 ;y++)
    {
        LCD_parse(buffer_letra, LCD_line1[y]);
        for(z=0; z<7 ;z++)
        {
            LCD_screen_buffer[129+ (7*y) +z] = buffer_letra[0+z];
        }
    }
    for(y=0; y<18 ;y++)
    {
        LCD_parse(buffer_letra, LCD_line2[y]);
        for(z=0; z<7 ;z++)
        {
            LCD_screen_buffer[385+ (7*y) +z] = buffer_letra[0+z];
        }
    }
    for(y=0; y<18 ;y++)
        {
            LCD_parse(buffer_letra, LCD_line3[y]);
            for(z=0; z<7 ;z++)
            {
                LCD_screen_buffer[641+ (7*y) +z] = buffer_letra[0+z];
            }
        }
        for(y=0; y<18 ;y++)
        {
            LCD_parse(buffer_letra, LCD_line4[y]);
            for(z=0; z<7 ;z++)
            {
                LCD_screen_buffer[897+ (7*y) +z] = buffer_letra[0+z];
            }
        }
    LCD_print();
}

void LCD_clear()
{
    memset(&LCD_screen_buffer, 0, 1024);
    LCD_print();
}
void LCD_set()
{
    memset(&LCD_screen_buffer, 0xFF, 1024);
    LCD_print();
}

//TIMER
static volatile uint32_t Counter = 0;
void Timer0BIntHandler(void)
{
    Counter++;
    TimeIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
}
//INTERRUPCION
void GPIOF_Handler_mifuncion()
{
    uint32_t banderas;
    banderas = GPIOIntStatus(GPIO_PORTF_BASE, true);
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
    banderas = GPIOIntStatus(GPIO_PORTE_BASE, true);
    sensor++;
}

void main(void)
{
    uint32_t valorADC;
  //  int PWMPeriod1 = 0;
    int Vel = 0;
    int vueltas = 0;
    int RPMA = 0;
    int RPMD = 350;
    int Start = 0;
    int INT_TIMER0B;

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
// I2C
    InitI2C0();
    LCD_init();
    LCD_clear();
//ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0  | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
//GPIO BOTON INICIO
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4); //E4
//PWM
    // Set the PWM clock to the system clock.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    // The PWM peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    // Enable the GPIO port that is used for the PWM output.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Configure the GPIO pad for PWM function on pins PF1
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    // Configure PWM1 to count down without synchronization.
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    // Set the PWM period to 250Hz.  To calculate the appropriate parameter
    // use the following equation: N = (1 / f) * SysClk.  Where N is the
    // function parameter, f is the desired frequency, and SysClk is the
    // system clock frequency.
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 1000);
    PWMPeriod1 = PWMGenPeriodGet(PWM1_BASE, PWM_GEN_2);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 1);
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
    // Enable the PWM generator block.
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
//INTERRUPCION
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_4);  //F3 ALTO F4 VALOR
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_INT_PIN_3, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_INT_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);
    GPIOIntRegister(GPIO_PORTF_BASE, GPIOF_Handler_mifuncion);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
//TIMER
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_B, SysCtlClockGet()/1000);
    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    IntEnable(INT_TIMER0B);
    TimerEnable(TIMER0_BASE, TIMER_B);

    while(1)
        {
        //LEER ADC
        ADCProcessorTrigger(ADC0_BASE, 2);
        while(!ADCIntStatus(ADC0_BASE, 2, false));
        ADCIntClear(ADC0_BASE, 2);
        ADCSequenceDataGet(ADC0_BASE, 2, &valorADC);
        SysCtlDelay(SysCtlClockGet()/12);

        Vel = (valorADC*13000)/4095;
        if(Vel<100)
        {
         Vel=50;
        }

        //I2C
        LCD_clear();
        sprintf(LCD_line1,"     Velocidad    ");
        sprintf(LCD_line2,"         %d       ", Vel);
        sprintf(LCD_line3,"                  ");
        LCD_print_text();
        SysCtlDelay(SysCtlClockGet()/5);

        Start = GPIOPinRead(GPIO_PORTE_BASE, GPIO_INT_PIN_4);

        if(Start==0x10)
        {
            IntMasterEnable();
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)==00)
            {
                PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, RPMD);

                Counter=0;
                while(Counter<=100)
                {
                    if(sensor==20)
                    {
                        vueltas++;
                        sensor=0;
                    }
                    else if(sensor>20)
                    {
                        sensor=0;
                    }
                }
                RPMA = vueltas*60;
                vueltas=0;
                if(RPMA==Vel)
                {
                    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, RPMD);
                }
                else if(RPMA<Vel)
                {
                    RPMD=RPMD+10;
                }
                else if(RPMA>Vel)
                {
                    RPMD=RPMD-10;
                }
                LCD_clear();
                sprintf(LCD_line1," RPM deseadas:  %d  ",Vel);
                sprintf(LCD_line2,"RPM actuales:  %d   ",RPMA);
                sprintf(LCD_line3,"Valor PWM:   %d     ",PWMPulseWidthGet(PWM1_BASE, PWM_OUT_5));
                LCD_print_text();
                SysCtlDelay(SysCtlClockGet()/5);
            }
            PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
            IntMasterDisable();
        }
        else
        {}
        }
