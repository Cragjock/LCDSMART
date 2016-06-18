#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
//#include "SPI.h"
#include "lcd.h"
#include "myi2c.h"


/*** Use at /dev/spidev0.1, CS=0 (CE0#),  hw_addr = 0 ***/
/**** hw address has no meaning on the 74HC595 but kept for ease of porting ****/
static const int bus = 0, chip_select = 0, hw_addr = 0;
static int SPI_fd = 0;
static int I2C_fp = 0;

/*** current LCD state ***/
// static int curcol = 0, currow = 0;
static uint8_t cur_address = 0;
static uint8_t cur_entry_mode = 0;
static uint8_t cur_function_set = 0;
static uint8_t cur_display_control = 0;

/*** the data on the output of the latched 74x595 ***/
static uint8_t latched =0;

/************************************************************
* Adafruit 74HC595 backpack SPI Latch port looks like:
* +---------+----+----+----+--------------------+
* | 7       | 6  | 5  | 4  | 3  | 2 | 1  | 0  |
* +---------+----+----+----+--------------------+
* | bklight | db4|db5 |db6 |db7 | E | RS | N/A|
* +---------+----+----+----+--------------------+
*    NOTE: DB4-7 are in backwards order, need to flip

// ==================================================
* Adafruit backpack I2C MCP23008 port looks like:
* +---------+-----+----+----+--------------------+
* | GP7     | GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  |
* +---------+-----+----+----+--------------------+
* | bklight | db7 |db6 |db5 |db4 | E  | RS | N/A|
* +---------+-----+----+----+--------------------+
*
//=======================================
* SainSmart I2C PFC8574T port looks like:
* +-----+-----+----+----+--------------------+
* | GP7 | GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  |
* +-----+-----+----+----+--------------------+
* | db7 | db6 |db5 |db4 |BKL | E  | RW | RS |
* +---------+-----+----+----+--------------------+
*
//=======================================
*   PifaceCad SPI MCP23s17
    PORT A ====
* +-----+-----+----+----+--------------------+
* | GP7 | GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  |
* +-----+-----+----+----+--------------------+
* | SWR | SWL |SWctr |SW5 |SW4 |SW3 |SW2|SW1|
* +---------+-----+----+----+----------------+
    PORT B =====
* +-----+-----+----+----+--------------------+
* | GP7 | GP6 |GP5 |GP4 |GP3 |GP2 |GP1 |GP0  |
* +-----+-----+----+----+--------------------+
* | BKL | RS |RW |E |db7 |db6 |db5 | db4 |
* +---------+-----+----+----+----------------+
*
***********************************************************/



uint8_t get_latch(void)
{
    return latched;
}
uint8_t set_latch(uint8_t latch)
{
    latched = latch;
    return latched;

}


static void sleep_ns(long nanoseconds);
static int max(int a, int b);
static int min(int a, int b);


/*************************************************/
int lcd_open(void)
{
    if((I2C_fp=I2C_Open(1,0x27)) <0)  // 0x20 for backpack and 0x27 sainsmart LCD
        {
            return -1;

        }

// set all to low for outputs page 13 in spec
    const uint8_t ioconfig = 0x00;
    int res = myI2C_write_byte(I2C_fp, ioconfig);
    res = myI2C_read_byte(I2C_fp);
    res = myI2C_write_byte(I2C_fp, 0xff);
    res = myI2C_read_byte(I2C_fp);
    res = myI2C_write_byte(I2C_fp, 0xa0);
    res = myI2C_read_byte(I2C_fp);
    res = myI2C_write_byte(I2C_fp, 0x0a);
    res = myI2C_read_byte(I2C_fp);
    res = myI2C_write_byte(I2C_fp, 0x00);
    res = myI2C_read_byte(I2C_fp);





    lcd_init();

    return I2C_fp;
}

void lcd_close(void)
{
    lcd_clear();
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_CURSOROFF);
    lcd_backlight_off();
    close(I2C_fp);
    //close(SPI_fd);
}

/*******************************************/
void lcd_init(void)
{
    int res;

// === setup sequence per HD44780 spec, page 46
    sleep_ns(DELAY_SETUP_0_NS);     // 15ms
/***** Special function case 1 ****/
    res = myI2C_write_byte(I2C_fp, 0x30);
    lcd_pulse_enable();
    sleep_ns(DELAY_SETUP_2_NS);     // 1ms
/***** Special function case 2 ****/
    res = myI2C_write_byte(I2C_fp, 0x30);
    lcd_pulse_enable();
    sleep_ns(DELAY_SETUP_2_NS);     // 1ms
/***** Special function case 3 ****/
    res = myI2C_write_byte(I2C_fp, 0x38);
    lcd_pulse_enable();
    sleep_ns(DELAY_SETUP_2_NS);     // 1ms
/***** Initial function set for 4 bits etc ****/
    res = myI2C_write_byte(I2C_fp, 0x28);
    lcd_pulse_enable();
    sleep_ns(DELAY_SETUP_2_NS);     // 1ms

/**************** Now in 4 bit mode *****************/
    cur_function_set |= LCD_4BITMODE | LCD_2LINE | LCD_5X8DOTS;
    lcd_send_command(LCD_FUNCTIONSET | cur_function_set); // 0x28 command
    sleep_ns(DELAY_SETTLE_NS);

    cur_display_control |= LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control); // x08 command
    //res = myI2C_write_data(I2C_fp, GPIO, 0x80);

    lcd_clear();

    cur_entry_mode |= LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_send_command(LCD_ENTRYMODESET | cur_entry_mode);
    sleep_ns(DELAY_SETTLE_NS);

    cur_display_control |= LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON; //x0f
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
    sleep_ns(DELAY_SETTLE_NS);

}


/***********************************************************/
uint8_t lcd_write(const char * message)
{
    lcd_send_command(LCD_SETDDRAMADDR | cur_address);

    // for each character in the message
    while (*message) {
        if (*message == '\n')
        {
            int newrow = address2row(cur_address);
            lcd_set_cursor(0, (newrow+1));   // lcd_set_cursor(0, 1);
        }
        else
        {
            lcd_send_data(*message);
            cur_address++;
        }
        message++;
    }
    return cur_address;
}


/**************************************************************/
uint8_t lcd_set_cursor(uint8_t col, uint8_t row)
{
    col = max(0, min(col, (LCD_RAM_WIDTH / 2) - 1));
    row = max(0, min(row, LCD_MAX_LINES - 1));
    uint8_t whatever = colrow2address(col, row) ;
    lcd_set_cursor_address(whatever);
    //lcd_set_cursor_address(colrow2address(col, row));
    return cur_address;
}


/************************************************************/
void lcd_set_cursor_address(uint8_t address)
{
    //printf("cur address before mod x%x\n", address);
    cur_address = address % 103;
    //cur_address = address % LCD_RAM_WIDTH;
    //printf("cur address after mod x%x\n", cur_address);
    lcd_send_command(LCD_SETDDRAMADDR | cur_address);
}

uint8_t lcd_get_cursor_address(void)
{
    return cur_address;
}



 /*******************************************************************/
void lcd_clear(void)
{
    lcd_send_command(LCD_CLEARDISPLAY);
    sleep_ns(DELAY_CLEAR_NS);		/* 2.6 ms  - added JW 2014/06/26 */
    cur_address = 0;
}

/*******************************************************************/

void lcd_home(void)
{
    lcd_send_command(LCD_RETURNHOME);
    sleep_ns(DELAY_CLEAR_NS);		/* 2.6 ms  - added JW 2014/06/26 */
    cur_address = 0;
}


void lcd_display_on(void)
{
    cur_display_control |= LCD_DISPLAYON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}




void lcd_display_off(void)
{
    cur_display_control &= 0xff ^ LCD_DISPLAYON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}




void lcd_blink_on(void)
{
    cur_display_control |= LCD_BLINKON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}




void lcd_blink_off(void)
{
    cur_display_control &= 0xff ^ LCD_BLINKON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}

void lcd_cursor_on(void)
{
    cur_display_control |= LCD_CURSORON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}

void lcd_cursor_off(void)
{
    cur_display_control &= 0xff ^ LCD_CURSORON;
    lcd_send_command(LCD_DISPLAYCONTROL | cur_display_control);
}

void lcd_backlight_on(void)
{
    int32_t rec = myI2C_read_byte(I2C_fp);
    rec = rec | BKL_ON;
    myI2C_write_byte(I2C_fp,rec);
    //lcd_set_backlight(1);
}

void lcd_backlight_off(void)
{
    uint8_t rec = myI2C_read_byte(I2C_fp);
    rec = rec && 0xfe;
    myI2C_write_byte(I2C_fp,rec);
    //lcd_set_backlight(0);
}

void lcd_move_left(void)
{
    lcd_send_command(LCD_CURSORSHIFT | \
                               LCD_DISPLAYMOVE | \
                               LCD_MOVELEFT);
}

void lcd_move_right(void)
{
    lcd_send_command(LCD_CURSORSHIFT | \
                               LCD_DISPLAYMOVE | \
                               LCD_MOVERIGHT);
}

void lcd_left_to_right(void)
{
    cur_entry_mode |= LCD_ENTRYLEFT;
    lcd_send_command(LCD_ENTRYMODESET | cur_entry_mode);
}

void lcd_right_to_left(void)
{
    cur_entry_mode &= 0xff ^ LCD_ENTRYLEFT;
    lcd_send_command(LCD_ENTRYMODESET | cur_entry_mode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll_on(void)
{
    cur_display_control |= LCD_ENTRYSHIFTINCREMENT;
    lcd_send_command(LCD_ENTRYMODESET | cur_display_control);
}

// This will 'left justify' text from the cursor
void lcd_autoscroll_off(void)
{
    cur_display_control &= 0xff ^ LCD_ENTRYSHIFTINCREMENT;
    lcd_send_command(LCD_ENTRYMODESET | cur_display_control);
}

void lcd_write_custom_bitmap(uint8_t location)
{
    lcd_send_command(LCD_SETDDRAMADDR | cur_address);
    lcd_send_data(location);
    cur_address++;
}

void lcd_store_custom_bitmap(uint8_t location, uint8_t bitmap[])
{
    location &= 0x7; // we only have 8 locations 0-7
    lcd_send_command(LCD_SETCGRAMADDR | (location << 3));
    int i;
    for (i = 0; i < 8; i++) {
        lcd_send_data(bitmap[i]);
    }
}

void lcd_send_command(uint8_t command)
{
    lcd_set_rs(0);
    //printf("Clear RS\n");
    lcd_send_byte(command);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcd_send_data(uint8_t data)
{
    lcd_set_rs(1);
    //printf("Set RS\n");
    lcd_send_byte(data);
    sleep_ns(DELAY_SETTLE_NS);
}

void lcd_send_byte(uint8_t b)
{
    uint8_t current_state = myI2C_read_byte(I2C_fp);
    current_state &= 0x0f; // clear the data bits

    //send high nibble (0bXXXX0000)
    uint8_t new_byte = current_state | ((b & 0xf0));
    int res = myI2C_write_byte(I2C_fp, new_byte);
    //printf("high nibble 0x%x\n", new_byte);
    lcd_pulse_enable();

    //send low nibble (0b0000XXXX)
    new_byte = current_state | ((b & 0x0f)<<4);
    //printf("low nibble 0x%x\n", new_byte);
    res = myI2C_write_byte(I2C_fp, new_byte);
    lcd_pulse_enable();

}


void lcd_set_rs(uint8_t state) // state is 0 or 1
{
    int32_t rec = myI2C_read_byte(I2C_fp);
    uint8_t current_rs = rec & 0x01;

    if(state == Bit_Set)    // make RS =1
        {
            rec = rec | (1<< PIN_RS); //0x01;
        }
    else
        {
            rec &= 0xff ^ (1<< PIN_RS);
        }

//    if(state == Bit_Clear) // make RS = 0
//        {
//            if(current_rs != 0)
//                rec = rec ^ 0x01;
//        }
    myI2C_write_byte(I2C_fp,rec);
}

void lcd_set_rw(uint8_t state)
{
    //SPI_write_bit(state, PIN_RW, SPI_fd);
    int32_t rec = myI2C_read_byte(I2C_fp);
}

void lcd_set_enable(uint8_t state)
{
    //SPI_write_bit(state, PIN_E, SPI_fd);
    int32_t rec = myI2C_read_byte(I2C_fp);

    if(state == Bit_Set)
        {
            rec = rec | (1<< PIN_E);//0x04
        }
    else
        {
        rec &= 0xff ^ (1<< PIN_E);
        }
//    if(state == Bit_Clear)
//        {
//            rec = rec ^ 0x04;
//        }
    myI2C_write_byte(I2C_fp,rec);
}

void lcd_set_backlight(uint8_t state)
{
    //SPI_write_bit(state, PIN_BKL, SPI_fd);
    int32_t rec = myI2C_read_byte(I2C_fp);
}

/* pulse the enable pin */
void lcd_pulse_enable(void)
{
    lcd_set_enable(Bit_Set);
    //printf("Enable on\n");
    sleep_ns(DELAY_PULSE_NS);
    lcd_set_enable(Bit_Clear);
    //printf("Enable off\n");
    sleep_ns(DELAY_PULSE_NS);
}


uint8_t colrow2address(uint8_t col, uint8_t row)
{
    return col + ROW_OFFSETS[row];
}

uint8_t address2col(uint8_t address)
{
    if(address > 0x40)
    {
        address = address - 0x40;
    }
    return address % LCD_WIDTH;   // #define LCD_WIDTH 20
    //return address % ROW_OFFSETS[1];
}

uint8_t address2row(uint8_t address)
{
    int row;
    int i =0;
    int value;
    if(address > ROW_OFFSETS[3])    // greater than 0x54
    {
        return row=3;
    }
    for(i=0; i<4; i++)
    {
        value = cur_address - ROW_OFFSETS[i];
        if((value < 19) && (value >= 0) )
        {
            row = i;
            return row;
        }
    }
    address > ROW_OFFSETS[1] ? 1 : 0;
    return row;
    //return address > ROW_OFFSETS[1] ? 1 : 0;
}

static void sleep_ns(long nanoseconds)
{
    struct timespec time0, time1;
    time0.tv_sec = 0;
    time0.tv_nsec = nanoseconds;
    nanosleep(&time0 , &time1);
    return 0;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}

static int min(int a, int b)
{
    return a < b ? a : b;
}


uint8_t flip(uint8_t data)
{
    char flip = data;  //   0b01100110; // starting data in
    char mirror=   0b00000000; // flipped data
    char mask =    0b00000001;

    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));;
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));
    flip = flip >> 1;
    mirror = ((mirror<<1) + (flip & mask));

    return mirror;

}
