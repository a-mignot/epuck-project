//Filename 		: motor_sequence.h
//Description  	: Header of motor_sequence.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef MOTOR_SEQUENCE_H
#define MOTOR_SEQUENCE_H

//--------- INCLUDES --------

//--------- DEFINES ---------



//--------- PROTOTYPES ---------

void move_forward(int16_t distance, systime_t time);
void move_backward(void);
void move_rotate(void);
void move_triangle(void);

#endif

//--------- END OF FILE ---------
