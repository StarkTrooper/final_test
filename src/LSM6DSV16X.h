//#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdlib.h> 
#include <stdio.h>  


#define I2C_NODE DT_NODELABEL(i2c0)
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);
//static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C_NODE);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/** I2C Device Address 8 bit format  if SA0=0 -> D5 if SA0=1 -> D7 **/
#define LSM6DSV16X_I2C_ADD_L                    0xD5U
#define LSM6DSV16X_I2C_ADD_H                    0xD7U
/** Device Identification (Who am I) **/
#define LSM6DSV16X_ID                           0x70U

#define LSM6DSV16X_WHO_AM_I                    0x0FU
#define LSM6DSV16X_I2C_ADDRESS   0x0F // p. 22 Datasheet, viene de D5>1

#define LSM6DSV16X_OUTX_L_G                    0x22U // p. 53
#define LSM6DSV16X_OUTX_H_G                    0x23U
#define LSM6DSV16X_OUTY_L_G                    0x24U
#define LSM6DSV16X_OUTY_H_G                    0x25U
#define LSM6DSV16X_OUTZ_L_G                    0x26U
#define LSM6DSV16X_OUTZ_H_G                    0x27U

#define LSM6DSV16X_OUTX_L_A                    0x28U
#define LSM6DSV16X_OUTX_H_A                    0x29U
#define LSM6DSV16X_OUTY_L_A                    0x2AU
#define LSM6DSV16X_OUTY_H_A                    0x2BU
#define LSM6DSV16X_OUTZ_L_A                    0x2CU
#define LSM6DSV16X_OUTZ_H_A                    0x2DU

#define SENSOR_CHAN_ACCEL_XYZ 0x10 
#define SENSOR_CHAN_GYRO_XYZ 0x11  

#define LSM6DSV16X_I2C_ADDR 0x70
#define LSM6DSV16X_REG_DATA 0x0F


void readLSM6DSV16XData(int LSM6DSV16Xvalues[6])
{
    //const struct device *i2c_dev;
    uint8_t accel_x_lsb, accel_x_msb, accel_y_lsb, accel_y_msb, accel_z_lsb, accel_z_msb;
    uint8_t gyro_x_lsb, gyro_x_msb, gyro_y_lsb, gyro_y_msb, gyro_z_lsb, gyro_z_msb;

    if (!i2c_dev) {
        printk("I2C: Device not found.\n");
        return;
    }

    // Leer los registros del acelerómetro en el eje X
    if (i2c_read(i2c_dev, &accel_x_lsb, 1, LSM6DSV16X_OUTX_L_A) < 0 ||
        i2c_read(i2c_dev, &accel_x_msb, 1, LSM6DSV16X_OUTX_H_A) < 0) {
        printk("Error reading accelerometer X data\n");
        return;
    }

    // Leer los registros del acelerómetro en el eje Y
    if (i2c_read(i2c_dev, &accel_y_lsb, 1, LSM6DSV16X_OUTY_L_A) < 0 ||
        i2c_read(i2c_dev, &accel_y_msb, 1, LSM6DSV16X_OUTY_H_A) < 0) {
        printk("Error reading accelerometer Y data\n");
        return;
    }

    // Leer los registros del acelerómetro en el eje Z
    if (i2c_read(i2c_dev, &accel_z_lsb, 1, LSM6DSV16X_OUTZ_L_A) < 0 ||
        i2c_read(i2c_dev, &accel_z_msb, 1, LSM6DSV16X_OUTZ_H_A) < 0) {
        printk("Error reading accelerometer Z data\n");
        return;
    }

    // Leer los registros del giroscopio en el eje X
    if (i2c_read(i2c_dev, &gyro_x_lsb, 1, LSM6DSV16X_OUTX_L_G) < 0 ||
        i2c_read(i2c_dev, &gyro_x_msb, 1, LSM6DSV16X_OUTX_H_G) < 0) {
        printk("Error reading gyroscope X data\n");
        return;
    }

    // Leer los registros del giroscopio en el eje Y
    if (i2c_read(i2c_dev, &gyro_y_lsb, 1, LSM6DSV16X_OUTY_L_G) < 0 ||
        i2c_read(i2c_dev, &gyro_y_msb, 1, LSM6DSV16X_OUTY_H_G) < 0) {
        printk("Error reading gyroscope Y data\n");
        return;
    }

    // Leer los registros del giroscopio en el eje Z
    if (i2c_read(i2c_dev, &gyro_z_lsb, 1, LSM6DSV16X_OUTZ_L_G) < 0 ||
        i2c_read(i2c_dev, &gyro_z_msb, 1, LSM6DSV16X_OUTZ_H_G) < 0) {
        printk("Error reading gyroscope Z data\n");
        return;
    }

    // Combina LSB y MSB para obtener los valores finales
    int16_t accel_x = (accel_x_msb << 8) | accel_x_lsb;
    int16_t accel_y = (accel_y_msb << 8) | accel_y_lsb;
    int16_t accel_z = (accel_z_msb << 8) | accel_z_lsb;

    int16_t gyro_x = (gyro_x_msb << 8) | gyro_x_lsb;
    int16_t gyro_y = (gyro_y_msb << 8) | gyro_y_lsb;
    int16_t gyro_z = (gyro_z_msb << 8) | gyro_z_lsb;

    // Imprime los valores del acelerómetro y el giroscopio
    /*
    printk("Accel X: %d\n", accel_x);
    printk("Accel Y: %d\n", accel_y);
    printk("Accel Z: %d\n", accel_z);
    printk("Gyro X: %d\n", gyro_x);
    printk("Gyro Y: %d\n", gyro_y);
    printk("Gyro Z: %d\n", gyro_z);
    */
    
    LSM6DSV16Xvalues[0] = accel_x;
    LSM6DSV16Xvalues[1] = accel_y;
    LSM6DSV16Xvalues[2] = accel_z;
    LSM6DSV16Xvalues[3] = gyro_x;
    LSM6DSV16Xvalues[4] = gyro_y;
    LSM6DSV16Xvalues[5] = gyro_z;
}

