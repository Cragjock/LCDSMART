
#ifndef MYLCD_H
#define MYLCD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DELAY_PULSE_NS 1000 // 1us
#define DELAY_SETTLE_NS 40000 // 40us
#define DELAY_CLEAR_NS 2600000L // 2.6ms
#define DELAY_SETUP_0_NS 15000000L // 15ms
#define DELAY_SETUP_1_NS 5000000L // 5ms
#define DELAY_SETUP_2_NS 1000000L // 1ms


/****
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D7 3        // output 3
#define PIN_D6 4        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 6        // output 6
#define PIN_BKL 7       // output 7
******/

// Adafruit backpack I2C
#ifdef MCP23008
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D7 6        // output 3
#define PIN_D6 5        // output 4
#define PIN_D5 4        // output 5
#define PIN_D4 3        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0x87
#endif // MCP23008

#ifdef ADA595           // Adafruit backpack SPI
#define PIN_RW 0        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 1        // output 1
#define PIN_E 2         // output 2
#define PIN_D7 3        // output 3
#define PIN_D6 4        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 6        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0x87
#endif // ADA595

#ifdef PFC8574T         // Saimsmart 20x4
#define PIN_RW 1        // output 0, no connection on 595, grounded on LCD input
#define PIN_RS 0        // output 1
#define PIN_E 2        // output 2
#define PIN_D7 7        // output 3
#define PIN_D6 6        // output 4
#define PIN_D5 5        // output 5
#define PIN_D4 4        // output 6
#define PIN_BKL 7       // output 7
#define DATA_MASK 0x0f
#endif // PFC8574T

#define Bit_Set 1
#define Bit_Clear 0

#define RS_on 0x01
#define RS_off 0x00
#define E_on 0x04
#define E_off 0x00

// LCD commands
#define LCD_CLEARDISPLAY 0x01       // RS = 0
#define LCD_RETURNHOME 0x02         // RS = 0
#define LCD_ENTRYMODESET 0x04       // RS = 0
#define LCD_DISPLAYCONTROL 0x08     // RS = 0
#define LCD_CURSORSHIFT 0x10        // RS = 0
#define LCD_FUNCTIONSET 0x20        // RS = 0
#define LCD_SETCGRAMADDR 0x40       // RS = 0
#define LCD_SETDDRAMADDR 0x80       // RS = 0

//#define LCD_WRITECGDDRAM            // RS = 1, no specific command data
#define LCD_NEWLINE 0xC0
#define LCD_SPACE 0x20

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// not a specific command
#define BKL_ON 0x08
#define BKL_OFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5X10DOTS 0x04
#define LCD_5X8DOTS 0x00

//#define LCD_MAX_LINES 2
//#define LCD_WIDTH 16

#define LCD_MAX_LINES 4
#define LCD_WIDTH 20
#define LCD_RAM_WIDTH 80 // RAM is 80 wide

static const uint8_t ROW_OFFSETS[] = {0, 0x40, 0x14, 0x54};

/************************************************************
* LCD DDRAM space (hex address) and row-column
*     1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
*    +---------+--------+--------+----------+--------------------+
* 1  |00|01|02|03|04|05|06|07|08|09|0A|0B|0C|0D|0E|0F|10|11|12|13|
*    +---------+--------+--------+----------+--------------------+
* 2  |40|41|42|43|44|45|46|47|48|49|4A|4B|4C|4D|4E|4F|50|51|52|53|
*    +---------+--------+--------+----------+--------------------+
* 3  |14|15|16|17|18|19|1A|1B|1C|1D|1E|1F|20|21|22|23|24|25|26|27|
*    +---------+--------+--------+----------+--------------------+
* 4  |54|55|56|57|58|59|5A|5B|5C|5D|5E|5F|60|61|62|63|64|65|67|68|
*    +---------+--------+--------+----------+--------------------+


*     1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20
*    +---------+--------+--------+----------+--------------------+
* 1  |00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|
*    +---------+--------+--------+----------+--------------------+
* 2  |64|65|66|67|68|69|70|71|72|73|74|75|76|77|78|79|80|81|82|83|
*    +---------+--------+--------+----------+--------------------+
* 3  |20|21|22|23|24|25|26|27|28|29|30|31|32|33|34|35|36|37|38|39|
*    +---------+--------+--------+----------+--------------------+
* 4  |84|85|86|87|88|89|90|91|92|93|94|95|96|97|98|99|100|101|102|103|
*    +---------+--------+--------+----------+--------------------+

*********/

int lcd_open(void); /** lcd_init call within this ***/
void lcd_close(void);
void lcd_init(void);



/*** Writes a message to the LCD screen starting from the current cursor **/
uint8_t lcd_write(const char * message);

/*** Sets the cursor position on the screen (col , row). Example:lcd_set_cursor(5, 1); **/
uint8_t lcd_set_cursor(uint8_t col, uint8_t row);

/*** Sets the cursor position on the screen (address = col + row*40).Example:lcd_set_cursor(41); // col=1, row=40 **/
void lcd_set_cursor_address(uint8_t address);

/*** Returns the cursor position (address).Example:uint8_t cursor_addr = pifacecad_lcd_get_cursor_address() **/
uint8_t lcd_get_cursor_address(void);

/*** Clears the screen and returns the cursor to home (0, 0) **/
void lcd_clear(void);

/*** Returns the cursor to home (0, 0)**/
void lcd_home(void);

/** Turns the display on. **/
void lcd_display_on(void);

/**  Turns the display off.**/
void pifacecad_lcd_display_off(void);

/** Turns the blink on **/
void lcd_blink_on(void);

/** Turns the blink off.**/
void lcd_blink_off(void);

/*** Turns the cursor on.**/
void lcd_cursor_on(void);

/** Turns the cursor off **/
void lcd_cursor_off(void);

/*** Turns the backlight on.**/
void lcd_backlight_on(void);

/** Turns the backlight off. **/
void lcd_backlight_off(void);

/*** Moves the display left.**/
void lcd_move_left(void);

/** Moves the display right.**/
void lcd_move_right(void);

/** The cursor will move to the right after printing causing text to read left to right.**/
void lcd_left_to_right(void);

/** The cursor will move to the left after printing causing text to read right to left.**/
void lcd_right_to_left(void);

/** The screen will follow text if it moves out of view.**/
void lcd_autoscroll_on(void);

/** The screen will not follow text if it moves out of view. **/
void lcd_autoscroll_off(void);

/*** Writes the custom bitmap stored at the specified bank location to the display
 * Example:lcd_write_custom_bitmap(0);**/
void lcd_write_custom_bitmap(uint8_t location);

/*** Stores a custom bitmap to the location specified (up to 8: 0-7).
 * Example:
 *     uint8_t bitmap[] = {0x15, 0xa, 0x15, 0xa, 0x15, 0xa, 0x15, 0xa};
 *     lcd_store_custom_bitmap(0, bitmap); // store
 *     lcd_write_custom_bitmap(0); // write */
void lcd_store_custom_bitmap(uint8_t location, uint8_t bitmap[]);

/** Set the backlight pin on the HD44780 **/
void lcd_set_backlight(uint8_t state);




/**
 * Returns an address calculated from a column and a row.
 * Example:uint8_t address = colrow2address(5, 1) **/
uint8_t colrow2address(uint8_t col, uint8_t row);
/** Returns a column calculated from an address.Example:uint8_t col = address2col(42); // col=2 **/
uint8_t address2col(uint8_t address);
/*** Returns a row calculated from an address. Example: uint8_t row = address2col(42); // row=1 **/
uint8_t address2row(uint8_t address);
uint8_t get_latch(void);
uint8_t set_latch(uint8_t latch);
/** Pulse the enable pin on the HD44780 ***/
void lcd_pulse_enable(void);
/*** Send a command to the HD44780 Example:lcd_send_command(LCD_CLEARDISPLAY) **/
void lcd_send_command(uint8_t command);
/*** Send data to the HD44780. Example lcd_send_data('a') **/
void lcd_send_data(uint8_t data);
/** Send a byte to the HD44780 Example: lcd_send_byte(0xaa); **/
void lcd_send_byte(uint8_t byte);
/*** Set the RS pin on the HD44780 lcd_set_rs(1) **/
void lcd_set_rs(uint8_t state);
/*** Set the enable pin on the HD44780 **/
void lcd_set_enable(uint8_t state);
uint8_t flip(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
