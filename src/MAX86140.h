// Code for the reading of MAX86140 (Sp02) by SPI interface
//  
//#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
// 
#define MAX86140_CHIPID_REG     0xFF   
#define ADD_REG_WHO_AM_I        0xFF
#define UC_WHO_AM_I_DEFAULT_VALUE 0xFF // check difference

#define LED1_SEQ_REG 			0x20 // use sequence
#define LED2_SEQ_REG 			0x21
#define LED3_SEQ_REG 			0x22

#define UC_ADD_REG_CTRL_4_CFG_VALUE 0xF9 // check
#define ADD_REG_CTRL_4          0x12    // check

#define LED1_REG				0x23
#define LED2_REG				0x24
#define LED3_REG				0x25

#define PPG_SYNC_CTRL_REG		0x10
#define PPG_CONFIG              0x11
#define MEMORY_CRTL				0xF2
#define MEMORY_INDEX			0xF3
#define MEMORY_DATA				0xF4

#define FIFO_WRITE_PTR          0x04
#define FIFO_READ_PNTR          0x05
#define FIFO_DATA_REG           0x08
#define FIFO_CONFIG             0x09
#define MEMORY                  0xF2

#define SPI_BUFSIZE  8   //SPI Communication buffer size
#define SPI_INSTANCE  0 //SPI Instance to be used

uint8_t   spi_tx_buf[SPI_BUFSIZE]; // spi tx buffer 
uint8_t   spi_rx_buf[SPI_BUFSIZE]; // spi rx buffer

//spi_buf and spi_buf_set structures to hold the buffer values for SPI
struct spi_buf tx_buf_arr; 
struct spi_buf_set tx;
struct spi_buf rx_buf_arr;
struct spi_buf_set rx ;

volatile  uint8_t   SPIReadLength, SPIWriteLength; // variables to hold read and write lengths


#define SPI_NODE DT_NODELABEL(spi1)
static const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);
//const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE) ;
struct spi_config spi_cfg ;
struct spi_cs_control chip;


#define MY_GPIO1 DT_NODELABEL(gpio1)
#define GPIO_1_CS    3  // Chip select
const struct device *gpio1_dev = DEVICE_DT_GET(MY_GPIO1);


/* set read single command. Attention: command must be 0x3F at most */
#define SET_READ_SINGLE_CMD(x)			(x | 0x80)
/* set read multiple command. Attention: command must be 0x3F at most */
#define SET_READ_MULTI_CMD(x)			(x | 0xC0)
/* set write single command. Attention: command must be 0x3F at most */
#define SET_WRITE_SINGLE_CMD(x)			(x & (~(0xC0)))
/* set write multiple command. Attention: command must be 0x3F at most */
#define SET_WRITE_MULTI_CMD(x)			(x & (~(0x80))	\
                                                 x |= 0x40)

/* A function to write into sensor Internal Register */
static void MAX86140_write_reg(int reg, int data)
{
    SPIWriteLength = 2; // set the spi write length to 2 bytes
    SPIReadLength = 0; // set the read length
    
    spi_tx_buf[0] = SET_WRITE_SINGLE_CMD(reg); // set the first byte which is a write command
    spi_tx_buf[1] = data; // A byte of data to be sent

   //specifying the tx and rx buffer specific to zephyr's SPI drivers
   	tx_buf_arr.buf = spi_tx_buf;
    tx_buf_arr.len = SPIWriteLength;
    tx.buffers = &tx_buf_arr;
    tx.count = 1;
	
    rx_buf_arr.buf = spi_rx_buf;
    rx_buf_arr.len = SPIReadLength;

    rx.buffers = &rx_buf_arr ;
    rx.count = 1;

	int error = spi_transceive(spi_dev, &spi_cfg, &tx,&rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
	}   
}

/* A function to read a value from the internal register of max86140 */
int MAX86140_read_reg(int reg)
{	
    /* Set the read command for reading a single byte */
    spi_tx_buf[0] = SET_READ_SINGLE_CMD(reg);
  
   //specifying the tx and rx buffer specific to zephyr's SPI drivers
   	tx_buf_arr.buf = spi_tx_buf;
    tx_buf_arr.len = 2;

    tx.buffers = &tx_buf_arr;
    tx.count = 1;
	
    rx_buf_arr.buf = spi_rx_buf;
    rx_buf_arr.len = 2;

    rx.buffers = &rx_buf_arr ;
    rx.count = 1;

	int error = spi_transceive(spi_dev,&spi_cfg,&tx,&rx);
	if(error != 0){
		printk("SPI transceive error: %i\n", error);
		return error;
	}

    return spi_rx_buf[1];
}

void printbuffers()
{
    printk("SPI TX: ");
	for (int j = 0; j<sizeof(spi_tx_buf); j++)
	{
		printk(" %x",spi_tx_buf[j]);
	}
	printk("\r\nSPI RX: ");
	for (int j = 0; j<sizeof(spi_rx_buf); j++)
	{
		printk(" %x",spi_rx_buf[j]);
	}
    printk("\r\n");
}

// Configuration of the SPI
void SPI_Init(void)
{
 	if (!device_is_ready(spi_dev)) {
        /* Device is not ready to use */
		printk("\r\nStop device not ready\r\n");
	}
    
	    if (chip.gpio.pin == NULL)
		    printk("CS CHIP PIN NOT READY\r\n");

    MAX86140_init();
}

struct spi_config spi_cfg = {
        .frequency = 125000U, //4000000, // p. 8
        .operation = SPI_WORD_SET(8), // 1 byte, 8 bits
        .slave = 0, // specify slave
};

// Initialize sensor
void MAX86140_init(void)
{   
    int regval;

    /* Read the who am i register to confirm sensor is communicating with our nrf device */
    regval = MAX86140_read_reg(ADD_REG_WHO_AM_I);

    /* check the who am i register value and compare it with default who am i value for max86140 
        if who am i is correct then proced */
    if( regval == UC_WHO_AM_I_DEFAULT_VALUE )
    {
        MAX86140_write_reg(ADD_REG_CTRL_4, UC_ADD_REG_CTRL_4_CFG_VALUE); // Specify

            /* verify written value */
        regval = MAX86140_read_reg(ADD_REG_CTRL_4);

            /* if written value is different */
        if( regval != UC_ADD_REG_CTRL_4_CFG_VALUE )
        {
                /* ERROR: stay here... */
            while(1)
            {
                printk("\r\nWrite Reg ERR\r\n");
                k_msleep(500);
            }
        }
    }
    else
    {
      /* ERROR: stay here... */
      while(1)
        {
            printk("\nDevice does not exist\r\n");
            k_msleep(10000);
        }			   
    }
}

void get_LEDS( int* x, int* y)
{

    *x = (MAX86140_read_reg(LED1_REG) << 8);
    *y = (MAX86140_read_reg(LED2_REG) << 8);
}

void readMAX86140Data(int MAX86140values[2])
{
	
    //Call the SPI initialization function
    SPI_Init();
    
	int led1, led2;
	//values that hold the values of acceleration in mg
    while(true)
      {
		get_LEDS(&led1,&led2);		
		printk("hr= %6d spo2= %6d \r\n", led1, led2);
		k_msleep(400);
      }
      
      MAX86140values[0] = led1;
      MAX86140values[1] = led2;

}

