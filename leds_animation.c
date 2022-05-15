//Filename 		: leds_animation.c
//Description  	: This module contains all the LED animations
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1

//--------- INCLUDES --------
#include<leds_animation.h>
#include<leds.h>
#include<spi_comm.h>
#include<math.h>
//--------- DEFINES ---------
#define NUMBER_OF_LEDS 4
#define NUMBER_OF_RGB_LEDS 4
#define RGB 3

#define ON 1
#define OFF 0

#define RED 0
#define GREEN 1
#define BLUE 2

#define ORANGE 90,50,20
#define BRIGHT_BLUE 20,50,90
#define PURPLE 80, 80, 100
#define RGB_LED_OFF 0,0,0

//--------- FUNCTIONS ---------

static uint8_t leds_states[NUMBER_OF_LEDS];
static uint8_t rgb_leds_states[NUMBER_OF_RGB_LEDS][RGB];

void tiles_symbol(void);
void set_save_led(uint8_t led_number, uint8_t state);
void set_save_rgb_led(uint8_t led_number, uint8_t red, uint8_t green, uint8_t blue);


//Circle animation with leds

void round_led_spin(uint16_t red, uint16_t green, uint16_t blue)
{
	static int cycle = 0;
	static int on_off = ON;
	switch(cycle)
	{
	case 0:
		set_save_led(LED1,on_off);
		break;
	case 1:
		set_save_rgb_led(LED2,on_off*red,on_off*green,on_off*blue);
		break;
	case 2:
		set_save_led(LED3,on_off);
		break;
	case 3:
		set_save_rgb_led(LED4,on_off*red,on_off*green,on_off*blue);
		break;
	case 4:
		set_save_led(LED5,on_off);
		break;
	case 5:
		set_save_rgb_led(LED6,on_off*red,on_off*green,on_off*blue);
		break;
	case 6:
		set_save_led(LED7,on_off);
		break;
	case 7:
		set_save_rgb_led(LED8,on_off*red,on_off*green,on_off*blue);
		break;
	}
	if(cycle==7){
	on_off = (on_off == ON) ? OFF : ON;
	cycle = 0;
	}
	else{cycle++;}
}

//Takes a 8 bit number in entry and lights the led corresponding to each bit.

void set_leds_from_byte(uint8_t desiredStates)
{
	for (int8_t i=7;i>=0;i--)
	{
		if(i%2==0)
		{
			set_led(i/2,(desiredStates & (1 << i)) != 0);
		}
		else
		{
			set_rgb_led(((i-1)/2),RGB_MAX_INTENSITY*((desiredStates & (1 << i)) != 0),0,0);
		}
	}
}

void front_back_flash(uint8_t direction, uint8_t sequence_stage)
{
	if(direction == FORWARD)
	{
		switch(sequence_stage)
		{
		case 1:
			set_save_rgb_led(LED2,ORANGE);
			set_save_rgb_led(LED8,ORANGE);
			break;
		case 2:
			set_save_rgb_led(LED2,RGB_LED_OFF);
			set_save_rgb_led(LED8,RGB_LED_OFF);
			break;
		case 3:
			set_save_rgb_led(LED2,BRIGHT_BLUE);
			set_save_rgb_led(LED8,BRIGHT_BLUE);
			break;
		case 4:
			set_save_rgb_led(LED2,RGB_LED_OFF);
			set_save_rgb_led(LED8,RGB_LED_OFF);
			break;
		}
	}
	if(direction == BACKWARD)
	{
		switch(sequence_stage)
		{
		case 1:
			set_save_rgb_led(LED4,ORANGE);
			set_save_rgb_led(LED6,ORANGE);
			break;
		case 2:
			set_save_rgb_led(LED4,RGB_LED_OFF);
			set_save_rgb_led(LED6,RGB_LED_OFF);
			break;
		case 3:
			set_save_rgb_led(LED4,BRIGHT_BLUE);
			set_save_rgb_led(LED6,BRIGHT_BLUE);
			break;
		case 4:
			set_save_rgb_led(LED4,RGB_LED_OFF);
			set_save_rgb_led(LED6,RGB_LED_OFF);
			break;
		}
	}
}

void side_leds_on()
{
	set_save_led(LED3,ON);
	set_save_led(LED7,ON);
}

void leds_triangle()
{
	set_save_led(LED1,ON);
	set_save_led(LED3,ON);
	set_save_led(LED7,ON);
}

void tiles_symbol()
{
	set_save_led(LED1,ON);
	set_save_led(LED3,ON);
	set_save_led(LED5,ON);
	set_save_led(LED7,ON);
}

void diamond_shapes(uint8_t leds_type)
{
	if(leds_type == LED_TYPE_NORMAL){
		tiles_symbol();
	}
	if(leds_type == LED_TYPE_RGB){
		set_save_rgb_led(LED2,PURPLE);
		set_save_rgb_led(LED4,PURPLE);
		set_save_rgb_led(LED6,PURPLE);
		set_save_rgb_led(LED8,PURPLE);
	}
}



void clear_top_leds()
{
	for (int i = 0; i<NUMBER_OF_LEDS; i++)
	{
		set_save_led(i,OFF);
		set_save_rgb_led(i, RGB_LED_OFF);
	}
}

void leds_animation_start()
{
	spi_comm_start();
	clear_leds();
}

void set_save_led(uint8_t led_number, uint8_t state)
{
	set_led(led_number,state);
	leds_states[led_number] = state;
}

void set_save_rgb_led(uint8_t led_number, uint8_t red, uint8_t green, uint8_t blue)
{
	set_rgb_led(led_number,red, green, blue);
	rgb_leds_states[led_number][RED] = red;
	rgb_leds_states[led_number][GREEN] = green;
	rgb_leds_states[led_number][BLUE] = blue;
}


void restore_all_leds_states()
{
	set_led(LED1,leds_states[LED1]);
	set_led(LED3,leds_states[LED3]);
	set_led(LED5,leds_states[LED5]);
	set_led(LED7,leds_states[LED7]);
	set_rgb_led(LED2,rgb_leds_states[LED2][RED],rgb_leds_states[LED2][GREEN],rgb_leds_states[LED2][BLUE]);
	set_rgb_led(LED4,rgb_leds_states[LED4][RED],rgb_leds_states[LED4][GREEN],rgb_leds_states[LED4][BLUE]);
	set_rgb_led(LED6,rgb_leds_states[LED6][RED],rgb_leds_states[LED6][GREEN],rgb_leds_states[LED6][BLUE]);
	set_rgb_led(LED8,rgb_leds_states[LED8][RED],rgb_leds_states[LED8][GREEN],rgb_leds_states[LED8][BLUE]);
}


//--------- END OF FILE ---------
