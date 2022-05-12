//Filename 		: command_module.c
//Description  	: This module contains functions that trigger motor sequences of the robot depending
//on the pitch received from sound_module
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//--------- INCLUDES --------
#include <ch.h>
#include "hal.h"

#include <command_module.h>
#include <sound_module.h>
#include <motor_sequence.h>

//--------- DEFINES ---------


//--------- FUNCTIONS ---------


//Thread that selects the sequence to launch according to the received note from sound_module
//everytime the received pitch changes, this module sets it back to zero and launches next sequence

static THD_FUNCTION(command_thd, arg){

	(void) arg;
	chRegSetThreadName(__FUNCTION__);

	for(;;){
		set_pitch_changed(PITCH_UNCHANGED);
		launchSequenceFromPitch(get_current_pitch());
	}

}

//Takes a pitch in input and executes the sequence corresponding to it

void launchSequenceFromPitch(pitch note)
{
	switch(note)
	{
//	case PITCH_C:
//		break;
	case PITCH_D:
		move_triangle(15,1000);
		break;
	case PITCH_E:
		move_straight_back_forth(15,1000);
		break;
	case PITCH_F:
		move_rotate_back_forth(20,550);
		break;
	case PITCH_G:
		move_circle(7.3,1,1000);
		break;
	case PITCH_A:
		move_eight_shape(10,1000);
		break;
	case PITCH_B:
		//do sequence 7
		break;
	default:
		//do default
		break;
	}
}

//initializes the command Thread with a normal priority (potentiellement à changer du coup)

void command_start(){
	 static THD_WORKING_AREA(command_thd_wa, 1024); // taille à fix en fonction de ce que j'ajoute comme variables
	 chThdCreateStatic(command_thd_wa, sizeof(command_thd_wa), NORMALPRIO, command_thd, NULL);
}

