/* #include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <nrf_soc.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_nvic.h>
#include <ble.h>
#include <ble_gap.h>
#include <sensor_driver.h> */

#include <stdbool.h>
#include <stdint.h>
#include <nrf.h>
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_ble_qwr.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "sensors.h" // Include your sensor header file
#include "stress_detection_algorithm.h" // Include your stress detection algorithm header file

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

    while (true) {
        // Acquire sensor data
        readMAX86140Data(); //Reading data from MAX86140 sensor's FIFO (example)
        readMAX30001Data();

        // Run stress detection algorithm
        runStressDetectionAlgorithm(); //Function to run the stress detection algorithm (example)

        // Implement power management strategies
        // ...

        // Send data over Bluetooth
        // ...

        // Sleep or other idle tasks
        // ...
    }
}
