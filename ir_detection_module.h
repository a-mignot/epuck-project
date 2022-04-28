//Filename 		: ir_detection_module.h
//Description  	: Header of ir_detection_module.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef IR_DETECTION_MODULE_H
#define IR_DETECTION_MODULE_H

//--------- INCLUDES --------

#include <sensors/proximity.h>

//--------- PROTOTYPES ---------

uint8_t collision_detection(proximity_msg_t *prox_values);
void collision_detection_start(void);


#endif
//--------- END OF FILE ---------
