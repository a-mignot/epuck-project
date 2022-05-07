//Filename 		: command_module.c
//Description  	: This module contains functions that define the ePuck response
//to the values measured by sensors (peut �tre trop vague)
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//--------- INCLUDES --------
#include <command_module.h>
#include <sound_module.h>
#include <motor_sequence.h>

//--------- DEFINES ---------


//--------- FUNCTIONS ---------



//main thread qui cr�e les threads de s�quence
static THD_FUNCTION(command_thd, arg){

	(void) arg;
	chRegSetThreadName(__FUNCTION__);


	for(;;){
	}

}




void command_start(){
	 static THD_WORKING_AREA(command_thd_wa, 1024); // taille � fix en fonction de ce que j'ajoute comme variables
	 chThdCreateStatic(command_thd_wa, sizeof(command_thd_wa), NORMALPRIO, command_thd, NULL);
}























