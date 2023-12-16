/* #include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <nrf_soc.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_nvic.h>
#include <ble.h>
#include <ble_gap.h>
#include <sensor_driver.h> */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <nrf.h>
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_ble_qwr.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "bluetooth.h"
#include "HR_Strss_01.h"

#include "MAX30210.h"
#include "MAX30001.h"
#include "ICP20100.h"
#include "LSM6DSV16X.h"
#include "MAX86140.h"
#include "PWM_Motor.h"
#include "LED_RGB.h"

#include "sensors.h" // Include your sensor header file
#include "stress_detection_algorithm.h" // Include your stress detection algorithm header file

#include <zephyr/settings/settings.h>

#define DEVICE_NAME "StressWristband" 
#define APP_BLE_OBSERVER_PRIO 3       // BLE observer priority

static nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0); // TWI instance
static nrf_drv_spi_t m_spi = NRF_DRV_SPI_INSTANCE(0); // SPI instance for MAX30001

// Initialize SPI (MAX30001 & MAX86140 sensor communication)
static void spi_init(void) {
    ret_code_t err_code;

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin = SPI_SCK_PIN;
    spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
    spi_config.mode = NRF_DRV_SPI_MODE_0;

    err_code = nrf_drv_spi_init(&m_spi, &spi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
}

// Initialize TWI (I2C) for sensor communication
static void twi_init(void) {
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
        .scl                = SCL_PIN,
        .sda                = SDA_PIN,
        .frequency          = NRF_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

// Initialize BLE stack
static void ble_stack_init(void) {
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_OBSERVER_PRIO);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_enable(&m_erase_bonds);
    APP_ERROR_CHECK(err_code);
}

// Main application loop
int main(void) {
    // Initialize microcontroller
    // ...

    // Initialize peripherals
    // ...

    // Initialize BLE stack
    ble_stack_init();

    // Initialize TWI (I2C)
    twi_init();

    // Initialize SPI for MAX30001 sensor
    spi_init();

    // Initialize sensors
    initMAX86140(); // Initialization function for MAX86140 sensor (example)
    initMAX30001();
    
    // Define buffers
    int sample1[] = {158233, 157389, 156587, 156082, 155676, 155377, 155204, 155121, 155027, 155010,
        155045, 155232, 155480, 155740, 155923, 156111, 156350, 156629, 156938, 157262,
        157655, 158051, 158540, 158915, 158754, 158233, 157839, 157704, 157601, 157495,
        157569, 157578, 157631, 157701, 157865, 158064, 158264, 158511, 158744, 159075,
        159303, 159508, 159941, 160288, 160321, 159943, 159355, 158965, 158773, 158718,
        158710, 158607, 158540, 158370, 158349, 158396, 158434, 158550, 158618, 158766,
        158880, 159042, 159242, 159354, 159392, 159003, 158569, 158328, 158184, 158119,
        158180, 158266, 158280, 158325, 158418, 158574, 158732, 158888, 159129, 159321,
        159515, 159757, 160081, 160378, 160561, 160643, 160308, 159857, 159557, 159434,
        159245, 159283, 159328, 159305, 159355, 159425, 159516, 159702, 159893, 160111,
        160327, 160526, 160802};
        
    int MAX30210values[1];
    int MAX86140values[2];
    int MAX30001values[3];
    int ICP20100values[2];
    int LSM6DSV16Xvalues[6];
    int val = 0;

    while (true) {
    	
        // Acquire sensor data
        readMAX86140Data(MAX86140values); //Reading data from MAX86140 sensor's FIFO (example)
        readMAX30001Data(MAX30001values);
        readMAX30210Data(MAX30210values);
        readICP20100Data(ICP20100values);
        readLSM6DSV16XData(LSM6DSV16Xvalues);
        Motor(val); 
        LED3(val);

        // Run stress detection algorithm
        // runStressDetectionAlgorithm(); //Function to run the stress detection algorithm (example)
        int valhr = HR(sample1, sizeof(sample1) / sizeof(sample1[0]));
    	//printf("%d\n", valhr);
    	runStressDetectionAlgorithm(5, valhr);
    	return 0;

        // Implement power management strategies
        // ...

        // Send data over Bluetooth
        // ...

        // Sleep or other idle tasks
        // ...
    }
}
