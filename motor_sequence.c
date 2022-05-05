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
#define MOTOR_STOP 0
#define COLLISION_AVOIDANCE_ANGLE 90 //in °


#define FRONT_IR_MASK 		0b11000011 //je sais pas si c'est le meilleur endroit (module) pour mettre ces define
#define BACK_IR_MASK 		0b00011000
#define FRONT_RIGHT_IR_MASK 0b00000011
#define FRONT_LEFT_IR_MASK  0b11000000
#define BACK_LEFT_IR_MASK 	0b00010000
#define BACK_RIGHT_IR_MASK 	0b00001000

#define MOVE_FORWARD 1
#define MOVE_BACKWARD -1



//--------- FUNCTIONS ---------

void move_straight(int16_t steps_needed){

	if(steps_needed > 0){
		move_loop_until(steps_needed,MOVE_FORWARD);
	}
	else if(steps_needed < 0){
		move_loop_until(steps_needed,MOVE_BACKWARD);
	}
	else{//steps_needed = 0 in this else
		right_motor_set_speed(MOTOR_STOP);
		left_motor_set_speed(MOTOR_STOP);
	}

}

///VRAIMENT AUCUNE IDEE POUR LE NOM DE LA FONCTION ---> RENAME PLEASE
void move_loop_until(int16_t steps_needed, int8_t direction){
	systime_t time;
	for(int16_t i = direction*steps_needed ; i>=0 ; i =- DEFAULT_SPEED*(DELTA_T/1000)){
		time = chVTGetSystemTime();

		right_motor_set_speed(direction*DEFAULT_SPEED);
		left_motor_set_speed(direction*DEFAULT_SPEED);

		uint8_t collision_states = get_collision_states();
		obstacle_to_avoid(direction,collision_states);

		if(get_pitch_changed()){//la fonction get_pitch_changed acquéris un static pitch_changed du module commande qui indique si le pitch a changé
			return;
			//dans le thread il faut que il'y ait un truc qui dès que tu return, il check
			//le static actual_pitch puis il déclenche la prochaine fonction de séquence
		}

		chThdSleepUntilWindowed(time, time + MS2ST(DELTA_T));
	}
}


//if degree is positive turn anticlockwise (trigonometric-wise)
//if degree is negative turn clockwise (antitrigonometric-wise)
void move_rotate(int16_t degree){

}

void move_triangle(){

}


//if the robot is rotating on itself we don't need to call this function as it will not collide with anything
//direction : 1 	- robot is going forward
//direction : -1 	- robots is going backward
//PEUT-ETRE FAIRE UN ENUM POUR LES DIRECTIONS


void obstacle_to_avoid(int8_t direction, uint8_t collision_states){

	if(direction >0 && (collision_states & FRONT_IR_MASK)){
		if(collision_states & FRONT_RIGHT_IR_MASK)	move_rotate(COLLISION_AVOIDANCE_ANGLE) ;
		if(collision_states & FRONT_LEFT_IR_MASK)	move_rotate(-COLLISION_AVOIDANCE_ANGLE);

	}
	if(direction < 0 && (collision_states & BACK_IR_MASK)){
		if(collision_states & BACK_RIGHT_IR_MASK)	move_rotate(-COLLISION_AVOIDANCE_ANGLE);
		if(collision_states & BACK_LEFT_IR_MASK)	move_rotate(COLLISION_AVOIDANCE_ANGLE) ;
	}

}

//--------- END OF FILE ---------
