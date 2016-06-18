
#include "myi2c.h"


static int slave_address = 0x20;
static int file;
const char * i2cdev[2] = {"/dev/ic2-0","/dev/i2c-1"};

int i = 0xFF;   // just set it

static uint8_t MCP_control_byte(uint8_t rw_cmd, uint8_t hw_addr);

/*****************************************/
int set_slave_addr(int sl_addr)
{
    if(sl_addr<0 | sl_addr>255)
    {
        printf("slave address NG");
        return -1;
    }
    slave_address = sl_addr;
    return slave_address;
}

/*******************************************************************/
int I2C_Open(int bus, int addr)
{
    int file;
    file = open(i2cdev[bus], O_RDWR);
    if (file == -1)
        {
            perror(i2cdev[bus]);
            int errsv = errno;
            return -1;
        }

    if (ioctl(file, I2C_SLAVE, addr) < 0)
    {
        perror("Failed to acquire bus access and/or talk to slave");
        //exit(1);
        return -1;
    }
    return file;
}

void I2C_Close(int filep)
{
    close(filep);
}

/**************************************************************
    specific to MCP 23008 and MCP23017
    and PFC8574, but no registers, only addr, data
    I2C and SPI control byte
* +-----------+---+---+---+-------+
* | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
* +-----------+---+---+---+-------+
* | 0 | 1 | 0 | 0 | A2| A1| A0| R/W
* +-----------+---+---+---+-------+
***********************************************************/
static uint8_t MCP_control_byte(uint8_t rw_cmd, uint8_t hw_addr)
{
    hw_addr = (hw_addr << 1) & 0xE;
    rw_cmd &= 1; // just 1 bit long
    return 0x40 | hw_addr | rw_cmd;
    // 0x40 == 0100 0000 A0-2 is from board address items
}

/***********************************************************/
int32_t myI2C_read_data(int file, uint8_t command)
    {
        int32_t res = i2c_smbus_read_byte_data(file, command);
        if (res < 0)
        {
            printf("Read error");
            return -1;
        }
        //printf("read is 0x%x \n", res);

//        if (read(file, buf, 1) != 1)     /***      size_t read(int fd, void *buf, size_t len) ***/
//        {
//            printf("error result: 0x%02X\n", buf[0]);
//            perror("Failed to read from the i2c bus");
//            exit(1);
//        }
        /***  success! what was read?   ***/
        //printf("after mode 1 read: 0x%02X  \n", buf[0]);
        return res;      // return the read data
    }

int32_t myI2C_read_byte(int file)
    {
        int32_t res = i2c_smbus_read_byte(file);
        if (res < 0)
        {
            printf("Read error");
            return -1;
        }


//        if (read(file, buf, 1) != 1)
//        {
//            printf("error result: 0x%02X\n", buf[0]);
//            perror("Failed to read from the i2c bus");
//            exit(1);
//        }
        /***  success! what was read?   ***/
        //printf("after read: 0x%02x  \n", buf[0]);

        return res;      // return the read data
    }

/*********************************************************/
int myI2C_write_data(int file, uint8_t command_reg, uint8_t data)
{
    /*** need control opcode byte, register address and data ***/
    int res = i2c_smbus_write_byte_data(file, command_reg, data);
    /** S Addr Wr [A] Comm [A] Data [A] P **/
    if (res<0)
    {
        printf("result i2c write error");
        return -1;
    }

    //printf("write is 0x%x \n", data);
    return 0;
}

/*************
static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command,  __u8 value)
    {
        union i2c_smbus_data data;
        data.byte = value;
        return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_BYTE_DATA, &data);
    }
static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
    {
        struct i2c_smbus_ioctl_data args;
        args.read_write = read_write;
        args.command = command;
        args.size = size;
        args.data = data;
        return ioctl(file,I2C_SMBUS,&args);
    }
*****/


/*********************************************************/
int myI2C_write_byte(int file, uint8_t data)
{
    data = data | 0x08; // can this set bkl all the time ?

    int res = i2c_smbus_write_byte(file, data);
    /** S Addr Wr [A] Data [A] P **/
    if (res<0)
    {
        printf("result i2c write error");
        return -1;
    }


    //printf("after mode 1 write: 0x%02X to register 0x%02X \n", buf[1], buf[0]);
    return 0;
}

/************************************************/


int all_on(int file)
{

    //buf[0]=ALL_LED_ON_H;    // FB
    //buf[1]= 0x10;       // 0001 0000
    //i2c_smbus_write_byte_data(file,ALL_LED_ON_H,buf[0]);
    //write(file, buf, 2);

    //buf[0]=ALL_LED_OFF_H;       //FD
    //buf[1]= 0x00;
    //i2c_smbus_write_byte_data(file,ALL_LED_OFF_H,buf[0]);
    //write(file, buf, 2);

    return 0;
}

/***
static inline s32 i2c_smbus_write_word_swapped(const struct i2c_client *client, u8 command, u16 value)
    { return i2c_smbus_write_word_data(client, command, swab16(value));

#define swab16(x) ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))
***/


/*********************
http://www.mjmwired.net/kernel/Documentation/i2c/dev-interfacehttp://www.mjmwired.net/kernel/Documentation/i2c/smbus-protocol


    __s32 i2c_smbus_read_byte(int file);
    __s32 i2c_smbus_write_byte(int file, __u8 value);
    S Addr Wr [A] Data [A] P


    __s32 i2c_smbus_read_byte_data(int file, __u8 command);
    __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value);
    S Addr Wr [A] Comm [A] Data [A] P


    __s32 i2c_smbus_read_word_data(int file, __u8 command);
    S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P
    i2c_smbus_read_word_swapped

    __s32 i2c_smbus_write_word_data(int file, __u8 command, __u16 value);
    S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P
    i2c_smbus_write_word_swapped

S     (1 bit) : Start bit
P     (1 bit) : Stop bit
Rd/Wr (1 bit) : Read/Write bit. Rd equals 1, Wr equals 0.
A, NA (1 bit) : Accept and reverse accept bit.
Addr  (7 bits): I2C 7 bit address. Note that this can be expanded as usual to get a 10 bit I2C address.
Comm  (8 bits): Command byte, a data byte which often selects a register on the device.
Data  (8 bits): A plain data byte. Sometimes, I write DataLow, DataHigh for 16 bit data.
Count (8 bits): A data byte containing the length of a block operation.

[..]: Data sent by I2C device, as opposed to data sent by the host adapter.

All these transactions return -1 on failure; you can read errno to see
what happened. The 'write' transactions return 0 on success; the
'read' transactions return the read value, except for read_block, which
returns the number of values read. The block buffers need not be longer
than 32 bytes.

do not forget byte swap
***********************/


