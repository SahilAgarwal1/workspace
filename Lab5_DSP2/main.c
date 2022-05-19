//#include <device.h>
//#include <driverlib.h>
//#include <F2837xD_device.h>
#include <F28x_Project.h>
#include <LCD_DRIVER.h>
#include <string.h>
interrupt void Timer1_isr(void);

void InitTimer1(void);
void InitAdca(void);

Uint16 test = 0;
Uint16 AdcData = 0;

int main(void)
{

    InitSysCtrl();      // Set SYSCLK to 200 MHz, disables watchdog timer, turns on peripheral clocks

    DINT;               // Disable CPU interrupts on startup

    // Init PIE
    InitPieCtrl();      // Initialize PIE -> disable PIE and clear all PIE registers
    IER = 0x0000;       // Clear CPU interrupt register
    IFR = 0x0000;       // Clear CPU interrupt flag register
    InitPieVectTable(); // Initialize PIE vector table to known state

    EALLOW;             // EALLOW for rest of program (unless later function disables it)

    //init_LCD();
    //write_string_LCD("Voltage: 0.00", 13);

    InitTimer1();       // Initialize CPU timer 1

    InitAdca();         // Initialize ADC A channel 0

    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;     // Blue LED for timer

    while(1);   // Stall
}

void InitAdca(void) {
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;                                 // Set ADCCLK to SYSCLK/4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE); // Initializes ADCA to 12-bit and single-ended mode. Performs internal calibration
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;                                 // Powers up ADC
    DELAY_US(1000);                                                    // Delay to allow ADC to power up
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;                                 // Sets SOC0 to channel 0 -> pin ADCINA0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 14;                                // Sets sample and hold window -> must be at least 1 ADC clock long
}

void InitTimer1(void) {
    InitCpuTimers();                            // Initialize all timers to known state
    ConfigCpuTimer(&CpuTimer1, 200, 500000);    // Configure CPU timer 1. 200 -> SYSCLK in MHz, 500000 -> period in usec. NOTE: Does NOT start timer
    PieVectTable.TIMER1_INT = &Timer1_isr;      // Assign timer 1 ISR to PIE vector table
    IER |= M_INT13;                             // Enable INT13 in CPU
    EnableInterrupts();                         // Enable PIE and CPU interrupts
    CpuTimer1.RegsAddr->TCR.bit.TSS = 0;        // Start timer 1
}

interrupt void Timer1_isr(void) {
    test++;                                 // Increment variable. Can view in real time in the "Expressions" window if breakpoint here set to refresh all windows (see breakpoint properties)
    AdcaRegs.ADCSOCFRC1.all = 0x1;          // Force conversion on channel 0
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;  // Toggle blue LED
    AdcData = AdcaResultRegs.ADCRESULT0;    // Read ADC result into global variable

}


/*
 * #include <F28x_Project.h>
#include <initAIC23.h>
#include <device.h>

volatile int ch_sel = 0;

int main(void){

    InitSysCtrl();      // Set SYSCLK to 200 MHz, disables watchdog timer, turns on peripheral clocks

    DINT;               // Disable CPU interrupts on startup

    // Init PIE
    InitPieCtrl();      // Initialize PIE -> disable PIE and clear all PIE registers
    IER = 0x0000;       // Clear CPU interrupt register
    IFR = 0x0000;       // Clear CPU interrupt flag register
    InitPieVectTable(); // Initialize PIE vector table to known state

    EALLOW;             // EALLOW for rest of program (unless later function disables it)



    InitSPIA();        // Initialize the SPI
    InitMcBSPb();      // Init McBSPb
    InitAIC23();       // Init AIC23

    initMRINTB();


    while(1);




    return 0;
}


interrupt void MRINTB_isr(void){
    uint32_t sample_L;
    uint32_t sample_R;

    if(ch_sel == 0){
        ch_sel = 1; // next recieve will be right sample
        McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_L); // recieveData
        McBSP_transmit32BitDataBlocking(MCBSPB_BASE, sample_L); // writeData
    }
    else{
        ch_sel = 0; // next recieve will be right sample
        McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_R); // recieveData
        McBSP_transmit32BitDataBlocking(MCBSPB_BASE, sample_R); // writeData
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

}


void initMRINTB(){
    EALLOW;
    PieVectTable.MCBSPB_RX_INT = &MRINTB_isr;
    PieCtrlRegs.PIEIER6.bit.INTx7 = 1;
    IER |= M_INT6;
    EnableInterrupts();
}
 */
