int get_RPM();                              // gets the RPM
void Toggle_Fan();                          // fan on/off
void Turn_Off_Fan();                        
void Turn_On_Fan();
void Increase_Speed();
void Decrease_Speed();
void do_update_pwm(char) ;                  // new pwm value
void Set_DC_RGB(int);                       // D3
void Set_RPM_RGB(int);

#define FAN_EN			PORTAbits.RA5        // Defines the port for FAN_EN
#define FAN_EN_LED		PORTEbits.RE1        // Defines the port for FAN_EN_LED
#define FAN_PWM			PORTCbits.RC2        // Defines the port for FAN_PWM




