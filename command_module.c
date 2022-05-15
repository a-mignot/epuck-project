//Filename 		: command_module.c
//Description  	: This module contains functions that trigger motor sequences of the robot depending
//on the pitch received from sound_module
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//--------- INCLUDES --------
#include <ch.h>

#include <command_module.h>
#include <sound_module.h>
#include <motor_sequence.h>

//--------- DEFINES ---------

//constants for sequences
#define STRAIGHT_SPEED 1000
#define ROTATION_SPEED 500
#define ROTATE_BACK_FORTH_DEGREE 20
#define MOVE_STRAIGHT_DISTANCE 15
#define MOVE_CIRCLE_RADIUS 10

#define COMMAND_THREAD_SIZE 1024

//----INTERNAL FUNCTIONS DECLARATION----

void launchSequenceFromPitch(pitch note);


//--------- FUNCTIONS ---------


//Thread that selects the sequence to launch according to the received note from sound_module
//everytime the received pitch changes, this module sets it back to zero and launches next sequence

static THD_FUNCTION(command_thd, arg)
{
	(void) arg;
	chRegSetThreadName(__FUNCTION__);

	for(;;)
	{
		set_pitch_changed(PITCH_UNCHANGED);
		launchSequenceFromPitch(get_current_pitch());
	}
}

//Takes a pitch in input and executes the sequence corresponding to it

void launchSequenceFromPitch(pitch note)
{
	switch(note)
	{
	case PITCH_D:
		move_triangle(MOVE_STRAIGHT_DISTANCE,STRAIGHT_SPEED);
		break;
	case PITCH_E:
		move_straight_back_forth(MOVE_STRAIGHT_DISTANCE,STRAIGHT_SPEED);
		break;
	case PITCH_F:
		move_rotate_back_forth(ROTATE_BACK_FORTH_DEGREE,ROTATION_SPEED);
		break;
	case PITCH_G:
		move_circle(MOVE_CIRCLE_RADIUS,CCW_ROTATION,STRAIGHT_SPEED);
		break;
	case PITCH_A:
		move_eight_shape(MOVE_CIRCLE_RADIUS,ROTATION_SPEED);
		break;
	case PITCH_B:
		break;
	default:
		//do default
		break;
	}
	leds_reset();
}


void command_start()
{

	ir_and_sound_init();

	//initializes the command Thread with a normal priority
	static THD_WORKING_AREA(command_thd_wa, COMMAND_THREAD_SIZE);
	chThdCreateStatic(command_thd_wa, sizeof(command_thd_wa), NORMALPRIO, command_thd, NULL);
}
