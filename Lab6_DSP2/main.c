#include <F28x_Project.h>
#include <initAIC23.h>
#include <device.h>
#include <User_Drivers/RAM_DRIVER.h>

const int microSeconds = 125;
const int useTimer = 0;

const uint32_t circle_buff_max = 0xFFF;
volatile int ch_sel = 0;
volatile uint32_t circle_buff_head = 0;
volatile uint32_t circle_buff_tail = 0;
volatile int button1Press = 0;

#pragma DATA_SECTION(circle_buff, "ramgs0");

uint16_t circle_buff[circle_buff_max];


// function prototypes
void initMRINTB();
void disableMRINTB();
void playFromBuffer();

int main(void){

    InitSysCtrl();      // Set SYSCLK to 200 MHz, disables watchdog timer, turns on peripheral clocks

    DINT;               // Disable CPU interrupts on startup

    // Init PIE
    InitPieCtrl();      // Initialize PIE -> disable PIE and clear all PIE registers
    IER = 0x0000;       // Clear CPU interrupt register
    IFR = 0x0000;       // Clear CPU interrupt flag register
    InitPieVectTable(); // Initialize PIE vector table to known state

    EALLOW;             // EALLOW for rest of program (unless later function disables it)

    // initialize the RAM module
    InitSpib();
    initSequentialMode();

    InitSPIA();        // Initialize the SPI
    InitMcBSPb();      // Init McBSPb
    InitAIC23();       // Init AIC23

    initBreakoutBoardGPIO();

    initGPIOINT();

    EALLOW;
    IER |= M_INT6;
    IER |= M_INT13;


    // init the interrupt for codec


    EnableInterrupts();


    while(1);

    return 0;
}




interrupt void MRINTB_isr(void){

    uint32_t sample_L;
    uint32_t sample_R;
    uint32_t switchRead;


    switchRead = GPIO_readPortData(GPIO_PORT_A);
    switchRead = switchRead & 0xC800;


    GPIO_setPortPins(GPIO_PORT_A, 0x0F);

    if(ch_sel == 0){
        ch_sel = 1; // next recieve will be right sample
        McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_L); // recieveData
        McBSP_transmit32BitDataBlocking(MCBSPB_BASE, sample_L);

        if(!switchRead) {
            GPIO_clearPortPins(GPIO_PORT_A, 0x00000001);
            circle_buff[circle_buff_head] = (sample_L >> 16);
        }
        else{
            GPIO_clearPortPins(GPIO_PORT_A, 0x09);
            circle_buff[circle_buff_head] = circle_buff[circle_buff_head] + (sample_L >> 16);
        }

        // iterate the circle buffer head
        circle_buff_head++;
        circle_buff_head = circle_buff_head % circle_buff_max;

        // check if circle buff head is equal to circle buff tail, and move tail if so
        if(circle_buff_head == circle_buff_tail) circle_buff_tail++;
        circle_buff_tail = circle_buff_tail % circle_buff_max;

    }
    else{
        ch_sel = 0; // next recieve will be right sample
        McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_R); // recieveData
        McBSP_transmit32BitDataBlocking(MCBSPB_BASE, sample_R);
    }


    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

}


void playFromBuffer(){
    while(circle_buff_tail != circle_buff_head){
        McBSP_transmit32BitDataBlocking(MCBSPB_BASE, (circle_buff[circle_buff_tail] << 16 ));
        circle_buff_tail++;
        circle_buff_tail = circle_buff_tail % circle_buff_max;
    }
}


void initMRINTB(){
    DINT;
    EALLOW;
    PieVectTable.MCBSPB_RX_INT = &MRINTB_isr;
    PieCtrlRegs.PIEIER6.bit.INTx7 = 1;
    IER |= M_INT6;
    //EnableInterrupts();
}

void disableMRINTB(){
    EALLOW;
    DINT; // disable interrupts globally
    PieCtrlRegs.PIEIER6.bit.INTx7 = 0; // diable interrupt in PIEIER
    for(int i; i < 5; i++); // 5 ish cycle delay
    IFR &= ~M_INT6; // disable interrupt on cpu
    PieCtrlRegs.PIEACK.all &= ~PIEACK_GROUP6; // disable interrupt on PIE
    //EnableInterrupts();
}

void initBreakoutBoardGPIO(){
    // the push buttons
    GPIO_setPinConfig(GPIO_10_GPIO10);
    GPIO_setPinConfig(GPIO_9_GPIO9);
    GPIO_setPinConfig(GPIO_8_GPIO8);
    // the switches
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setPinConfig(GPIO_15_GPIO15);
    GPIO_setPinConfig(GPIO_14_GPIO14);
    GPIO_setPinConfig(GPIO_11_GPIO11);
    // the leds

    GPIO_setPinConfig(GPIO_7_GPIO7);
    GPIO_setPinConfig(GPIO_6_GPIO6);
    GPIO_setPinConfig(GPIO_5_GPIO5);
    GPIO_setPinConfig(GPIO_4_GPIO4);
    GPIO_setPinConfig(GPIO_3_GPIO3);
    GPIO_setPinConfig(GPIO_2_GPIO2);
    GPIO_setPinConfig(GPIO_1_GPIO1);
    GPIO_setPinConfig(GPIO_0_GPIO0);

    // configure the pull ups for the push buttons and switches
    GPIO_setPadConfig(10, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_PULLUP);

    GPIO_setPadConfig(16, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(15, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(14, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_PULLUP);

    // configure the input qualifications
    // should help with bouncing!
    GPIO_setQualificationMode(10, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(9, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(8, GPIO_QUAL_6SAMPLE);

    GPIO_setQualificationPeriod(10, 510); // large divider.

    // configure directions

    GPIO_setDirectionMode(10, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_IN);


    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(15, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(14, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(11, GPIO_DIR_MODE_IN);

    GPIO_setDirectionMode(7, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(3, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);
}




interrupt void Button3_isr(void){
    if(!button1Press){
        button1Press = 1;
        if(useTimer){
            InitTimer1(microSeconds);
        }
        else{
        initMRINTB();
        }
    }
    else{
        button1Press = 0;
        if(useTimer){
              disableTimer1();
          }
          else{
              disableMRINTB();
          }
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    //EnableInterrupts();
}

interrupt void Button1_isr(void){
    playFromBuffer();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void initGPIOINT(){
    EALLOW;
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptPin(8,GPIO_INT_XINT1);
    GPIO_setInterruptPin(10,GPIO_INT_XINT2);

    EALLOW;
    PieVectTable.XINT1_INT = &Button1_isr;
    PieVectTable.XINT2_INT = &Button3_isr;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;
    IER |= M_INT1;

    GPIO_enableInterrupt(GPIO_INT_XINT1);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
}

void disableTimer1(){
    EALLOW;
    DINT; // disable interrupts globally
    for(int i; i < 5; i++); // 5 ish cycle delay
    IFR &= ~M_INT13; // disable interrupt on cpu
}


interrupt void Timer1_isr(void) {
       uint32_t sample_L;
       uint32_t sample_R;

       GPIO_togglePin(0);
       if(ch_sel == 0){
           ch_sel = 1; // next recieve will be right sample
           McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_L); // recieveData
           SRAM_16bit_write(circle_buff_head, ((uint16_t)(sample_L >>16)));

           // iterate the circle buffer head
           circle_buff_head++;
           circle_buff_head = circle_buff_head % circle_buff_max;

           // check if circle buff head is equal to circle buff tail, and move tail if so
           if(circle_buff_head == circle_buff_tail) circle_buff_tail++;
           circle_buff_tail = circle_buff_tail % circle_buff_max;

       }
       else{
           ch_sel = 0; // next recieve will be right sample
           McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_R); // recieveData
       }


}


void InitTimer1(int microSeconds){
    InitCpuTimers();                            // Initialize all timers to known state
    ConfigCpuTimer(&CpuTimer1, 200, microSeconds);    // Configure CPU timer 1. 200 -> SYSCLK in MHz, 500000 -> period in usec. NOTE: Does NOT start timer
    EALLOW;
    PieVectTable.TIMER1_INT = &Timer1_isr;      // Assign timer 1 ISR to PIE vector table
    IER |= M_INT13;                             // Enable INT13 in CPU                         // Enable PIE and CPU interrupts
    CpuTimer1.RegsAddr->TCR.bit.TSS = 0;        // Start timer 1
}


void Decimation(){
    for(uint32_t i  = 0; i < 0x1FFF; i++){
        if(i < 0x1FFF/3){
        uint16_t temp = SRAM_16bit_read(i*3);
        SRAM_16bit_write(i, temp);
    }
        else{
            SRAM_16bit_write(i, 0x00);
        }
}
}

void interpolation(){
    for(uint32_t i  = 0; i < (0x1FFF/4); i++){
        uint16_t temp = SRAM_16bit_read(i*4);
        uint16_t temp2 = SRAM_16bit_read((i*4)+1);
        for(int k = 0; k < 4; k++){
            SRAM_16bit_write((i*4) + k, (temp2-temp) * (-1)  + temp2);
        }

    }
}




