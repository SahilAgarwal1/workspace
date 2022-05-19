

/**
 * main.c
 */

#include <F2837xD_device.h>
#include <sysctl.h>
#include <LCD_DRIVER.h>
#include <OneToOneI2CDriver.h>
#include <RAM_DRIVER.h>
#include <string.h>



int main(void)
{

    InitSysCtrl();

   InitSpib();
   //init_LCD();
   initSequentialMode();

   GPIO_WritePin(66, 0);
   uint16_t check = Spi_Transmit((0x05 << 8));
   GPIO_WritePin(66, 1);

   SRAM_16bit_write(0x00, 0x1011);
   uint16_t outdata = SRAM_16bit_read(0x00);



   while(1);

	return 0;
}


void demo0(){
    init_LCD();


    Uint16 name_me[13] = {'S','A','H','I','L',' ', 'A', 'G', 'A', 'R', 'W', 'A', 'L'};
    Uint16 classNum[7] = {'E', 'E', 'L', '4', '5', '1', '1'};


   write_string_LCD(name_me, 13 );
   write_char_LCD(0b11000000, 0); // // move to second line
   write_string_LCD(classNum, 7);

   while(1);
}

void test1(){
    for(uint32_t i = 0; i < 0x3FFFF; i++){
        SpibWriteData_dualSram(i, ('$' << 8 ) | '$');
    }

    for(int k = 0x0003FFFE; k > 0; k--){
        uint16_t temp = SpibReadData_dualSram(k);
        if(temp != (('$' << 8) | '$')){
            int errorAddress = k;

            write_string_LCD("I'm Broke", 9);
            while(1); // error loop

        }
    }

    write_string_LCD("Check Cashed", 12);
    while(1); // No error loop
}

void test2(){
    for(uint32_t i = 0; i < 0x7FFF; i++){
        SpibWriteData_dualSram(i, 0x02 * i);
    }

    for(uint32_t k = 0x00007FFE; k > 0; k--){
        uint16_t temp = SpibReadData_dualSram(k);
        if(temp != (k* 0x02)){
            int errorAddress = k;
            write_string_LCD("Inc test Bad",12);
            while(1); // error loop

        }
    }
    write_string_LCD("Inc Test Good",13);
    while(1); // No error loop

}

void quiz(){
    uint16_t sentence[9] = {'C','A','T','M','O','P','D','O','G'};

    for(uint32_t i = 0; i < 0x3FFFF; i++){
        SpibWriteData_dualSram(i, sentence[i%9]);
    }

    char readchar[3];

    for(uint32_t i = 0x0003FFFE; i > 0; i--){
        readchar[i%3] = SpibReadData_dualSram(i);

        if((i%3) == 0){
            clearScreen();
            write_string_LCD(readchar, 3);
        }
    }
}


