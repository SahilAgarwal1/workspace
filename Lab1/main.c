#include "msp.h"
#include "driverlib.h"
#include <RGBLEDS.h>


/**
 * main.c
 */


// UART config

static const eUSCI_UART_Config Uart115200Config =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK, // SMCLK Clock Source
    6, // BRDIV
    8, // UCxBRF
    0, // UCxBRS
    EUSCI_A_UART_NO_PARITY, // No Parity
    EUSCI_A_UART_LSB_FIRST, // LSB First
    EUSCI_A_UART_ONE_STOP_BIT, // One stop bit
    EUSCI_A_UART_MODE, // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
};

void uartInit()
{
    /* select GPIO functionality */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /* configure baud rate */
    MAP_UART_initModule(EUSCI_A0_BASE, &Uart115200Config);

    /* enable UART */
    MAP_UART_enableModule(EUSCI_A0_BASE);
}

static inline void uartTransmitString(char* s)
{
    /* Loop while not null */
    while(*s)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, *s++);
    }
}

uint16_t fletcher16inC(uint8_t *data, int count)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;


    for (index = 0; index < count; ++index)
    {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}

int Modulus255(int input){
    return input % 255;
}

extern uint16_t fletcher16(uint8_t *data, int count);



void partAandB(){
    uartInit(); // init UART


    // variable definitions
    char str[255];
    uint8_t data[4][4] = {{1,2,3,4},
                          {5,6,7,8},
                          {8,9,101,11},
                          {12,13,13,12}};
    int count = 16;
    uint16_t checksum = fletcher16(data,count);
    uint16_t checksumC = fletcher16inC(data,count);

    // output via usart

    snprintf(str,255,"Hello world, checksum is %d", checksum);

    uartTransmitString(str);

    snprintf(str,255," and the checksum calculated with C code is %d", checksumC);

    uartTransmitString(str);

    init_RGBLEDS();

    LP3943_LedModeSet(1, checksum);

    while(1);


}

void partC(){
    init_RGBLEDS();
    init_PortISR();
    SysTick_Config(CS_getMCLK()/4); // 250ms time for interrupt
    SysTick_enableInterrupt();

    while(1){
        PCM_gotoLPM0();
}
}

volatile bool buttonPress = false;
volatile uint16_t pattern = 0b0101010101010101;



void SysTick_Handler(){
    pattern = ~pattern;
    if(buttonPress){
        LP3943_LedModeSet(RED, pattern);
        LP3943_LedModeSet(GREEN, 0);
    }
    else{
        LP3943_LedModeSet(GREEN, pattern);
        LP3943_LedModeSet(RED, 0);
    }

}

void init_PortISR(){
    P4->DIR &= ~BIT4;//set direction
    P4->IFG &= ~BIT4; // clear flag
    P4->IE |= BIT4; // enable interrupt
    P4->IES |= BIT4; // high to low transition
    P4->REN |= BIT4; // PULL up resistor
    P4->OUT |= BIT4; // set res to pull up

    NVIC_EnableIRQ(PORT4_IRQn);

}

void PORT4_IRQHandler(void){
    P4->IFG &= ~BIT4; // clear the flag

    buttonPress = !buttonPress;
}


// quiz stuff

typedef struct dateinfo{
    uint8_t hour;
    uint8_t minute;
    uint8_t day;
    uint8_t month;
} dateinfo;

extern void quiz1(dateinfo* input, dateinfo* output);


void quizdemo(){
    dateinfo test = {
                     .hour = 15,
                     .minute = 59,
                     .day = 26,
                     .month = 1

    };

    dateinfo result;

    quiz1(&test, &result);

    while(1);


}

void main(void)
{
  partC();

}
