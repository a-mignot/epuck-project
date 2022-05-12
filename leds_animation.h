//Filename 		: leds_animation.h
//Description  	: Header of leds_animation.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef LEDS_ANIMATION_H
#define LEDS_ANIMATION_H

//--------- INCLUDES --------

//--------- DEFINES ---------



//--------- PROTOTYPES ---------

void round_led_spin(int RED, int GREEN, int BLUE);
void clear_top_leds(void);
void leds_animation_start(void);
void set_leds_from_byte(int desiredStates);
#endif

//--------- END OF FILE ---------
