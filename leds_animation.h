//Filename 		: leds_animation.h
//Description  	: Header of leds_animation.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef LEDS_ANIMATION_H
#define LEDS_ANIMATION_H

//--------- INCLUDES --------
#include <ch.h>
//--------- DEFINES ---------

#define FORWARD 1
#define BACKWARD 0
#define MAX_INTENSITY 100

//--------- PROTOTYPES ---------

void round_led_spin(uint16_t RED, uint16_t GREEN, uint16_t BLUE);
void clear_top_leds(void);
void leds_animation_start(void);
void set_leds_from_byte(uint8_t desiredStates);
void front_back_flash(uint8_t direction);
void leds_triangle(void);
void tiles_switch(uint8_t loop_number);
void side_leds_on(void);
void restore_all_leds_states(void);
#endif

//--------- END OF FILE ---------
