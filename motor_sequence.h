//Filename 		: motor_sequence.h
//Description  	: Header of motor_sequence.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef MOTOR_SEQUENCE_H
#define MOTOR_SEQUENCE_H

//--------- INCLUDES --------

//--------- DEFINES ---------


//--------- PROTOTYPES --------


//--------- PROTOTYPES ---------

//These are basic sequences used to form more complex shapes
void move_straight(uint32_t cm_needed, int16_t speed);
void move_rotate(uint32_t degree, int16_t speed);
void move_arc(uint32_t degree, float radius, int8_t rotation, int16_t speed);

//These are the different sequences/shapes that the robot can make
//More sequences could be added
void move_triangle(uint32_t vertice_size, int16_t speed);
void move_straight_back_forth(uint32_t size, int16_t speed);
void move_rotate_back_forth(uint32_t degree, int16_t speed);
void move_circle(float radius, int8_t rotation, int16_t speed);
void move_eight_shape(uint32_t straight_size, int16_t speed);

//These are the two control loops of the motors
void move_control_loop(uint32_t steps_needed, int8_t rotation, int16_t speed);
void move_control_loop_curve(uint32_t steps_needed, int8_t rotation, int16_t speed, float radius);

void move_stop(void);

void obstacle_to_avoid(int8_t direction, uint8_t collision_states);
void motor_sequence_start(void);
void leds_reset(void);

#endif

//--------- END OF FILE ---------
