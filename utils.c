#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <p18f4620.h>
#include "utils.h"
#include "Main.h"
                                                                                        // these variables have been explained in the other .C / .h files
extern char found;
extern char Nec_code1;
extern short Nec_OK;
extern char array1[21];
extern char duty_cycle;
extern char Nec_Button;
extern int num_step;
extern float volt;
                                                                                        // these variables have been explained in the other .C / .h files
char check_for_button_input()
{       
 if (Nec_OK == 1)                                                                       // the button pressed
        {
            Nec_OK = 0;

            Enable_INT_Interrupt();                                                      // calls the interrupt
            printf ("NEC_Button = %x \r\n", Nec_Button);                                 // print the button pressed on tera tera
			
            found = 0xff;
          
                                                                                         // add code here to add FOR loop to search for match of Nec_Button with entry of array1
            for(int i = 0; i < 21; i++)                                                  // the number of buttons are placed in an array
            {
                if(array1[i] == Nec_Button)                                              // calls the array and checks if the button pressed matches 
                {
                    found = i;                                                           // found is equal to the value stores in the array   
                    i=21;
                }    
            }
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");                                     // button not found
                return (0);
            }
            else
            {
                return (1);
            }
    }
}

char bcd_2_dec (char bcd)                                                               // binary code to decimal
{
    int dec;
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)                                                                // decimal to binary code
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}

void Do_Beep()                                                                          // activate the beep function
{
    Activate_Buzzer();                                                                  // buzzer turns on then turns off after 1 second
    Wait_One_Sec();
    Deactivate_Buzzer();
    Wait_One_Sec();
    do_update_pwm(duty_cycle);                                                          // updates the current PWM
}

void Do_Beep_Good()                                                                     // beep settings for for a frequency at 2kHz
{
    Activate_Buzzer_2KHz();
    Wait_One_Sec();
    Deactivate_Buzzer();
}

void Do_Beep_Bad()                                                                      // beeps at 500Hz
{
    Activate_Buzzer_500Hz();
    Wait_One_Sec();
    Deactivate_Buzzer();
}

void Wait_One_Sec()                                                                     // 1 sec delay
{
    for(int k=0;k<0xffff;k++);
}

void Activate_Buzzer()                                                                  // function for the buzzer to activate
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_500Hz()                                                            // buzzer at 500hZ
{
    PR2 = 0b11111001;
    T2CON = 0b00000111;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_2KHz()                                                             // at 2kHz
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01111100;
    CCP2CON = 0b00111100;
}

void Activate_Buzzer_4KHz()
{
    PR2 = 0b01111100;
    T2CON = 0b00000101;
    CCPR2L = 0b00111110;
    CCP2CON = 0b00011100;
}

void Deactivate_Buzzer()                                                                // turn off the buzzer
{
    CCP2CON = 0x0;
    PORTCbits.RC1 = 0;
}

void Set_RGB_Color(char color)                                                          // turn on D3
{
    
    PORTB=(PORTB&0x07) | (color <<3);                                                   // mask the bits at PORTB

}

float read_volt()                                                                       // ADC conversion
{
 num_step= get_full_ADC();                                                              // Gather value from AN0                
 float Volt = num_step *5 / 1000.0;
 return (Volt);
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                                                                     // Start Conversion
   while(ADCON0bits.DONE==1);                                                           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                                                  // combine result of upper byte and
                                                                                        // lower byte into result
   return result;                                                                       // return the result.
}

void Init_ADC()
{
    ADCON0 = 0x05;                                                                      // Set ADCON0 to 0x01
    ADCON1= 0x0E;                                                                       // Set ADCON1 to 0x0F
     ADCON2= 0xA9;                                                                      // Set ADCON2 to 0xA9
}
