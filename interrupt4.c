//Tables 6 & 7
// ECE3301L.04
//Lab13
#include <xc.h>
#include <p18f4620.h>
#include "interrupt.h"

unsigned char bit_count;                                            // global bit count variable 
unsigned int Time_Elapsed;                                          // counts the time passed

unsigned long long Nec_code;                                        //
unsigned char Nec_State = 0;                                        // 

extern char Nec_Button;                                             // the button linked to it
extern short Nec_OK;

extern char INT1_flag;
extern char INT2_flag;

void Init_Interrupt()
{
    INTCON3bits.INT1IF = 0;                                         // Clears the external interrupt1 flag
    INTCON2bits.INTEDG1 = 0;// falling edge
    INTCON3bits.INT1IE = 1;                                         // Enable external interrupt INT1IE
    
    INTCON3bits.INT2IF =0; // Clear INT2IF
    INTCON2bits.INTEDG2=1; // INT2 EDGE rising
    INTCON3bits.INT2IE=1; // Set INT2 IE
    
    TMR1H = 0;                                                      // Reset Timer1
    TMR1L = 0;                                                      // high bit must be initialized first 
    PIR1bits.TMR1IF = 0;                                            // Clear interrupt flag
    PIE1bits.TMR1IE = 1;                                            // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                                            // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                                             // Enable global interrupts
}

void Enable_INT_Interrupt()
{
    INTCON3bits.INT1IE = 1;                                         // Enable external interrupt
    INTCON2bits.INTEDG1 = 0;                                        // falling edge
}    

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCON3bits.INT1IF == 1) INT1_isr();
    if (INTCON3bits.INT2IF == 1) INT2_isr();
}


void TIMER1_isr(void)
{
    Nec_State = 0;                                                  // Reset decoding process
    INTCON2bits.INTEDG1 = 0;                                        // Edge programming for INT 1 falling edge
    T1CONbits.TMR1ON = 0;                                           // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                                            // Clear interrupt flag
}

void Reset_Nec_State()                                              // resets the present state of the "Nec_State" variable
{
    Nec_State=0;
    T1CONbits.TMR1ON = 0;
}

void INT1_isr(void)
{
    INTCON3bits.INT1IF = 0;
    if (Nec_State != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;                        // Store Timer1 value
        TMR1H = 0;                                                  // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_State)
    {
        case 0 :
        {
                                                                    // Clear Timer 1
            TMR1H = 0;                                              // Reset Timer1
            TMR1L = 0;                                              //
            PIR1bits.TMR1IF = 0;                                    //
            T1CON= 0x90;                                            // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;                                   // Enable Timer 1
            bit_count = 0;                                          // Force bit count (bit_count) to 0
            Nec_code = 0;                                           // Set Nec_code = 0
            Nec_State = 1;                                          // Set Nec_State to state 1
            INTCON2bits.INTEDG1 = 1;                                // Change Edge interrupt of INT 1 to Low to High            
            return;
        }
        
        case 1 :
        {
            if((Time_Elapsed >= 8500) && (Time_Elapsed <=9500))     // execute if time elapsed is within this range
            {
                Nec_State =2;                                       // occurs if the variable is equal to 2
            }
            else Reset_Nec_State();                                 // if it doesnt occur call the reset function 
             INTCON2bits.INTEDG1=0;                                 // falling edge INTCON2
             return;                                                // returns to main program
        }
        
        case 2 :                                                    // Add your code here
        {
            if((Time_Elapsed >=4000) && (Time_Elapsed <=5000))      // occurs in between 4000 & 5000
            {
                Nec_State=3;                                        // sets it to 3 
            }
            else Reset_Nec_State();                                 // reset function is called
            INTCON2bits.INTEDG1=1;                                  // rising edge set to 1
            return;
        }
        
        case 3 :                                                    // between 400 & 700
        {
            if((Time_Elapsed >=400) && (Time_Elapsed <=700))
            {
                Nec_State=4;                                        // sets it to 4
            }
            else Reset_Nec_State();                                 // calls the reset function 
            INTCON2bits.INTEDG1=0;                                  // falling edge
            return;
        }
        
        case 4 :                                                    // same as the previous instruction cycle 
        {
            if((Time_Elapsed >=400) && (Time_Elapsed <=1800))       // same as the previous instruction cycle 
            {
                Nec_code=Nec_code<<1;                               // shifts the bit by 1 to the left
                
                if(Time_Elapsed >=1000)                             // occurs if time elapsed is greater than 1k
                Nec_code+=1;                                        // Nec_code +1         
                bit_count+=1;                                       // adds 1 to the bit count
                
                if(bit_count>31)                                    // occurs at values larger than 31
                {
                    Nec_OK=1;                                       // the button on the remote has been detected 
                    INTCON3bits.INT1IE=0;                           // disables the interrupt 
                    Nec_Button = (char)(Nec_code>>8);               // right shifts the bits 
                    Nec_State=0;                                    // sets the nec state to 0
                }
                else Nec_State =3;                                  
            }
            else Reset_Nec_State();                                 // calls the reset function
            
            INTCON2bits.INTEDG1=1;                                  // rising edge 

            return;                                                 // returns control to main
        }

    }
}

void INT2_isr()                                                     // interrupt to activate the button for the alarm
{
INTCON3bits.INT2IF=0;                                               // Clear the interrupt flag
INT2_flag = 1;                                                      // set software INT2_flag

}