/**
  ******************************************************************************
  * @file    DS2438.c
  * @author  Daniel Marek, Marvin Perzi 
  * @version V01
  * @date    19-01-2022
  * @brief   
  ******************************************************************************
  * @history 19-01-2022: Perzi/Marek creation
  ******************************************************************************
  */

#include <stm32f10x.h>
#include <stdio.h>
#include "DS2438_Library.h"

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
    init_OnewirePort();
    float voltage, temperature, current, capacity = 0;
    char msg[50];

    if(!DS2438_IsDevicePresent())
    {
        uart_put_string_newline("no Device found");
    }
    else
    {
        uart_put_string_newline("Device present");

        DS2438_EnableIAD();
        DS2438_EnableCA();
        DS2438_SelectInputSource(DS2438_INPUT_VOLTAGE_VAD);


        while (1) {
            if (DS2438_ReadVoltage(&voltage)) {
                sprintf(msg, "V: %f", voltage);
                uart_put_string_newline(msg);

            } else {
                uart_put_string_newline("Could not read voltage");
            }
            if (DS2438_GetCurrentData(&current)) {
                sprintf(msg, "mA: %.8f", current);
                uart_put_string_newline(msg);
            } else {
                uart_put_string_newline("Could not read current");
            }
            if (DS2438_GetCapacity_mAh(&capacity))
            {
                sprintf(msg, "Remaining Capacity in mAh: %.8f", capacity);
                uart_put_string_newline(msg);
            }
            else
            {
                uart_put_string_newline("Could not read current");
            }
            if (DS2438_ReadTemperature(&temperature))
            {
                sprintf(msg, "Temperature: %.8f °C", temperature);
                uart_put_string_newline(msg);
            }
            else
            {
                uart_put_string_newline("Could not read temperature");
            }
            uart_put_string_newline("");
            uart_put_string_newline("Pagedata (00h-06h):");
            //printing all the content from the pages
            for (uint8_t page = 0; page < 7; page++)
            {
                uint8_t page_data[9];
                DS2438_ReadPage(page, page_data);
                uart_put_page_content(page_data, page);
            }
            wait_10us(400000);
        }
    }}
