#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Main.h"
#include "I2C_Support.h"
#include "Setup_Fan_Temp.h"
#include "Main_Screen.h"
#include "ST7735_TFT.h"
#include "utils.h"


extern char setup_fan_set_text[];                               // sets up the fan text
unsigned char setup_fan_set_temp;                               // set up fan temperature
char Select_Fan_Temp_Field;                                     // select the temp
char In_Fan_Temp_Setup;
extern char *txt;
extern char found;                                              // when a certain button is pressed
extern unsigned char fan_set_temp;

void Setup_Temp_Fan(void)                                       // setting up the fan 
{

    In_Fan_Temp_Setup = 1;
    setup_fan_set_temp = fan_set_temp;
    Initialize_Setup_Fan_Screen();                              // Initialize setup heater screen
    Update_Setup_Fan_Screen();                                  // Update the screen with latest info

    while (In_Fan_Temp_Setup == 1)
    {   
         if (check_for_button_input() == 1)                     // match is found
        {
             if (( (found == Ch_Minus) || (found == Ch_Plus) || (found == Minus) || (found == Plus)))
             {
                Do_Beep_Good(); 
                if (found == Ch_Plus) Do_Save_New_Fan_Temp();   //add code here to check if 'Ch_Plus' then use Do_Save_New_Time();  
                if (found == Ch_Minus) Exit_Setup_Fan_Temp();   // add code here to check if 'Ch_Minus' Exit_Time_Setup();   
                if (found == Minus) Decrease_Fan_Temp();        // add code here to check if 'Minus' then use Decrease_Time();
                if (found == Plus) Increase_Fan_Temp();         // add code here to check if 'Plus' then use Increase_Time();
                
                                                                // add code to handle 'Plus', 'Minus', 'Ch_Plus' and 'Ch_Minus'
                found = 0xff;
             }
             else
             {
                 Do_Beep_Bad();
             }             
        }                   
    }
}

void  Increase_Fan_Temp() // done
{
    printf ("Increase Fan Temp");
                                                                // add code to increase setup_fan_set_temp by 1
                                                                // then check if it is greater than 110. If so, limit to 110
    setup_fan_set_temp+=1;                                      // increase by 1
    
    if (setup_fan_set_temp> 120)
    {
        setup_fan_set_temp=120;
    }
    	Update_Setup_Fan_Screen();                              // Update screen with latest info

}

void  Decrease_Fan_Temp() 
{
    printf ("Decrease Fan Temp");
                                                                // add code to decrease setup_fan_set_temp by 1
                                                                // then check if it is less than 50. If so, limit to 50
      setup_fan_set_temp-=1;                                    // decrease by 1
    
    if (setup_fan_set_temp<50)
    {
        setup_fan_set_temp=50;
    }
    	Update_Setup_Fan_Screen();                              // Update screen with latest info
        
}

void Exit_Setup_Fan_Temp()
{
    printf ("Exit Fan Temp");
    DS3231_Read_Time();                                         // Read time
    Initialize_Screen();                                        // Initialize the screen before returning  
    In_Fan_Temp_Setup = 0;                                   
}

void Do_Save_New_Fan_Temp()
{
    printf ("Save Fan Temp");
    fan_set_temp = setup_fan_set_temp;
    DS3231_Read_Time();                                         // Read time
    Initialize_Screen();                                        // Initialize the screen before returning  
    In_Fan_Temp_Setup = 0;    
}
    
void Initialize_Setup_Fan_Screen(void) 
{ 
    fillScreen(ST7735_BLACK);                                   // Fills background of screen with color passed to it
 
    strcpy(txt, "ECE3301L Sp23 Final\0");                       // Text displayed 
    drawtext(start_x , start_y, txt, ST7735_WHITE  , ST7735_BLACK, TS_1); // X and Y coordinates of where the text is to be displayed

    strcpy(txt, " Fan Setup\0");                                // Text displayed 
    drawtext(start_x , start_y+25, txt, ST7735_YELLOW, ST7735_BLACK, TS_2);     
                         
    strcpy(txt, "  Set Fan Temp");
    drawtext(setup_fan_x , setup_fan_y, txt, ST7735_CYAN  , ST7735_BLACK, TS_1);
}
    
void Update_Setup_Fan_Screen(void)                              // updates the fan screen
{
    char dc_char1 = setup_fan_set_temp/100;
    char dc_char2 = (setup_fan_set_temp/10)%10;                 // add code here
    char dc_char3 = setup_fan_set_temp%10;                      // add code here 
    setup_fan_set_text[0] = dc_char1 + '0';
    setup_fan_set_text[1] = dc_char2 + '0';
    setup_fan_set_text[2] = dc_char3 + '0';
    drawtext(setup_data_fan_x, setup_data_fan_y ,setup_fan_set_text, ST7735_RED, ST7735_BLACK, TS_2);
}

