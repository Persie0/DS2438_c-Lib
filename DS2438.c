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

void uart_put_page_content(int page_data[]);
void uart_put_string_newline(char *string);
int reset_Onewire(void);
void init_OnewirePort(void);
void wait_10us(int mal);
int enableIAD(void);
int DS2438_ReadPage(int page_number, int* page_data);
int DS2438_WritePage(int page_number, int* page_data);
void wait_us(int factor);
void OneWire_WriteByte(int data);
int OneWire_ReadBit();
int OneWire_ReadByte();
int DS2438_IsDevicePresent(void);
void OneWire_WriteBit(int bit);

/*----------------------------- Define Pins for Onewire--------------*/
#define GPIOA_IDR GPIOA_BASE + 2*sizeof(uint32_t)    // Calc peripheral address GPIOA IDR
#define GPIOA_ODR GPIOA_BASE + 3*sizeof(uint32_t)   // Calc peripheral address GPIOA ODR

// Calc Bit Band Adress from peripheral address: a = peripheral address b = Bit number
#define BITBAND_PERI(a, b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))

#define Onewire_Out  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_ODR,0)))  // PA0; OneWire Leitung Out
#define Onewire_In  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,0)))  // PA0; OneWire Leitung In


// ===========================================================
//                      ERROR CODES
// ===========================================================

/**
*   \brief Device was not found on the 1-Wire interface.
*/
#define DS2438_DEV_NOT_FOUND    0

/**
*   \brief CRC Check failed.
*/
#define DS2438_CRC_FAIL         0

/**
*   \brief Operation finished unsuccessfully.
*/
#define DS2438_ERROR            0

/**
*   \brief Bad parameter error.
*/
#define DS2438_BAD_PARAM        0

/**
*   \brief Operation was successful
*/
#define DS2438_OP_SUCCESS   1

// ===========================================================
//                      1-WIRE COMMANDS
// ===========================================================

/**
*   \brief Command to read ROM.
*/
#define DS2438_READ_ROM 0x33

/**
*   \brief Command to skip ROM match/search.
*   This command can save time in a single-drop bus system by allowing the bus master to access the
*   memory functions without providing the 64-bit ROM code.
*/
#define DS2438_SKIP_ROM 0xCC

/**
*   \brief Command to trigger voltage conversion.
*/
#define DS2438_VOLTAGE_CONV 0xB4

/**
*   \brief Command to trigger temperature conversion.
*/
#define DS2438_TEMP_CONV 0x44

/**
*   \brief Command to perform memory recall from EEPROM.
*   recalls the stored values in EEPROM / SRAM page xxh to the scratchpad page xxh.
*/
#define DS2438_RECALL_MEMORY 0xB8

/**
*   \brief Command to read scratchpad.
*/
#define DS2438_READ_SCRATCHPAD 0xBE

/**
*   \brief Command to write scratchpad.
*/
#define DS2438_WRITE_SCRATCHPAD 0x4E

/**
*   \brief Command to copy scratchpad.
*/
#define DS2438_COPY_SCRATCHPAD 0x48


/**
  *****************************************************************************
  * @brief Ouput 8 Bit value to 8 LED Array of PMOD_LED (connected to PMOD1)
  *
  * @param value - 8 bit value to display on LED Array
  * @retval none 
  *****************************************************************************
  */

int main(void) {
    uart1_init();
    //float voltage, temperature, current, capacity = 0;
    init_OnewirePort();

    if(DS2438_IsDevicePresent())
    {
        uart_put_string_newline("Device present");

    }
    else
        uart_put_string_newline("no Device found");
}

void wait_10us(int factor){	//wait for 10us multiplied by the value that gets passed as argument
    int j;
    for(j = 0; j < 80*factor; j++) {
    }
}

void wait_us(int factor){	//wait for 1us multiplied by the value that gets passed as argument
    int j;
    for(j = 0; j < 8*factor; j++) {
    }
}

void uart_put_page_content(int page_data[])
{
    char msg[50];
    sprintf(msg, "%d %d %d %d %d %d %d %d", page_data[0], page_data[1],
            page_data[2], page_data[3],
            page_data[4], page_data[5],
            page_data[6], page_data[7]);
    uart_put_string("Current page content (MSB to LSB): ");
    uart_put_string_newline(msg);
}

void init_OnewirePort(void) {
    int temp;
    RCC->APB2ENR |= 0x4;       // enable clock for GPIOA (APB2 Peripheral clock enable register)

//Configure GPIO lines OneWire
    temp = GPIOA->CRL;
    temp &= 0xFFFFFFF0;    // Reset PA0 Configuration Bits
    temp |= 0x7;    // Set PA0 to GP OD mode
    GPIOA->CRL = temp;
}

int reset_Onewire(void) {
    int result;
    Onewire_Out=0; // Drives DQ low
    wait_10us(50);
    Onewire_Out=1; // Releases the bus
    wait_10us(7);

    return Onewire_In; //0 if low pulse from slave detected, 1 if not
}

//returns 1 if device is ok
int DS2438_IsDevicePresent(void)
{
    // check if device is present on the bus
        return reset_Onewire() == 0;
}

int DS2438_EnableIAD(void)// Enable current measurements and ICA
{
    int page_data[9];
    //Enable Current A/D Control Bit (Set bit 0 in byte 0 of page 0)
    if (DS2438_ReadPage(0x00, page_data))//read Page 0 successful?
    {
        // set bit 0
        page_data[0] = page_data[0] | 0x01;
        return DS2438_WritePage(0x00, page_data);//write Page 0 successful?
    }
    return DS2438_ERROR;
}

int DS2438_DisableIAD(void)// Disable current measurements and ICA
{
    // Clear bit 0 in byte 0 of page 0
    int page_data[9];
    if (DS2438_ReadPage(0x00, page_data))//read Page 0 successful?
    {
        // Clear bit 0
        page_data[0] = page_data[0] & (~0x01);
        return DS2438_WritePage(0x00, page_data);
    }
    return DS2438_ERROR;
}

int DS2438_EnableCA(void)
{
    // Set bit 1 in byte 0 of page 0
    int page_data[9];
    if (DS2438_ReadPage(0x00, page_data))//read Page 0 successful?
    {
        // set bit 1
        page_data[0] = page_data[0] | 0x02;
        return DS2438_WritePage(0x00, page_data);
    }
    return DS2438_ERROR;

}

int DS2438_DisableCA(void)
{
    // Clear bit 1 in byte 0 of page 0
    int page_data[9];
    if (DS2438_ReadPage(0x00, page_data))//read Page 0 successful?
    {
        // Clear bit 1
        page_data[0] = page_data[0] & (~0x02);
        return DS2438_WritePage(0x00, page_data);//write current Byte 0 with bit 1 cleared
    }
    return DS2438_ERROR;
}

// Read one page of data, cbv - return page data
int DS2438_ReadPage(int page_number, int* page_data)
{
    if (page_number > 0x07)//there are only pages from 0x00 to 0x07
        return DS2438_BAD_PARAM;
    else
    {
        if (DS2438_IsDevicePresent())// Reset sequence
        {
            // Skip ROM command
            OneWire_WriteByte(DS2438_SKIP_ROM);
            // Recall memory command
            OneWire_WriteByte(DS2438_RECALL_MEMORY);
            OneWire_WriteByte(page_number);//page number from which to recall memory
            if (DS2438_IsDevicePresent())// Reset sequence
            {
                // Skip ROM command 
                OneWire_WriteByte(DS2438_SKIP_ROM);
                // Read scratchpad command
                OneWire_WriteByte(DS2438_READ_SCRATCHPAD);
                //  which scratchpad page number to read from
                OneWire_WriteByte(page_number);
                // Read nine bytes
                for (int i = 0; i < 9; i++) //eight 8-byte pages, 9th byte contains a cyclic redundancy check (CRC) byte
                {
                    page_data[i] = OneWire_ReadByte();
                }
                return DS2438_OP_SUCCESS;
            }
        }
    }
    return DS2438_DEV_NOT_FOUND;
}

// Write one page of data
int DS2438_WritePage(int page_number, int * page_data)
{
    if (page_number > 0x07)//there are only pages 0x00 to 0x07
        return DS2438_BAD_PARAM;
    else
    {
        // Reset sequence
        if (DS2438_IsDevicePresent())
        {
            // Skip ROM command [CCh]
            OneWire_WriteByte(DS2438_SKIP_ROM);
            // Write scratchpad command
            OneWire_WriteByte(WRITE_SCRATCHPAD);
            // Write page data followed by page data
            OneWire_WriteByte(page_number);
            for (int i = 0; i < 9; i++)
            {
                OneWire_WriteByte(page_data[i]);
            }
            if (DS2438_IsDevicePresent())
            {
                // Skip ROM command 
                OneWire_WriteByte(SKIP_ROM);
                // Copy scratchpad command
                OneWire_WriteByte(COPY_SCRATCHPAD);
                // Write page number
                OneWire_WriteByte(page_number);
                return DS2438_OP_SUCCESS;
            }
        }
    }
    return DS2438_DEV_NOT_FOUND;
}

void OneWire_WriteByte(int data)
{
    int bit;

    // Loop to write each bit in the byte, LS-bit first
    for (bit = 0; bit < 8; bit++)
    {
        OneWire_WriteBit(data & 0x01);

        // shift the data byte for the next bit
        data >>= 1;
    }

}

void OneWire_WriteBit(int bit)
{
    if (bit)
    {
        // Write '1' bit
        Onewire_Out=0; // Drives DQ low
        wait_10us(1); // Complete the Write 1 Low Time time
        Onewire_Out=0; // Releases the bus
        wait_10us(7); // Complete the Time Slot time and Recovery Time
    }
    else
    {
        // Write '0' bit
        Onewire_Out=0; // Drives DQ low
        wait_10us(7); // Complete the Write 0 Low Time and Time Slot time
        Onewire_Out=1; // Releases the bus
        wait_10us(1);// Complete the Recovery Time
    }
}

int OneWire_ReadByte()
{
    int bit, result=0x0;
    for (bit = 0; bit < 8; bit++)
    {
        // shift the result to get it ready for the next bit
        result >>= 1;

        // if result is one, then set MS bit (first set Bit gets shifted to LSB)
        if (OneWire_ReadBit())
            result |= 0x80;
    }
    return result;
}

int OneWire_ReadBit()
{
    int result;
    Onewire_Out=0; // Drives DQ low
    wait_us(4); // Complete the Read Low Time
    Onewire_Out=1; // Releases the bus
    wait_us(4);//be sure Onewire_Out is 1 but to be still in the Read Data Valid time window
    result = Onewire_In // Sample the bit value from the slave
    wait_10us(6); // Complete the Time Slot time and Recovery Time

    return result;
}

int DS2438_StartVoltageConversion(void)
{
    // Reset sequence
    if (DS2438_IsDevicePresent())
    {
        // Write read rom command
        OneWire_WriteByte(DS2438_SKIP_ROM);
        OneWire_WriteByte(DS2438_VOLTAGE_CONV);
        return DS2438_OP_SUCCESS;
    }
    return DS2438_DEV_NOT_FOUND;
}

int DS2438_ReadVoltage(float* voltage)
{
    if (DS2438_StartVoltageConversion())
    {
        while(DS2438_HasVoltageData());
        return DS2438_GetVoltageData(voltage);
    }
    return DS2438_ERROR;
}

int DS2438_HasVoltageData(void)
{
    int page_data[9];
    if (DS2438_ReadPage(0x00, page_data))
    {
        //TODO: CRC check

        //the DS2438 will output “0” on the bus as
        //long as it is busy making a voltage measurement;
        //it will return a “1” when the conversion is complete
        if ((page_data[0] & (0x01 << 6)) == 0)
        {
            return DS2438_OP_SUCCESS;
        }
        else
        {
            return DS2438_ERROR;
        }

    }
    return DS2438_DEV_NOT_FOUND;
}

int DS2438_GetVoltageData(float* mV_voltage)
{
    int page_data[9];
    if (DS2438_ReadPage(0x00, page_data))
    {
        //TODO: CRC check
        //getting the 2 VOLTAGE REGISTER byte:
        int volt_lsb = page_data[3];
        int volt_msb = page_data[4];
        //volt_msb only has 2 valid bits, rest is 0
        //moving the msb by 8 bits so the result is MSB+LSB
        //divide by 10 because the resolution is 10 mV
        //resulting voltage is in mV
        *mV_voltage = (((volt_msb & 0xFF) << 8) | (volt_lsb)) / 10.0;
        return DS2438_OP_SUCCESS;
    }
    return DS2438_DEV_NOT_FOUND;
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

void uart_put_string_newline(char *string)
{
    while (*string)  {
        uart_put_char (*string++);
    }
    uart_put_char ('\r');
    uart_put_char ('\n');
}