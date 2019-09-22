/**
 * @file main.c
 * @brief Reflex testing
 *
 * The programs is intended for testing out the reflexes. One of the
 * four light-emitting diodes (LEDs) is randomly turned on and if the adequate
 * button is pressed while LED on the number of points is increased by three.
 * If a non-corresponding button is pressed, or none at all while LED on, the
 * number of points is decreased by one. In case a button is pressed while LED
 * off, the number of points is also decreased by one. The current number of
 * points is shown on the LCD.
 *
 * @date 2019
 * @author Sanja Milovanovic
 */

#include <msp430.h> 
#include <stdlib.h>
#include <stdint.h>

/** LCD port output macro */
#define LCD_PORT        P8OUT
/** LCD port direction macro */
#define LCD_PORT_DIR    P8DIR
/** LED port output macro */
#define LED_PORT        P4OUT
/** LED port direction macro */
#define LED_PORT_DIR    P4DIR
/** button port direction macro */
#define BUTTON_PORT_DIR P2DIR
/** button edge select macro */
#define BUTTON_EDGE_SEL P2IES
/** button flag macro */
#define BUTTON_FLAG     P2IFG
/** button interrupt enable macro */
#define BUTTON_INT_EN   P2IE
/** LCD enable macro */
#define LCD_EN            BIT3
/** LCD register select macro */
#define LCD_RS            BIT2
/** mask upper four bits macro */
#define MASK_UPPER        0xF0
/** mask all eight bits macro */
#define MASK_ALL          0xff
/** 4 bit mode 5x7 font LCD macro */
#define MODE_FONT         0x28
/** no cursor no blink LCD macro */
#define CURS_BLINK        0x0C
/** automatic increment no display shift LCD macro */
#define SHIFT             0x06
/** address DDRAM with 0 offset 80h LCD macro */
#define DDRAM             0x80
/** clear LCD screen macro */
#define CLEAR_SCREEN      0x01
/** ASCII constant for conversion to ASCII macro */
#define ASCII_CONV        0x30

/** current number of points */
volatile int8_t Points = 0;
/** pseudo-random number */
volatile uint8_t Random_number;
/** adequate pseudo-random number for LED port */
volatile uint8_t Random_numberLED;
/** button pressed once while LED on */
volatile uint8_t First_pressed;
/** button pressed while LED on */
volatile int8_t Pressed;
/** character used for display */
char Points1;

/**
 * @brief LCD command
 *
 * Inserting a command into the LCD module
 *
 * @param c Character representing a LCD module command
 */
void lcd_command (char c)
{
    // Upper mask of a command
    P8OUT = (c & MASK_UPPER) | LCD_EN;
    P8OUT = (c & MASK_UPPER);

    // Lower mask of a command
    P8OUT = ((c << 4) & MASK_UPPER) | LCD_EN;
    P8OUT = ((c << 4) & MASK_UPPER);

    __delay_cycles(2000);
}

/**
 * @brief Clear LCD screen
 */
void lcd_clear()
{
    lcd_command(DDRAM);             // address DDRAM with 0 offset 80h
    lcd_command(CLEAR_SCREEN);      // clear LCD screen
}

/**
 * @brief LCD initialization
 *
 * Initialize the LCD module in the wanted mode
 */
void lcd_initialization()
{
    LCD_PORT_DIR = MASK_ALL;        // set all LCD pins to output mode
    lcd_command(MODE_FONT);         // 4 bit mode 5x7 font
    lcd_command(CURS_BLINK);        // no cursor no blink
    lcd_command(SHIFT);             // automatic increment no display shift
    lcd_clear();                    // clear LCD
}

/**
 * @brief Character display
 *
 * Display one character on the LCD screen
 *
 * @param data Character displayed
 */
void lcd_data(unsigned char data)
{
    // Upper mask data
    LCD_PORT = ((data & MASK_UPPER) | LCD_EN | LCD_RS);
    LCD_PORT = ((data & MASK_UPPER) | LCD_RS);

    // Lower mask data
    LCD_PORT = (((data << 4) & MASK_UPPER) | LCD_EN | LCD_RS);
    LCD_PORT = (((data << 4) & MASK_UPPER) | LCD_RS);

    __delay_cycles(4000);
}

/**
 * @brief Display points
 *
 * Displays the number of points on the LCD screen
 */
void lcd_display()
{
    lcd_clear();                               // clear LCD screen

    // Check if number of points negative
    if (Points < 0){
        lcd_data('-');                         // display minus sign
    }

    // Check if number of points in single-digit format
    if (abs(Points) < 10){
        Points1=abs(Points) + ASCII_CONV;      // convert digit to ASCII code
        lcd_data(Points1);                     // display digit
    } else {
        Points1=abs(Points/10) + ASCII_CONV;   // convert digit to ASCII code
        lcd_data(Points1);                     // display digit
        Points1=abs(Points%10) + ASCII_CONV;   // points division by 10 and conversion to ASCII code
        lcd_data(Points1);                     // display digit
    }
}

/**
 * @brief Main function
 *
 * Demo turns a pseudo-random LED diode on for a limited time and displays
 * the current number of points
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // Configure LED port
    LED_PORT &= BIT3;           // reset P4.3
    LED_PORT &= BIT4;           // reset P4.4
    LED_PORT &= BIT5;           // reset P4.5
    LED_PORT &= BIT6;           // reset P4.6

    LED_PORT_DIR |= BIT3;       // set P4.3 as out
    LED_PORT_DIR |= BIT4;       // set P4.4 as out
    LED_PORT_DIR |= BIT5;       // set P4.5 as out
    LED_PORT_DIR |= BIT6;       // set P4.6 as out

    // Configure button port
    BUTTON_PORT_DIR &= ~BIT4;   // set P2.4 as in
    BUTTON_PORT_DIR &= ~BIT5;   // set P2.5 as in
    BUTTON_PORT_DIR &= ~BIT6;   // set P2.6 as in
    BUTTON_PORT_DIR &= ~BIT7;   // set P2.7 as in

    BUTTON_EDGE_SEL &= ~BIT4;   // button interrupt on rising edge
    BUTTON_EDGE_SEL &= ~BIT5;   // button interrupt on rising edge
    BUTTON_EDGE_SEL &= ~BIT6;   // button interrupt on rising edge
    BUTTON_EDGE_SEL &= ~BIT7;   // button interrupt on rising edge

    BUTTON_FLAG &= ~BIT4;       // clear IFG
    BUTTON_FLAG &= ~BIT5;       // clear IFG
    BUTTON_FLAG &= ~BIT6;       // clear IFG
    BUTTON_FLAG &= ~BIT7;       // clear IFG

    BUTTON_INT_EN |= BIT4;      // enable P2.4 irq
    BUTTON_INT_EN |= BIT5;      // enable P2.4 irq
    BUTTON_INT_EN |= BIT6;      // enable P2.4 irq
    BUTTON_INT_EN |= BIT7;      // enable P2.4 irq

    lcd_initialization();       // initialize and configure the LCD module

    __enable_interrupt();       // enable all interrupts

    while (1)
    {
        // Create pseudo-random number between 1 and 4
        Random_number=(rand()%4)+1;

        if (Random_number == 1)           // check if first LED should be used
        {
            Random_numberLED = BIT3;
            Random_number = BIT4;
        }

        if (Random_number == 2)           // check if second LED should be used
        {
            Random_numberLED = BIT4;
            Random_number = BIT5;
        }

        if (Random_number == 3)           // check if third LED should be used
        {
            Random_numberLED = BIT5;
            Random_number = BIT6;
        }

        if (Random_number == 4)           // check if fourth LED should be used
        {
            Random_numberLED = BIT6;
            Random_number = BIT7;
        }

        Pressed &= ~BIT0;                 // button can be pressed

        // Turn on LED for a limited time
        LED_PORT |= Random_numberLED;
        lcd_display();                    // display current points
        __delay_cycles(1000000);          // LED on for a limited time
        lcd_display();                    // display current points
        P4OUT &= ~Random_numberLED;       // turn off LED

        if (Pressed != BIT0)              // check if button not pressed while LED on
            Points = Points - 1;

        lcd_display();                    // display current points
        __delay_cycles(1000000);          // LED off for a limited time

        First_pressed &= ~BIT0;           // reset First_pressed for the next iteration
    }
}

