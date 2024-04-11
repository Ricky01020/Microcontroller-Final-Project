#define _XTAL_FREQ      8000000

#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02


#define FAN_EN_LED      PORTEbits.RE1   // LED for fan enable
#define FAN_EN          PORTAbits.RA5   // fan enable
#define FAN_PWM         PORTCbits.RC2   // Pulse Width Modulation
#define RTC_ALARM_NOT   PORTBbits.RB0   //   for the alarm function

#define RED_D3          PORTBbits.RB3   // D3
#define GREEN_D3        PORTBbits.RB4   // D3
#define BLUE_D3          PORTBbits.RB5  // D3          


#define KEY         PORTEbits.RE0                                                     
#define BUZZER      PORTCbits.RC0

#define Setup_Time_Key        	0       // 1
#define Setup_Alarm_Key       	1       // 2
#define Setup_Fan_Temp_Key    	2       // 3
#define Toggle_Fan_Key      	14      // Ok
#define Prev            		13      // left
#define Next            		15      // right
#define Minus           		16      // down
#define Plus            		12      // up
#define Ch_Plus            		14      // ok to save
#define Ch_Minus				11      // #

