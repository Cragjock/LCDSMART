

#ifndef __myi2c_h__
#define __myi2c_h__

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
//#include <linux/i2c.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <linux/swab.h>     // for swab16 will this work ??

#define LBYTE(LB) (LB & 0x0FF)
#define HBYTE(HB) ((HB & 0xF00)>>8)



#define IODIR 0x00  // I/O direction A
#define IPOL 0x01  // I/O polarity A
#define GPINTEN 0x02  // interupt enable A
#define DEFVAL 0x03  // register default value A (interupts)
#define INTCON 0x04  // interupt control A
#define IOCON 0x05  // I/O config (also 0x0B)
#define GPPU 0x06  // port A pullups
#define INTF 0x07  // interupt flag A (where the interupt came from)
#define INTCAP 0x08  // interupt capture A (value at interupt is saved here)
#define GPIO 0x09  // port A
#define OLAT 0x0a  // output latch A

// I/O config
#define SEQOP_OFF 0x20  // incrementing address pointer
#define SEQOP_ON 0x00
#define DISSLW_ON 0x10  // slew rate
#define DISSLW_OFF 0x00
#define HAEN_ON 0x08  // hardware addressing
#define HAEN_OFF 0x00
#define ODR_ON 0x04  // open drain for interupts
#define ODR_OFF 0x00
#define INTPOL_HIGH 0x02  // interupt polarity
#define INTPOL_LOW 0x00


#define I2C_SLAVE_ADDRESS     0x20    // device slave address, hard coded




char buf[80];
typedef unsigned char BYTE;
char input[5];

int set_slave_addr(int sl_addr);
int I2C_Open(int bus, int addr);
void I2C_Close(int filep);
int32_t myI2C_read_byte(int file);
int myI2C_write_data(int file, uint8_t command_reg, uint8_t data);
int myI2C_write_byte(int file, uint8_t data);

int all_on(int file);



//extern int32_t  i2c_smbus_read_word_data(const struct i2c_client *client, uint8_t command);
//extern int32_t i2c_smbus_write_word_data(const struct i2c_client *client, uint8_t command, uint16_t value);

/*********************
static inline int32_t i2c_smbus_read_word_swapped(const struct i2c_client *client, uint8_t command)
    {
        int32_t value = i2c_smbus_read_word_data(client, command);
        return (value < 0) ? value : swab16(value);
    }

static inline int32_t i2c_smbus_write_word_swapped(const struct i2c_client *client, uint8_t command, uint16_t value)
    {
        return i2c_smbus_write_word_data(client, command, swab16(value));
    }
*********************/

#endif
/*** done ***/



