#include <p18f4620.h>
#include "Fan_Support.h"
#include "stdio.h"
#include "Main.h"


 extern char duty_cycle;                        // used to save the duty cycle
 extern char FAN;                               // variable for the fan
 extern int diff;                               // difference in temperature
 extern signed int DS1621_tempF;                // the temp being read by the DS1621
 extern char fan_set_temp;                      // the wanted temp output
 extern int rpm;                                // rev per minute
 

int get_RPM()                                   // function to obtain the rpm
{
 int RPS = TMR3L / 2;                           // read the count. Since there are 2 pulses per rev
TMR3L = 0;                                      // clear out the count
return (RPS *60);                               // return RPM = 60 * RPS
} 

void Turn_On_Fan()                              // Turn_On_Fan function
{
    FAN=1;                                      // Turns on fan
    do_update_pwm(duty_cycle);                  // updates duty cycle
    FAN_EN=1;                                   // Turns on FAN_EN
    FAN_EN_LED=1;                               // Turns off FAN_EN_LED
}

void Turn_Off_Fan()                             // Turn_Off_Fan function
{
    FAN=0;                                      // Turns off fan
    FAN_EN=0;                                   // Turns off FAN_EN
    FAN_EN_LED=0;                               // Turns off FAN_EN_LED
}

void Toggle_Fan()                               // Toggle_Fan Function
{
        
     if(FAN==1)                                 // Checks to see if fan is 1
     {
        Turn_Off_Fan();                         // if fan is 1, turn on fan         
     }
     else                                       
     {
        Turn_On_Fan();                          // if fan is not 1, turn off fan
     }
    
}
void Increase_Speed()                            // Increase Speed Function
{

    if (duty_cycle==100)                         // checks if duty cycle is 100
    {
        Do_Beep();                               // Do Beep twice
        Do_Beep();
        do_update_pwm(duty_cycle);               // updates duty cycle
    }
    else 
    {
    
        duty_cycle+=5;                           // Increments duty cycle by 5
        do_update_pwm(duty_cycle);               // updates duty cycle
    }


}

void Decrease_Speed()                            // Decrease Speed Function
{
   if (duty_cycle==0)                            // Checks if fan is 0
    {
        Do_Beep();                               // Do Beep twice
        Do_Beep();
        do_update_pwm(duty_cycle);               // updates duty cycle
    }
    else 
    {
        duty_cycle=duty_cycle-5;                 // decrements duty cycle by 5
        do_update_pwm(duty_cycle);               // update duty cycle
    }
}

void do_update_pwm(char duty_cycle)              // update duty cycle function
{
float dc_f;
 int dc_I;
PR2 = 0b00000100 ;                               // set the frequency for 25 Khz
T2CON = 0b00000111 ; 
dc_f = ( 4.0 * duty_cycle / 20.0) ;              // calculate factor of duty cycle versus a 25 Khz

dc_I = (int) dc_f;                               // get the integer part
 if (dc_I > duty_cycle) dc_I++;                  // round up function
 CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
 CCPR1L = (dc_I) >> 2;
}




void Set_DC_RGB(int duty_cycle)                  // Duty Cycle RGB 
{
    int duty_cycle2= duty_cycle/10;              // divide duty cycle by 10
    if (duty_cycle2>=7)                          // checks if duty cycle 2 is greater than 7  
    {
         PORTA=(PORTA &0x20) | (7*4);            // output the color white
    }
    else 
        PORTA= (PORTA &0x20) |(duty_cycle2*4);   // cycles through colors depending on the duty cycle
                                                 // Displays colors to PORTA RGB
}

void Set_RPM_RGB(int rpm)
{
    int rmp2;                                    
    if(rpm==0)                                   // checks if rpm is 0
    {
        rmp2=0;                                  // if true, turn off led
    }
    else 
    {
        rmp2 =((rpm/500)+1);                     // divide rpm by 500 and add 1
        if(rmp2>7)                               // checks if rpm2 is greater than 7
        {
            rmp2=7;                              // if true, display white
        }
        PORTD=(PORTD & 0x70) | rmp2;             // display colors to PORTD RGB
    }
   
}

int get_duty_cycle(signed int temp, int set_temp)
{
    diff= set_temp-temp;                        // determined the level of the duty cycle
    
    if (temp>= set_temp)                        // temp can't be greater than the set temperature
    {
        duty_cycle=0;                           // fan turns off
    }   
    if (diff>50)                                // if the diff is greater than 50, set the DC to100
    {
        duty_cycle=100;
    }
    else if (diff>=25 && diff<50)               // multiply DC output by 2 if the diff in temp is between these 2 values
    {
        duty_cycle=diff*2;
    }
    else if (diff>= 10 && diff<25)              // multiply DC output by 1.5 if the diff in temp is between these 2 values
    {
        duty_cycle=diff*3/2;
    }
    else if (diff>=0 && diff<10)                // DC is equal to the diff in temperature
    {
        duty_cycle=diff;
    }
    
    return duty_cycle;
}


void Monitor_Fan()                              // sets the conditions for the fan to operate
{
    if(FAN==1)                                  // fan is on
    {
        if(DS1621_tempF < fan_set_temp)         // the set fan temperature has to be greater than the temp the DS1621 is reading
        { 
            get_duty_cycle(DS1621_tempF, fan_set_temp);                           
            do_update_pwm(duty_cycle);          // update the pwm with the calculated duty cycle
            FAN_EN_LED = 1;                     // set Fan led
            FAN_EN = 1;                         // set the fan enable
        }                   
        else                                    // if temp being read from the DS1621 is greater than the set temp
        {            
            FAN_EN = 0;                         // turn off the fan                                           
            FAN_EN_LED = 0;
        }
    }
    
}