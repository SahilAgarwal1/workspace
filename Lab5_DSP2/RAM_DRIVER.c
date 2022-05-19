/*
 * RAM_DRIVER.c
 *
 *  Created on: Oct 2, 2021
 *      Author: sahil
 *
 */

#include <RAM_DRIVER.h>



#define SPI_BRR       0x1 // for baud rate of 40 MHZ


void InitSpib(void)
{

    /*
    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpibRegs.SPICCR.bit.SPISWRESET = 0;
    SpibRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpibRegs.SPICCR.bit.SPICHAR = (16-1);
    SpibRegs.SPICCR.bit.SPILBK = 1;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpibRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpibRegs.SPICTL.bit.TALK = 1;
    SpibRegs.SPICTL.bit.CLK_PHASE = 0;
    SpibRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpibRegs.SPIBRR.bit.SPI_BIT_RATE = SPI_BRR;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpibRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpibRegs.SPICCR.bit.SPISWRESET = 1;
    */

    SPI_disableModule(SPIB_BASE); // disable module to start editing it
    InitGpioSpib();
    SysCtl_setLowSpeedClock(SYSCTL_LSPCLK_PRESCALE_1);
    SPI_setConfig(SPIB_BASE, DEVICE_SYSCLK_FREQ ,SPI_PROT_POL0PHA0, SPI_MODE_MASTER, SPI_BRR, 16);
    SpibRegs.SPIPRI.bit.FREE = 1; // set emulation free for debugging
    SPI_enableModule(SPIB_BASE);



}

void InitGpioSpib(void){
    // Enable internal pull-up for the selected pins
    GPIO_setPadConfig(63, GPIO_PIN_TYPE_PULLUP); // SPISIMOB
    GPIO_setPadConfig(64, GPIO_PIN_TYPE_PULLUP); // SPISOMIB
    GPIO_setPadConfig(65, GPIO_PIN_TYPE_PULLUP); // SCLKB
    //GPIO_setPadConfig(66, GPIO_PIN_TYPE_PULLUP); // SPI CS0
    //GPIO_setPadConfig(67, GPIO_PIN_TYPE_PULLUP); // SPI CS1

    // set up async qual for all pins
    GPIO_setQualificationMode(63, GPIO_QUAL_ASYNC); // SPISIMOB
    GPIO_setQualificationMode(64, GPIO_QUAL_ASYNC); // SPISOMIB
    GPIO_setQualificationMode(65, GPIO_QUAL_ASYNC); // SPICLKB
    //GPIO_setQualificationMode(66, GPIO_QUAL_ASYNC); // SPI CS0
    //GPIO_setQualificationMode(67, GPIO_QUAL_ASYNC); // SPI CS1

    // set up as spi pins in mux
    GPIO_setPinConfig(GPIO_63_SPISIMOB);
    GPIO_setPinConfig(GPIO_64_SPISOMIB);
    GPIO_setPinConfig(GPIO_65_SPICLKB);

    //set up CS0 and CS1
    GPIO_writePin(66, 1); // set pin as 1 so that chip select is disabled
    GPIO_writePin(67, 1);
    GPIO_setDirectionMode(66, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(67, GPIO_DIR_MODE_OUT);
}


uint16_t Spi_Transmit(uint16_t data){
    //write data to SPI register
    SPI_writeDataBlockingNonFIFO(SPIB_BASE, data);
    // wait till the block has been sent out
    while(!SpibRegs.SPISTS.bit.INT_FLAG);
    // read and recieved data
    uint16_t RxData = SPI_readDataBlockingNonFIFO(SPIB_BASE);
    return RxData;

}
void SpibWriteData_SRAM_8bit(uint32_t address, uint16_t data, uint8_t CSpinNum){
    GPIO_WritePin(CSpinNum, 0); // set chip select low
    Spi_Transmit((0x02 << 8) | ((address  & 0x00FF0000) >> 16));
    Spi_Transmit(address & 0x0000FFFF);
    Spi_Transmit(data << 8);
    GPIO_WritePin(CSpinNum, 1); // set chip select high

}


uint16_t SpibReadData_SRAM_8bit(uint32_t address, uint8_t CSpinNum){

    GPIO_WritePin(CSpinNum, 0); // bring cs low
    Spi_Transmit((0x03 << 8) | ((address  & 0x00FF0000) >> 16));
    Spi_Transmit(address & 0x0000FFFF);
    uint16_t readData = Spi_Transmit(0x0000);
    GPIO_WritePin(CSpinNum, 1); // set chip select high
    return readData & 0x00FF;
}

void SpibWriteData_SRAM_16bit(uint32_t address, uint16_t data, uint8_t CSpinNum){
    SpibWriteData_SRAM_8bit(address, data & 0x00FF, CSpinNum);
    SpibWriteData_SRAM_8bit(address + 1, (data & 0xFF00) >> 8, CSpinNum);
}

uint16_t SpibReadData_SRAM_16bit(uint32_t address, uint8_t CSpinNum){
    uint16_t temp = SpibReadData_SRAM_8bit(address, CSpinNum);
    uint16_t temp1 = SpibReadData_SRAM_8bit(address + 1, CSpinNum);
    return ((temp1 << 8) | temp);
}

void SpibWriteData_dualSram(uint32_t address, uint16_t data){
    uint16_t CSpinNum;
    if(address & (0x01 << 17)){
        CSpinNum = 67;
                            // check if SRAM1 or SRAM2
    }
    else{
        CSpinNum = 66;
    }

    SpibWriteData_SRAM_16bit((address & 0x0001FFFF) * 2, data, CSpinNum); // send the write address
}

uint16_t SpibReadData_dualSram(uint32_t address){
    uint16_t CSpinNum;
    if(address & (0x01 << 17)){
        CSpinNum = 67;
                            // check if SRAM1 or SRAM2
    }
    else{
        CSpinNum = 66;
    }

    return SpibReadData_SRAM_16bit((address & 0x0001FFFF) * 2, CSpinNum);

}




