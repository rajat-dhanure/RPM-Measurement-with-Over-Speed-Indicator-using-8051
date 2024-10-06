/*
PROJECT TOPIC: Indicator of Digital RPM for Electrical Motor using Alarm Indicator with Over Speed
*/

#include<reg52.h>
#define lcd_data_port P3

sbit rs=P2^0;  // RS pin of LCD
sbit rw=P2^1;  // RW pin of LCD
sbit en=P2^2;  // EN pin of LCD
sbit ir=P1^0;  // IR sensor input
sbit buz=P0^2; // Buzzer output

void delay(unsigned int count)  
{
    unsigned int k;
    for (k = 0; k < count; k++) 
    {
        // Timer 0 initialization for delay
			
        TMOD = 0X01;
        TH0 = 0XFC;
        TL0 = 0X65;
        TR0 = 1;
        while (!TF0); // Wait until timer overflows
        TF0 = 0;      // Clear Timer 0 overflow flag
        TR0 = 0;      // Stop Timer 0
   }
}
     
void LCD_Command (unsigned char cmd)  
{
    lcd_data_port= cmd; // Send command to LCD
    rs=0;               // RS low (command mode)
    rw=0;               // RW low (write mode)
    en=1;               // EN high (enable)
    delay(1);           // Delay for EN pulse width
    en=0;               // EN low (disable)
    delay(5);           // Delay for processing time of LCD command
}
     
void LCD_Char (unsigned char char_data)  
{
    lcd_data_port=char_data; // Send data to LCD
    rs=1;                    // RS high (data mode)
    rw=0;                    // RW low (write mode)
    en=1;                    // EN high (enable)
    delay(1);                // Delay for EN pulse width
    en=0;                    // EN low (disable)
    delay(5);                // Delay for processing time of LCD command
}

void lcd_int(unsigned int i)     
{   
    unsigned char c;
    int num[10];
    int p;
    int k=0;
    while(i>0) 							// Extract digits of the number
    {
        num[k]=i%10;
        i=i/10;
        k++;
    }
    k--;
    for (p=k;p>=0;p--) 			// Display each digit on LCD
    {
        c=num[p]+48;
        lcd_data_port = c;
        rw = 0;
        rs = 1;
        en = 1;
        delay(1);
        en = 0;
    }   
    return;
}
     
void LCD_String (unsigned char *str) 
{
    int i;
    for(i=0;str[i]!=0;i++)  // Display string on LCD
    {
        LCD_Char (str[i]);  
    }
}
  
void LCD_Init (void) 
{
    delay(20);       				// Delay for LCD initialization
    LCD_Command (0x38); 		// Initialize LCD in 2 lines, 5x7 matrix
    LCD_Command (0x0C);		 	// Display ON, Cursor OFF
    LCD_Command (0x06);		 	// Auto-increment cursor position
    LCD_Command (0x01); 		// Clear display screen
    LCD_Command (0x80); 		// Set cursor position to first line, first column
}

int count()
{
    unsigned int count=0;
    unsigned int k;
    for (k = 0; k < 1000; k++) { 		// Loop for 1000 milliseconds (1 second)
        TMOD = 0X01;       					// Timer 0 mode 1: 16-bit timer with auto-reload
        TH0 = 0XFC;        					// Set Timer 0 high byte for 1 ms delay
        TL0 = 0X65;        					// Set Timer 0 low byte for 1 ms delay
        TR0 = 1;           					// Start Timer 0
        while (!TF0){      					// Wait until Timer 0 overflows
            if(!ir) {      					// If IR sensor input is low (falling edge detected)
                count++;   					// Increment count
                while(!ir);					// Wait until IR sensor input becomes high again
            }
        }
        TF0 = 0;           					// Clear Timer 0 overflow flag
        TR0 = 0;           					// Stop Timer 0
    }
    return count;         					// Return count of falling edges
}
   
void main()
{
    LCD_Init();            					// Initialize LCD
    LCD_Command(0x0C);    					// Turn off cursor
    delay(20);             					// Delay for LCD initialization
    LCD_String("____WELCOME____");  
    delay(1000);           					// Delay for 1 second
    LCD_Command(0xc0);    					// Move cursor to second line
    LCD_String("_GAWANDE_SIR_TO_"); 
    delay(1000);           					// Delay for 1 second
    LCD_Command(0x01);    					// Clear display screen
    LCD_Command(0x0c);    					// Turn off cursor
    LCD_String("_RPM_MEASUREMENT_");
    delay(1000);           					// Delay for 1 second
    LCD_Command(0xc0);    					// Move cursor to second line
    LCD_String("___GROUP_C_18___");   
    delay(1000);           					// Delay for 1 second
    while(1)
    {
        int COUNT=count(); 					// Get count of falling edges
        int RPM=(COUNT*60)/4; 				// Convert count to RPM (revolutions per minute)
        LCD_Command(0x01); 					// Clear display screen
        LCD_Command(0x0c); 					// Turn off cursor
        LCD_String("RPM :"); 				// Display RPM label
        lcd_int(RPM);        				// Display RPM value
        LCD_Command(0xC0);  				// Move cursor to second line
        if(RPM > 1000){
            LCD_String("OVER SPEED!"); // Display over speed message
            buz = 0; 									// Activate buzzer
        }
        else{
            buz = 1; 									// Deactivate buzzer
        }
        delay(250); 									// Delay for 250 milliseconds
    }
}
