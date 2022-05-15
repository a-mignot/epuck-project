//Filename 		: leds_animation.h
//Description  	: Header of leds_animation.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef LEDS_ANIMATION_H
#define LEDS_ANIMATION_H

//--------- INCLUDES --------
#include <stdint.h>
//--------- DEFINES ---------



#define FORWARD 1
#define BACKWARD 0

#define MAX_INTENSITY 100
#define FLASH_NUMBER_OF_STAGES 4

#define LED_TYPE_NORMAL 1
#define LED_TYPE_RGB 2

//--------- PROTOTYPES ---------

//sets on leds corresponding to a byte number in which 1 means on and 0 off
//i.e 0b00110001 will turn on leds 1, 5 and 6
void set_leds_from_byte(uint8_t desiredStates);

//basic led animations
void round_led_spin(uint16_t RED, uint16_t GREEN, uint16_t BLUE);
void front_back_flash(uint8_t direction, uint8_t sequence_stage);
void leds_triangle(void);
void diamond_shapes(uint8_t led_configuration);
void side_leds_on(void);


//sets off all e-puck leds that are on top
void clear_top_leds(void);

//allows to restore all temporarily changed leds states
void restore_all_leds_states(void);

//resets the cycle counter for the animation to start again
void reset_round_led_cycle(void);

#endif

//--------- END OF FILE ---------
