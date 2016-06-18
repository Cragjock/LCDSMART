#include <stdio.h>
#include <stdlib.h>


#include "lcd.h"
//#include "SPI.h"
#include "pitime.h"
#include "myi2c.h"

#define XXXBM 0x08
#define LeftBM 0x01
#define MiddleBM 0x02
#define RightBM 0x03
#define SatLeftBM 0x04
#define SatRightBM 0x05
#define HandBM 0x06
#define CheckBM 0x07

#define full 0x01
#define half_fill_upper 0x02
#define half_fill_lower 0x03
#define full_L_upper 0x04
//#define full_L_lower
#define half_L_lower 0x05
#define half_L_upper 0x06
//#define full_R_upper
#define half_R_lower 0x07
#define half_R_upper 0x08

char upper[16] = {"Az: "};
char lower[16] = {"El: "};
char merge[3]= {"456"};
const char enter[] = {"Enter to continue"};
const char count_msg[] = "PWM count: ";
uint8_t sdceng[] ={0x53,0x74,0x65,0x76,0x65,0x6e};
const char sdcjp[]={0xbd,0xc3,0xa8,0xb0,0xcc,0xde,0xdd};
const char alpha[] ={"123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h"};

const char *menu_select[] = {"go", "stop", "Linda", "Cabernet", "Chardonney"};
char buf[80];
void setbmp(void);

// NIST
char NL = '\n';
char EL = '\0';
int k=0;
char str_filenm[33];

int (*device_open)(void);
//char* (*alt_pitime)();
int (*alt_pitime)(char*);

//char myTest[] = "E57523 01-01-15 00:58:16 50 0 0 257.3 UTC(NIST) *";


int main()
{
    printf("Hello world!\n");

//  adding NIST read time code ========
    char myTime[80]={0};
    char *mysplit[10];
    //char *myTest;
    char *myTest=malloc(sizeof(char) * 80);
    char *pch;
    //char *pch = malloc(sizeof(char) * 80);
    int sizecheck;

/*** function pointer ***/
    //sizecheck = buf_pitime(myTime);

    //alt_pitime= &buf_pitime;
    alt_pitime= &no_net_pitime;

    sizecheck = alt_pitime(myTime);

/*****
57523 16-05-15 00:58:16 50 0 0 257.3 UTC(NIST) *
len = 51
*****/

    //pch = strtok (myTest," ");
    pch = strtok (myTime," ");
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        mysplit[k]= (char*)malloc(strlen(pch+1));
        strcpy(mysplit[k],pch);
        pch = strtok (NULL, " ");
        k++;
    }
    // format for LCD
    strcat(str_filenm,mysplit[7]);      // UTC(NIST)
    strcat(str_filenm,"\n");
    strcat(str_filenm,mysplit[2]);      // Time
    strcat(str_filenm,"\n");
    strcat(str_filenm,mysplit[1]);      // date
    printf("strcat %s\n",str_filenm);

    char* mytest1 = myTest+15;
    *mytest1=NL;
    char* mytest3=myTest+7;

    char* mytest4= myTest+24;
    *mytest4=EL;
    myTest[0]=0x20;     // clear the new line from NIST return
// ==== NIST end

    device_open=&lcd_open;

    int I2C_fp= device_open();

    //int I2C_fp = lcd_open();

    //myI2C_write_byte(I2C_fp, 0x08);
    //myI2C_write_byte(I2C_fp, 0x00);
    //myI2C_write_byte(I2C_fp, 0x08);

    setbmp();
    lcd_clear();

    lcd_set_cursor_address(0x40);
    //myI2C_write_byte(I2C_fp, 0x08);
    lcd_write(str_filenm);   //NIST
    //myI2C_write_byte(I2C_fp, 0x08);

    lcd_write("ISS Azimuth:127\nISS Elevation:36.98");

    lcd_backlight_off();
    lcd_backlight_on();

    lcd_clear();
    sprintf(buf, "sat %c%c all: %x\n",SatLeftBM, SatRightBM);
    lcd_write(buf);

    lcd_clear();

    //lcd_set_cursor(uint8_t col, uint8_t row)
    //lcd_set_cursor(1,2);
    lcd_write("Push a button 5=exit\n");
    //lcd_clear();


    lcd_set_cursor_address(0x00);
    lcd_write(sdcjp);
    lcd_set_cursor_address(0x14);
    lcd_write(sdceng);

    lcd_write(enter);   // this wraps cur address over 27 !!!
    lcd_clear();
    lcd_write(alpha);

    //buf[1] = getchar();

    lcd_clear();
    lcd_set_cursor_address(0x40);
    //lcd_set_cursor(8,2);// col row

    sprintf(buf, "%c====== %c =======%c",LeftBM, MiddleBM,RightBM);
    lcd_write(buf);

/*********** walking satellite bitmap  ********************/
    int ddcount=0;
    for(ddcount=0; ddcount<40; ddcount++) // oops, need to account for the LCD address gap
    {
        lcd_send_command(LCD_SETDDRAMADDR+ddcount); // direct to DDRAM location 80+i on display. 0,0
        lcd_send_data(SatLeftBM);
        lcd_send_command(LCD_SETDDRAMADDR+ddcount+1);
        lcd_send_data(SatRightBM);

        //sleep(1);
        usleep(250000); // 0.5 seconds use 500000
        lcd_send_command(0x80+ddcount);
        lcd_send_data(LCD_SPACE);              // blank space to overwrite sat bmp
    }
    lcd_send_command(0x8e);       // direct to DDRAM location 8e, location 15 on display
    //lcd_send_data(0x02);          // check mark bitmap
    lcd_send_data(XXXBM);
    //lcd_send_command(0x8f);       // location 16 on display
    //lcd_send_data(0x07);          // X bitmap
    lcd_send_data(CheckBM);


    strcat(upper, merge); // Az: 456
    lcd_write(upper);
    lcd_set_cursor_address(ROW_OFFSETS[1]);
    sprintf(buf, "= %c %i %d =",0x7e,213,75);
    strcat(lower, buf); // El:
    lcd_write(lower);
    lcd_set_cursor_address(0x1e);


    lcd_close();
    return 0;
}



void setbmp(void)
{
/******** BITMAP SET UP ************************/
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYOFF | LCD_CURSOROFF | LCD_BLINKOFF); // x08 command
    uint8_t bmLeft[]= {8,12,10,9,10,12,8,0};
    uint8_t bmMiddle[]={0,0,31,14,4,14,31,0};
    uint8_t bmRight[]={2,6,10,18,10,6,2,0};
    uint8_t bmSatLeft[]={0,20,21,21,31,21,20,20};
    uint8_t bmSatright[]= {0,5,21,21,31,21,5,5};
    uint8_t bmhand[]= {4,14,30,31,31,31,14,14};
    uint8_t bmCheck[] = {0,1,3,22,28,8,0,0};
    uint8_t bmXXX[] = {0,27,14,4,12,27,0,0};

/****
    uint8_t filled[] = {31,31,31,31,31,31,31,31};
    uint8_t upper_half_filled[] = {31,31,31,31,0,0,0,0};
    uint8_t lower_half_filled[] = {0,0,0,0,31,31,31,31};
    uint8_t upperright[] = {24,28,30,31,31,31,31,0};
    uint8_t lowerright[] = {31,31,31,31,31,30,28,24};
    uint8_t half_upper_right[] = {31,30,28,24,0,0,0,0};
    uint8_t half_lower_right[] = {0,0,0,0,31,30,28,24};
    uint8_t upperleft[] = {3,7,15,31,31,31,31,0};
    uint8_t lowerleft[] = {31,31,31,31,15,7,3,0};
    uint8_t half_upper_left[] = {31,15,7,3,0,0,0,0};
    uint8_t half_lower_left[] = {0,0,0,0,31,31,15,7};

    lcd_store_custom_bitmap(1, filled); // store
    lcd_store_custom_bitmap(2, upper_half_filled); // store
    lcd_store_custom_bitmap(3, lower_half_filled); // store ok
    lcd_store_custom_bitmap(4, upperleft); // store ok
    lcd_store_custom_bitmap(5, half_lower_left); // store ok
    lcd_store_custom_bitmap(6, half_upper_left); // store ok
    lcd_store_custom_bitmap(7, half_lower_right); // store ok
    lcd_store_custom_bitmap(8, half_upper_right); // store ok

//#define full 0x01                   // need
//#define half_fill_upper 0x02        // need
//#define half_fill_lower 0x03        // need
//#define full_L_upper 0x04           // need
//#define full_L_lower
//#define half_L_lower 0x05             // need
//#define half_L_upper 0x06
//#define full_R_upper
//#define half_R_lower 0x07               // need
//#define half_R_upper 0x08


    lcd_set_cursor(1,0);// col row
    lcd_write_custom_bitmap(full_L_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(full);

    lcd_set_cursor(1,1);// col row
    lcd_write_custom_bitmap(full);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(half_fill_lower);

    lcd_set_cursor(1,2);// col row
    lcd_write_custom_bitmap(half_L_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(half_fill_upper);
    lcd_write_custom_bitmap(full);

    lcd_set_cursor(1,3);// col row
    lcd_write_custom_bitmap(half_L_lower);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(half_fill_lower);
    lcd_write_custom_bitmap(full);
****/

    lcd_store_custom_bitmap(1, bmLeft); // store
    lcd_store_custom_bitmap(2, bmMiddle); // store
    lcd_store_custom_bitmap(3, bmRight); // store
    lcd_store_custom_bitmap(4, bmSatLeft); // store
    lcd_store_custom_bitmap(5, bmSatright); // store
    lcd_store_custom_bitmap(6, bmhand); // store
    lcd_store_custom_bitmap(7, bmCheck); // store
    lcd_store_custom_bitmap(0, bmXXX); // store

    lcd_send_command(LCD_RETURNHOME);
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON);

}


