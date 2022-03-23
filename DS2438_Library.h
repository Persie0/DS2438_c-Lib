#ifndef DS2438_H
#define DS2438_H
/*----------------------------- Define Pins for Onewire--------------*/
#define GPIOA_IDR GPIOA_BASE + 2*sizeof(uint32_t)    // Calc peripheral address GPIOA IDR
#define GPIOA_ODR GPIOA_BASE + 3*sizeof(uint32_t)   // Calc peripheral address GPIOA ODR

// Calc Bit Band Adress from peripheral address: a = peripheral address b = Bit number
#define BITBAND_PERI(a, b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))

#define Onewire_Out  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_ODR,0)))  // PA0; OneWire Leitung Out
#define Onewire_In  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,0)))  // PA0; OneWire Leitung In

// ===========================================================
//                      VOLTAGE A/D INPUT SELECTION
// ===========================================================

/**
*   \brief Select battery voltage (VDD) as A/D input source.
*/
#define DS2438_INPUT_VOLTAGE_VDD 0

/**
*   \brief Select general purpose input as A/D input source.
*/
#define DS2438_INPUT_VOLTAGE_VAD 1


/*---------------------------Prototypes ---------------------------------------*/
    // ===========================================================
    //                 INITIALIZATION FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Initializes the DS2438.
    *
    *   This function starts the device by checking that
    *   the DS2438 is present on the 1-Wire bus.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
int reset_Onewire(void);

    /**
    *   \brief Check that the DS2438 is present on the bus.
    *
    *   This function checks that the DS2438 is present
    *   on the 1-Wire bus.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
int DS2438_IsDevicePresent(void);

    // ===========================================================
    //                  VOLTAGE CONVERSION FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Start voltage conversion.
    *
    *   This function starts a voltage conversion by sending the
    *   appropriate command to the DS2438.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
uint8_t DS2438_StartVoltageConversion(void);

    /**
    *   \brief Check if voltage conversion completed.
    *
    *   This function checks if a voltage conversion has been completed
    *   by reading the appropriate bit in the Status/Configuration register.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_HasVoltageData(void);

    /**
    *   \brief Get voltage data.
    *
    *   This function gets the voltage data from the DS2438.
    *   \param voltage pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_GetVoltageData(float* mV_voltage);

    /**
    *   \brief Read voltage data.
    *
    *   This function is a blocking function that starts a voltage conversion,
    *   wait until new voltage data are available, and gets the voltage data from 
    *   the DS2438 converting it in float format.
    *   \param voltage pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_ReadVoltage(float* voltage);

    /**
    *   \brief Select input source for A/D conversion.
    *
    *   This function sets the input source for the A/D conversion of the DS2438.
    *   \param input_source selection of AD voltage source:
    *       - #DS2438_INPUT_VOLTAGE_VDD for battery input
    *       - #DS2438_INPUT_VAD for VAD pin
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed when reading scratchpad.
    */
uint8_t DS2438_SelectInputSource(uint8_t input_source);

    // ===========================================================
    //                  TEMPERATURE CONVERSION FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Start temperature conversion.
    *
    *   This function starts a new temperature conversion.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
uint8_t DS2438_StartTemperatureConversion(void);

    /**
    *   \brief Check if temperature conversion is complete. 
    *
    *   This function checks if temperature conversion is complete
    *   by reading the appropriate bit in the Status/Configuration register.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_HasTemperatureData(void);

    /**
    *   \brief Get temperature data in float format.
    *
    *   This function is gets the temperature data from the DS2438 in float format.
    *   It is a non-blocking function since it does not wait for the 
    *   completion of the conversion, that must be started prior to calling this function.
    *   \param temperature pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_GetTemperatureData(float* temperature);

    /**
    *   \brief Read temperature data in float format.
    *
    *   This function is a blocking function that starts a temperature conversion,
    *   wait until new temperature data are available, and gets the temperature data from 
    *   the DS2438 in float format.
    *   \param temperature pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
uint8_t DS2438_ReadTemperature(float* temperature);

    // ===========================================================
    //              CURRENT AND ACCUMULATORS FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Read current data in float format.
    *
    *   This function gets the current data in float format. A positive current
    *   means that the battery is being charged, while a negative current
    *   means that current is flowing out of the battery. In order to correctly
    *   convert the current value from raw to float format, the true value of
    *   the sense resistor must be set in the #DS2348_SENSE_RESISTOR macro.
    *   \param current pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
		
uint8_t DS2438_GetCurrentData(float* current);
    /**
    *   \brief Read content of ICA register.
    *
    *   This function gets the current content of the ICA register,
    *   which is a 8-bit value representing the integration of the
    *   voltage across sense resistor over time..
    *   \param current pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
		
uint8_t DS2438_GetICA(uint8_t* ica);
    /**
    *   \brief Get remaining capacity of the battery.
    *
    *   This function converts the content of the ICA into
    *   remaining capacity according to the formula:
    *   \f$capacity = \dfrac{ICA}{2048\dotR_{sense}} \f$.
    *   \param capacity pointer to variable where voltage data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    *   \retval #DS2438_CRC_FAIL if CRC check failed.
    */
		
uint8_t DS2438_GetCapacity_mAh(float* capacity_mAh);
    // ===========================================================
    //                  CONFIGURATION FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Enable current measurements and ICA.
    *
    *   If IAD is enabeld, current measurements will be taken at a rate
    *   of 36.41 Hz, and the results of the conversion can be retrieved
    *   using the #DS2438_GetCurrentData().
    *   \retval #DS2438_OK if no error was generated.
    *   \retval #DS2438_DEV_NOT_FOUND if no device was found on the bus.
    */
		
uint8_t DS2438_EnableIAD(void);
    /**
    *   \brief Disable current measurements and ICA.
    *
    *   DIsable current conversion and ICA. No current conversion
    *   will be performed by the DS2438.
    *   \retval #DS2438_OK if no error was generated.
    *   \retval #DS2438_DEV_NOT_FOUND if no device was found on the bus.
    */
		
uint8_t DS2438_DisableIAD(void);
    /**
    *   \brief Enable current accumulator.
    *
    *   If CA is enabled, bot charging current accumulator and discharging
    *   current accumulator are used. These registers store the total
    *   charging/discharging current the battery has encountered in
    *   its lifetime.
    *   \retval #DS2438_OK if no error was generated.
    *   \retval #DS2438_DEV_NOT_FOUND if no device was found on the bus.
    */
		
uint8_t DS2438_EnableCA(void);
    /**
    *   \brief Disable current measurements and ICA.
    *
    *   Disable current conversion and ICA. No current conversion
    *   will be performed by the DS2438.
    *   \retval #DS2438_OK if no error was generated.
    *   \retval #DS2438_DEV_NOT_FOUND if no device was found on the bus.
    */
		
uint8_t DS2438_DisableCA(void);
    // ===========================================================
    //                  LOW LEVEL FUNCTIONS
    // ===========================================================
    
    /**
    *   \brief Read one page of data.
    *
    *   This function reads one page of data and return the read samples
    *   in the array passed in as parameter. This function issues a
    *   recall memory command, followed by a read scratchpage command
    *   and the page to be read.
    *   \param page_number the page to be read.
    *   \param page_data pointer to array where data will be stored.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
uint8_t DS2438_ReadPage(uint8_t page_number, uint8_t* page_data);

    /**
    *   \brief Write one page of data.
    *
    *   This function writes one page of data to the DS2438. This can be
    *   used to either configure bits in the registers or to write user bytes
    *   to the device in its EEPROM.
    *   \param page_number the page number to be written.
    *   \param page_data the data to be written to the page.
    *   \retval #DS2438_OK if device is present on the bus.
    *   \retval #DS2438_DEV_NOT_FOUND if device is not present on the bus.
    */
uint8_t DS2438_WritePage(uint8_t page_number, uint8_t * page_data);

    // ===========================================================
    //                  ONE WIRE FUNCTIONS
    // ===========================================================
		
    /**
    *   \brief Write a byte on 1-Wire interface.
    *   
    *   This function writes a byte on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param data the byte to be written.
    */		
void OneWire_WriteByte(int data);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void OneWire_WriteBit(int bit);

    /**
    *   \brief Read a byte on 1-Wire interface.
    *   
    *   This function reads a byte on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \return the byte that was read.
    */
int OneWire_ReadByte(void);

    /**
    *   \brief Read a bit on 1-Wire interface.
    *   
    *   This function reads a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \return the bit that was read.
    */
int OneWire_ReadBit(void);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void init_OnewirePort(void);

		// ===========================================================
    //                  WAIT FUNCTIONS
    // ===========================================================

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void wait_10us(int mal);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void wait_us(int mal);

    // ===========================================================
    //                  UART FUNCTIONS
    // ===========================================================

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void uart1_init(void);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void uart_put_char(char ch);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void uart_put_string(char *string);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void uart_put_page_content(uint8_t* page_data, uint8_t page_number);

    /**
    *   \brief Write a bit on 1-Wire interface.
    *   
    *   This function writes a single bit on the 1-Wire
    *   interface.
    *   \param pin 1-Wire interface pin. This value can be found in the Pin_aliases.h file
    *       in the Pin folder in the Generated source folder.
    *   \param bit the bit to be written.
    */
void uart_put_string_newline(char *string);

#endif
