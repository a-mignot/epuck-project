//Filename 		: motor_sequence.h
//Description  	: Header of motor_sequence.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef MOTOR_SEQUENCE_H
#define MOTOR_SEQUENCE_H

//--------- INCLUDES --------

//--------- DEFINES ---------



//--------- PROTOTYPES ---------

void move_straight(int16_t distance);;
void move_rotate(int16_t degree);
void move_triangle(void);
void move_loop_until(int16_t steps_needed, int8_t direction);
void obstacle_to_avoid(int8_t direction, uint8_t collision_states);

#endif

//--------- END OF FILE ---------
