#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h>

#define SLEEP_TIME_MS   1000

//0X63 = 0110 0011          0x31 = 0011 0001            0xC6 = 1100 0110
#define ICP20100_I2C_ADDRESS   0x63U // p. 20 

// Pressure
#define PRESS_ABS_LSB 0xC7U
#define PRESS_ABS_MSB 0xC8U
#define PRESS_DELTA_LSB 0xC9U
#define PRESS_DELTA_MSB 0xCAU

#define PRESS_DATA_0 0xFAU
#define PRESS_DATA_1 0xFBU
#define PRESS_DATA_2 0xFCU

// Temp
#define TEMP_DATA_0 0xFDU
#define TEMP_DATA_1 0xFEU
#define TEMP_DATA_2 0xFFU

#define I2C_NODE DT_NODELABEL(i2c0)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);


void readICP20100Data(int ICP20100values[2])
{
    if (!i2c_dev) {
        printk("Error al obtener el controlador I2C\n");
        return;
    }

    uint8_t data[2]; // Buffer sensor data
    uint32_t pressure, temperature;

    // Configure data transfer (press)
    uint8_t pressure_register[2] = {PRESS_DATA_0, PRESS_DATA_1};
    int ret = i2c_write(i2c_dev, pressure_register, sizeof(pressure_register), ICP20100_I2C_ADDRESS);
    if (ret < 0) {
        printk("Error al escribir la dirección de presión: %d\n", ret);
        return;
    }

    ret = i2c_read(i2c_dev, data, sizeof(data), ICP20100_I2C_ADDRESS);
    if (ret < 0) {
        printk("Error al leer datos de presión: %d\n", ret);
        return;
    }

    // Convertir datos de presión
    pressure = (data[1] << 8) | data[0];

    // Configure data transfer (temp)
    uint8_t temp_register[2] = {TEMP_DATA_0, TEMP_DATA_1};
    ret = i2c_write(i2c_dev, temp_register, sizeof(temp_register), ICP20100_I2C_ADDRESS);
    if (ret < 0) {
        printk("Error al escribir la dirección de temperatura: %d\n", ret);
        return;
    }

    ret = i2c_read(i2c_dev, data, sizeof(data), ICP20100_I2C_ADDRESS);
    if (ret < 0) {
        printk("Error al leer datos de temperatura: %d\n", ret);
        return;
    }

    // Convertir datos de temperatura
    temperature = (data[1] << 8) | data[0];

    // Ahora los datos están en las variables "pressure" y "temperature"
    //printk("Presión: %u\n", pressure);
    //printk("Temperatura: %u\n", temperature);
    
    ICP20100values[0] = pressure;
    ICP20100values[1] = temperature;

}

