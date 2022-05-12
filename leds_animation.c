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
#define ON 1
#define OFF 0



//--------- FUNCTIONS ---------


//Circle animation with leds

void round_led_spin(int RED, int GREEN, int BLUE)
{
	static int cycle = 0;
	static int on_off = ON;
	switch(cycle)
	{
	case 0:
		set_led(LED1,on_off);
		break;
	case 1:
		set_rgb_led(LED2,on_off*RED,on_off*GREEN,on_off*BLUE);
		break;
	case 2:
		set_led(LED3,on_off);
		break;
	case 3:
		set_rgb_led(LED4,on_off*RED,on_off*GREEN,on_off*BLUE);
		break;
	case 4:
		set_led(LED5,on_off);
		break;
	case 5:
		set_rgb_led(LED6,on_off*RED,on_off*GREEN,on_off*BLUE);
		break;
	case 6:
		set_led(LED7,on_off);
		break;
	case 7:
		set_rgb_led(LED8,on_off*RED,on_off*GREEN,on_off*BLUE);
		break;
	}
	if(cycle==7){
	on_off = (on_off == ON) ? OFF : ON;
	cycle = 0;
	}
	else{cycle++;}
}

//Takes a 8 bit number in entry and lights the led corresponding to each bit.

void set_leds_from_byte(int desiredStates)
{
	for (int i=7;i>=0;i--)
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





void clear_top_leds()
{
	clear_leds();
}

void leds_animation_start()
{
	spi_comm_start();
	clear_leds();
}



//--------- END OF FILE ---------
