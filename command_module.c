//Filename 		: command_module.c
//Description  	: This module contains functions that define the ePuck response
//to the values measured by sensors (peut être trop vague)
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//--------- INCLUDES --------
#include <command_module.h>
#include <sound_module.h>
#include <motor_sequence.h>

//--------- DEFINES ---------


//--------- FUNCTIONS ---------



//main thread qui crée les threads de séquence
static THD_FUNCTION(command_thd, arg){

	(void) arg;
	chRegSetThreadName(__FUNCTION__);


	for(;;){
	}

}




void command_start(){
	 static THD_WORKING_AREA(command_thd_wa, 1024); // taille à fix en fonction de ce que j'ajoute comme variables
	 chThdCreateStatic(command_thd_wa, sizeof(command_thd_wa), NORMALPRIO, command_thd, NULL);
}























