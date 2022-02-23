/**
  ******************************************************************************
  * @file    Dosieranlage.c 
  * @author  Daniel Marek, Marvin Perzi 
  * @version V01
  * @date    19-01-2022
  * @brief   Simulation of a dosing plant for a water tank
  ******************************************************************************
  * @history 19-01-2022: Perzi/Marek creation
  ******************************************************************************
  */

#include <stm32f10x.h>
#include <stdio.h>

/*---------------------------Prototypes ---------------------------------------*/
void uart1_init(void);
void uart_put_char(char ch);
void uart_put_string(char *string);

void init_OnewirePorts(void);
void wait_ms(int ms);
void wait_10us(int mal);

/*----------------------------- Define Pins for Onewire--------------*/
#define GPIOA_IDR GPIOA_BASE + 2*sizeof(uint32_t)    // Calc peripheral address GPIOA IDR
#define GPIOA_ODR GPIOA_BASE + 3*sizeof(uint32_t)   // Calc peripheral address GPIOA ODR

// Calc Bit Band Adress from peripheral address: a = peripheral address b = Bit number
#define BITBAND_PERI(a, b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))

#define Onewire_Out  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_ODR,0)))  // PA0; OneWire Leitung Out
#define Onewire_In  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,0)))  // PA0; OneWire Leitung In

void init_OnewirePorts(void) {
    int temp;
    RCC->APB2ENR |= 0x4;       // enable clock for GPIOA (APB2 Peripheral clock enable register)

//Configure GPIO lines OneWire
    temp = GPIOA->CRL;
    temp &= 0xFFFFFFF0;    // Reset PA0 Configuration Bits
    temp |= 0x7;    // Set PA0 to GP OD mode
    GPIOA->CRL = temp;
}

int init_Onewire(void) {
    int result;
    Onewire_Out=0; // Drives DQ low
    wait_10us(50);
    Onewire_Out=1; // Releases the bus
    wait_10us(7);
    if(!Onewire_In)// Sample for presence low pulse from slave
        result=1;
    else
        result=0;
    return result;
}

void wait_10us(int mal){	//wait for time (ms) that gets passed as argument
    int j;
    for(j = 0; j < 80*mal; j++) {
    }
}

void wait_ms(int ms){	//wait for time (ms) that gets passed as argument
    int j;
    for(j = 0; j < 7987*ms; j++) {
    }
}

int main(void) {
    uart1_init();
	
    init_OnewirePorts();

    if(init_Onewire())
    {
        uart_put_string("Init suc\\n\\r");

    }
    else
        uart_put_string("Init failed");
}







void uart1_init(void)
{
    RCC->APB2ENR |= 0x4; //GPIOA mit einem Takt versorgen

    GPIOA->CRH &= 0xFFFFFF0F;     // reset  PA.9 configuration-bits
    GPIOA->CRH |= 0xB0;           //Tx (PA9) - alt. out push-pull

    GPIOA->CRH &= 0xFFFFF0FF;     //reset PA.10 configuration-bits
    GPIOA->CRH |= 0x400;          //Rx (PA10) - floating

    RCC->APB2ENR |= 0x4000;       //USART1 mit einem Takt versrogen

    USART1->CR1 &= ~0x1000;       // M: Word length:0 --> Start bit, 8 Data bits, n Stop bit
    USART1->CR1 &= ~0x0400;       // PCE (Parity control enable):0 --> No Parity

    USART1->CR2 &= ~0x3000;       // STOP:00 --> 1 Stop bit

    USART1->BRR  = 0x1D4C;        // set Baudrate to 9600 Baud (SysClk 72Mhz)

    USART1->CR1 |= 0x0C;          // enable  Receiver and Transmitter
    USART1->CR1 |= 0x2000;        // Set USART Enable Bit
}





void uart_put_char(char zeichen)
{
    while (!(USART1->SR & 0x80)); //warten, bis die letzten Daten gesendet wurden
    USART1->DR = zeichen;				//Daten in Senderegister schreiben
}

/**
  *****************************************************************************
  * @brief Sends one '\0' terminated String  via USART1
  *
  * @param  string - pointer to '0' terminated String (C convention)
  * @retval none
  *****************************************************************************
  */
void uart_put_string(char *string)
{
    while (*string)  {
        uart_put_char (*string++);
    }
}