

//ECE3301L Session4
//Lab13, FINAL
//Tables 7 & 6

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>
#include "I2C.h"
#include "I2C_Support.h"
#include "interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"
#include "Main_Screen.h"
#include "Setup_Alarm_Time.h"
#include "Setup_Fan_Temp.h"
#include "Setup_Time.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF

void test_alarm();
                                                                                        // global variables
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char fan_set_temp = 75;
unsigned char Nec_state = 0;
char INT1_flag, INT2_flag;

char Nec_Button;                                                                        // the remote 
short Nec_OK=0;                                                                         
char Nec_code1;
char FAN;                                                                               // the fan we are using     
char duty_cycle;
int rps;                                                                                // rev per sec
int rpm;                                                                                // rev per min
int ALARMEN;                                                                            // for the alarm function
int alarm_mode, MATCHED,color;                                                          // more variables for the alarm
int diff;                                                                               // difference in temperature
int num_step;
float volt;                                                                             //voltage 


char buffer[31]     = " ECE3301L Sp'23 Final\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_Set_Temp_Txt[] = "075F";
char Fan_SW_Txt[]   = "OFF";                                                            // text storage for Heater Mode
char array1[17]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2,
0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x18, 0x10, 0x38, 0x5a, 0x4a};                      // add more value into this array (button characters from remote when pressed)
    
char DC_Txt[]       = "000";                                                            // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";                                                          // text storage
char RTC_ALARM_Txt[]= "0";                      
char RPM_Txt[]      = "0000";                                                           // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_set_text[]   = "075F";


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Set_ADCON0 (char ch)                                                               // setting ADCOM0    
{
    ADCON0 = ch * 4 + 1;
}
void Do_Init()                                                                          // Initialize the ports 
{ 
    init_UART();                                                                        // Initialize the uart
    Init_ADC();
    OSCCON=0x70;                                                                        // Set oscillator to 8 MHz 
    
    ADCON1=0x0E;                                                                        // Initialize ADCON1
    TRISA = 0x12;                                                                       // I/O
    TRISB = 0x07;                                                                       // I/O
    TRISC = 0x01;                                                                       // I/O
    TRISD = 0x00;                                                                       // I/O
    TRISE = 0x00;                                                                       // I/O
    PORTE = 0x00;                                                                       // I/O
    Initialize_Screen();                                                                // call the LCD screen function
    
    FAN = 0;                                                                            // turn fan off
    RBPU=0;

    Nec_OK = 0;                                                                          // Clear flag
    Nec_code1 = 0x0; 
    I2C_Init(100000); 

    DS1621_Init();                                                                      // call the DS1621 function
    Init_Interrupt();                                                                   // call the interrupt function]
    TMR3L = 0x00;                                                                       // initialize lower bits of TMR3                   
    T3CON = 0x03;                                                                       // initialize T3CON
    Turn_Off_Fan();                                                                     // turn off the fan
    fan_set_temp = 75;
    DS3231_Setup_Time();                                                                 // sets up the time
}


void main() 
{
    Do_Init();  
    DS3231_Turn_Off_Alarm();                                                            // turn off the alarm              
    DS3231_Read_Alarm_Time();                                                           // Read alarm time for the first time
    tempSecond = 0xff;                                                                  // initialize temp seconds

    while (1)                                                                           // loop
    {
        DS3231_Read_Time();                                                             // call the function

        if(tempSecond != second)
        {   
            tempSecond = second;
            DS1621_tempC = DS1621_Read_Temp();                                      
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;                                 // convert from C to F
            rpm = get_RPM();                                                            // rpm
            volt= read_volt();                                                          // store the read voltage
            Set_DC_RGB(duty_cycle);                                                     // DC determines the color output                              
            Set_RPM_RGB(rpm);                                                           // voltage determines the color output
          printf ("RTC_alarm = %d MATCHED= %d \r\n\n", RTC_ALARM_NOT, MATCHED);         // print match found on tera tera
          printf ("alarm mode = %d \r\n\n", alarm_mode);                                // print alarm mode status
            Monitor_Fan();                                                              // call the monitor fan function
            test_alarm();                                                               // call the test alarm function
            Update_Screen();                                                            // update screen with the new data
        }
        
        if (check_for_button_input() == 1)                                              //  checks if a button is pressed
        {
            KEY=1;                                                                      // sets key to 1
            Nec_OK  = 0;
            
            switch (found)                                                              // switch statement for different cases
            {
                case Setup_Time_Key:                                                    // menu to set up the time on the LCD
                    printf("Setup_Time_Key");
                    Do_Beep_Good();
                    Do_Setup_Time();
                    break;
                
                case Setup_Alarm_Key:                                                   // setting the alarm settings
                    printf("Setup_Alarm_Key");
                    Do_Beep_Good();
                    Do_Setup_Alarm_Time();
                    break;
                    
                case Setup_Fan_Temp_Key:                                                // regulate fan temperature
                    printf("Setup_Fan_Temp_Key");
                    Do_Beep_Good();
                    Setup_Temp_Fan();            
                    break;
                    
                case Toggle_Fan_Key:                                                    // turn the fan on/off
                    printf("Toggle_Fan_Key");
                    Do_Beep_Good();
                    Toggle_Fan();
                     break;           
        
                default:
                    Do_Beep_Bad();
                    break;
            }
        }    
        if (INT2_flag == 1)                                                             // clear the interrupt flag bit after the interrupt has occurred
        {
            INT2_flag = 0;
            if (ALARMEN == 0) ALARMEN = 1;                                              
            else ALARMEN = 0;
        }
        KEY=0;
    }
}




void test_alarm()                                                                       // function for the alarm 
{
    if (ALARMEN == 1 && alarm_mode != 1 )                                               // condition to turn on the alarm
    {
        DS3231_Turn_On_Alarm();                                                         
        alarm_mode = 1;
        color = 0;                                                                      
    }
    else if (alarm_mode == 1 && ALARMEN == 0)                                           // turns off the alarm
    {
        DS3231_Turn_Off_Alarm();
        alarm_mode = 0;
        color = 0;
        Deactivate_Buzzer();
        MATCHED =0;
    }
    else if (ALARMEN == 1 && alarm_mode == 1)                                           // match is found, alarm turns on and activates D3
    {
        if (RTC_ALARM_NOT == 0)
        {
            MATCHED = 1;                                                                // set match to 1 to activate the next if statement
            Activate_Buzzer_4KHz();                                                     // calls the specified buzzer frequency            
        }      
        if (MATCHED == 1)                                                                                                              
        {
            if (color > 7)                                                              // the sequence of colors loops after white has been displayed
            {
                color = 0;
            }
            if (volt > 3)                                                               // does not operate above 3V
            {
                MATCHED = 0;                                                            // clear match variable
                color = 0;                                                              // set color back to zero
                Deactivate_Buzzer();                                                    // deactivate the buzzer
                alarm_mode = 0;                                                         // alarm mode off
            }
                    color++;                                                            // increment the colors
        }
    }                            
            Set_RGB_Color(color);                                                       // display  'color'  at D3           
}

