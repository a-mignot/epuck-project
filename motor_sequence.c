//Filename 		: motor_sequence.c
//Description  	: this file contains all the motors sequences possible for the motor
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//-- Additional information --
/*
 * In order to check if the pitch has changed or if there is a collision detected,
 * the movements of the robot need to be decomposed in tiny parts of XX ms (10ms for test)
 * We can't use messagebus_topic_wait() semaphores or mutexes and such as
 * because if the sleep delay of the thread is not consistent then the movement of
 * the robot won't be accurate.
 */

//--------- INCLUDES --------

#include <motors.h>
#include <ch.h>

#include <motor_sequence.h>


//--------- DEFINES ---------

#define DEFAULT_SPEED 1000 //in step/s -> corresponds to 13 cm/s !!!NEEDS TO BE VERIFIED EXPERIMENTALLY!!!
#define DELTA_T 10 //in ms

//--------- FUNCTIONS ---------

void move_forward(int16_t steps_needed, systime_t time){
	for(int16_t i = steps_needed ; i>=0 ; i =- DEFAULT_SPEED*(DELTA_T/1000)){
		time = chVTGetSystemTime();

		right_motor_set_speed(DEFAULT_SPEED);
		left_motor_set_speed(DEFAULT_SPEED);

		if(get_collision_vector()){

			//do stuff
		}

		if(get_pitch_changed()){//la fonction get_pitch_changed acquéris un static pitch_changed du module commande qui indique si le pitch a changé
			return;
			//dans le thread il faut que il'y ait un truc qui dès que tu return, il check
			//le static actual_pitch puis il déclenche la prochaine fonction de séquence
		}

		chThdSleepUntilWindowed(time, time + MS2ST(DELTA_T));
	}

}

void move_backward(){

}

void move_rotate(){

}

void move_triangle(){

}

//--------- END OF FILE ---------
