//Filename 		: command_module.c
//Description  	: This module contains functions that define the ePuck response
//to the values measured by sensors (peut être trop vague)
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//--------- INCLUDES --------
#include <ch.h>
#include "hal.h"

#include <command_module.h>
#include <sound_module.h>
#include <motor_sequence.h>
#include <chprintf.h>

//--------- DEFINES ---------


//--------- FUNCTIONS ---------

void launchSequenceFromPitch(pitch note);


//main thread qui crée les threads de séquence
static THD_FUNCTION(command_thd, arg){

	(void) arg;
	chRegSetThreadName(__FUNCTION__);


	for(;;){
		set_pitch_changed(0);
		launchSequenceFromPitch(get_current_pitch());
	}

}


void launchSequenceFromPitch(pitch note)
{
	switch(note)
	{
	case PITCH_C:
		//do sequence 1
		chprintf((BaseSequentialStream *)&SD3,"seq1\r\n");
	case PITCH_D:
		//do sequence 2
		chprintf((BaseSequentialStream *)&SD3,"seq2\r\n");
	case PITCH_E:
		//do sequence 3
		chprintf((BaseSequentialStream *)&SD3,"seq3\r\n");
	case PITCH_F:
		//do sequence 4
	case PITCH_G:
		//do sequence 5
	case PITCH_A:
		//do sequence 6
	case PITCH_B:
		//do sequence 7
	default:
		chprintf((BaseSequentialStream *)&SD3,"default\r\n");
	}
}








void command_start(){
	 static THD_WORKING_AREA(command_thd_wa, 1024); // taille à fix en fonction de ce que j'ajoute comme variables
	 chThdCreateStatic(command_thd_wa, sizeof(command_thd_wa), NORMALPRIO, command_thd, NULL);
}























