#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>

#define MAX30210_I2C_ADDRESS   0x80U
#define TEMP_DATA_MSB          0x2BU 
#define TEMP_DATA_LSB          0x2CU 


#define I2C_NODE DT_NODELABEL(i2c0)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);


void readMAX30210Data(int MAX30210values[1])
{
    if (!i2c_dev) {
        printk("I2C Error \n");
        return;
    }

    uint8_t data[2]; 
    uint32_t temperature;

    uint8_t temp_register[2] = {TEMP_DATA_LSB, TEMP_DATA_MSB};
    int ret = i2c_write(i2c_dev, temp_register, sizeof(temp_register), MAX30210_I2C_ADDRESS);
    if (ret < 0) {
        printk("Writting Error: %d\n", ret);
        return;
    }

    ret = i2c_read(i2c_dev, data, sizeof(data), MAX30210_I2C_ADDRESS);
    if (ret < 0) {
        printk("Reading Error: %d\n", ret);
        return;
    }

    //temperature = (data[1] << 8) | data[0];
    MAX30210values[0] = (data[1] << 8) | data[0];

    printk("Temperatura corporal: %u\n", temperature);
    //printk("MAX30210: %.2f °C \n", temperature);
    

}

