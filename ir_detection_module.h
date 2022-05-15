//Filename 		: ir_detection_module.h
//Description  	: Header of ir_detection_module.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef IR_DETECTION_MODULE_H
#define IR_DETECTION_MODULE_H

//--------- INCLUDES --------

#include <sensors/proximity.h>


//--------- DEFINES ---------

#define FRONT_IR_MASK 		0b11000011
#define BACK_IR_MASK 		0b00011000
#define FRONT_RIGHT_IR_MASK 0b00000011
#define FRONT_LEFT_IR_MASK  0b11000000
#define BACK_LEFT_IR_MASK 	0b00010000
#define BACK_RIGHT_IR_MASK 	0b00001000

//--------- PROTOTYPES -------

uint8_t get_collision_states(void);
void collision_detection_start(void);

#endif
//--------- END OF FILE ---------
