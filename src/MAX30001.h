// Code for the reading of MAX30001 (ECG & BioZ) by SPI interface 

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <stdlib.h>
#include <zephyr/logging/log.h>

// ADDRESSES
#define MAX30001_REG_INFO       0x0FU
#define MAX30001_REG_CNFG_GEN   0x10U
#define MAX30001_REG_CNFG_ECG   0x15U
#define MAX30001_REG_CNFG_BIOZ  0x18U
#define MAX30001_REG_CNFG_RTOR1 0x1DU

// 1rst way
LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define SPI_DEV_NAME      "SPI_1"
#define MAX30001_SPI_FREQ 5000000  //  check datasheet

static struct spi_config spi_cfg = {
    .frequency = MAX30001_SPI_FREQ,
    .operation = (SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE),
    .slave = 0,
};

static const struct device *spi_dev;

void max30001_configure(void)
{
    int ret;

    uint8_t cnfg_ecg_value = MAX30001_REG_CNFG_ECG; // Configure if needed
    uint8_t cnfg_bioz_value = MAX30001_REG_CNFG_BIOZ; 
    uint8_t cnfg_rtor1_value = MAX30001_REG_CNFG_RTOR1; 

    struct spi_buf tx_buf[] = {
        {
            .buf = &cnfg_ecg_value,
            .len = sizeof(cnfg_bioz_value),
        },
        {
            .buf = &cnfg_ecg_value,
            .len = sizeof(cnfg_ecg_value),
        },
        {
            .buf = &cnfg_bioz_value,
            .len = sizeof(cnfg_bioz_value),
        },
        {
            .buf = &cnfg_bioz_value,
            .len = sizeof(cnfg_bioz_value),
        },
        {
            .buf = &cnfg_rtor1_value,
            .len = sizeof(cnfg_rtor1_value),
        },
        {
            .buf = &cnfg_rtor1_value,
            .len = sizeof(cnfg_rtor1_value),
        },
    };

    struct spi_buf_set tx_set = {
        .buffers = tx_buf,
        .count = ARRAY_SIZE(tx_buf),
    };

    ret = spi_write(spi_dev, &spi_cfg, &tx_set);
    if (ret) {
        LOG_ERR("Error al configurar MAX30001: %d", ret);
    }
}

void max30001_read_data(uint8_t reg_address, uint8_t *rx_data, uint16_t num_samples)
{
    int ret;

    struct spi_buf tx_buf = {
        .buf = &reg_address,
        .len = sizeof(reg_address),
    };

    struct spi_buf rx_buf = {
        .buf = rx_data,
        .len = num_samples,
    };

    ret = spi_transceive(spi_dev, &spi_cfg, &tx_buf, &rx_buf);
    if (ret) {
        LOG_ERR("Error reading MAX30001: %d", ret);
    }
}

void readMAX30001Data(int MAX30001values[3])
{
    

    spi_dev = device_get_binding(SPI_DEV_NAME);
    if (!spi_dev) {
        LOG_ERR("No se pudo obtener el dispositivo SPI");
        return 0;
    }

    max30001_configure();

    // Variables for achieve ECG, BIOZ and RTOR1 data
    uint8_t ecg_data[100];  // Adjust
    uint8_t bioz_data[100]; 
    uint8_t rtor1_data[100];

    
    max30001_read_data(MAX30001_REG_CNFG_ECG, ecg_data, sizeof(ecg_data));
    max30001_read_data(MAX30001_REG_CNFG_BIOZ, bioz_data, sizeof(bioz_data));
    max30001_read_data(MAX30001_REG_CNFG_RTOR1, rtor1_data, sizeof(rtor1_data));

    LOG_HEXDUMP_INF(ecg_data, sizeof(ecg_data), "Datos ECG: ");
    LOG_HEXDUMP_INF(bioz_data, sizeof(bioz_data), "Datos BIOZ: ");
    LOG_HEXDUMP_INF(rtor1_data, sizeof(rtor1_data), "Datos RTOR1: ");

    
    
    MAX30001values[0] = ecg_data;
    MAX30001values[1] = bioz_data;
    MAX30001values[2] = rtor1_data;
    
}
