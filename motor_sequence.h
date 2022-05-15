//Filename 		: motor_sequence.h
//Description  	: Header of motor_sequence.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef MOTOR_SEQUENCE_H
#define MOTOR_SEQUENCE_H

//--------- INCLUDES --------

//--------- DEFINES ---------

#define CW_ROTATION -1
#define CCW_ROTATION 1

//--------- PROTOTYPES ---------

//These are the different sequences/shapes that the robot can make
void move_triangle(uint32_t vertice_size, int16_t speed);
void move_straight_back_forth(uint32_t size, int16_t speed);
void move_rotate_back_forth(uint32_t degree, int16_t speed);
void move_circle(float radius, int8_t rotation, int16_t speed);
void move_eight_shape(uint32_t straight_size, int16_t speed);

//inits collision detection thread and sound module for motor_sequence
void ir_and_sound_init(void);

//turns off every led on top of the e-puck
void leds_reset(void);

#endif

//--------- END OF FILE ---------
