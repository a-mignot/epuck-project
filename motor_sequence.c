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

#define MAX_SPEED 1000 //in step/s -> corresponds to 13 cm/s !!!NEEDS TO BE VERIFIED EXPERIMENTALLY!!!
#define DELTA_T 10 //in ms
#define MOTOR_STOP_SPEED 0 //in step/s
#define COLLISION_AVOIDANCE_ANGLE 90 //in °


#define FRONT_IR_MASK 		0b11000011 //je sais pas si c'est le meilleur endroit (module) pour mettre ces define
#define BACK_IR_MASK 		0b00011000
#define FRONT_RIGHT_IR_MASK 0b00000011
#define FRONT_LEFT_IR_MASK  0b11000000
#define BACK_LEFT_IR_MASK 	0b00010000
#define BACK_RIGHT_IR_MASK 	0b00001000

#define NO_ROTATION 0
#define ACLOCKWISE_ROTATION 1
#define CLOCKWISE_ROTATION -1

#define DIR_FORWARD 1
#define DIR_BACKWARD -1

#define WHEEL_PERIMETER 13
#define WHEEL_DISTANCE      5.35f    //cm
#define PI 3.14159265f


//-------- MACROS ----------

#define CM_TO_STEPS(DIST) 	(int)((DIST*1000)/WHEEL_PERIMETER)
#define DEG_TO_STEPS(DEG) 	CM_TO_STEPS(DEG_TO_RAD(DEG)*WHEEL_DISTANCE/2.0f)
#define DEG_TO_RAD(DEG) 	(DEG*PI/180.0f)

//--------- FUNCTIONS ---------

void move_straight(uint32_t cm_needed, int16_t speed){
	if(speed != 0 && cm_needed != 0){
		move_control_loop(CM_TO_STEPS(cm_needed),NO_ROTATION,speed);
	}
	move_stop();
}

//positive speed : anticlockwise rotation
//negative speed : clockwise rotation
void move_rotate(uint32_t degree, int16_t speed){
	if(degree != 0){
		if(speed > 0){
			move_control_loop(DEG_TO_STEPS(degree),ACLOCKWISE_ROTATION,speed);
		}
		if(speed < 0){
			move_control_loop(DEG_TO_STEPS(degree),CLOCKWISE_ROTATION,speed);
		}
	}
	move_stop();
}

void move_triangle(uint32_t vertice_size, int16_t speed){
	while(1){
		move_straight(vertice_size,speed);
		if(get_pitch_changed()) return;
		move_rotate(120,speed);
		if(get_pitch_changed()) return;
	}
}


//This function is the main control loop used to control the motors to the desired speed
//It can only handle same speed for the 2 motors which means it is suitable for rotation
//of the robot on itself and going forward and backward i.e no curved trajectories
void move_control_loop(uint32_t steps_needed, int8_t rotation, int16_t speed){

	systime_t time;
	int8_t direction = (speed > 0) ? DIR_FORWARD : DIR_BACKWARD;

	for(uint32_t i = steps_needed ; i>0 ; i -= (direction*speed*DELTA_T)/1000){
		//we multiply by direction to always subtract a positive value from i
		time = chVTGetSystemTime();

		if(rotation == NO_ROTATION){
			uint8_t collision_states = get_collision_states();
			obstacle_to_avoid(direction,collision_states);
			right_motor_set_speed(speed);
			left_motor_set_speed(speed);
		}

		//if the robots rotates on itself obstacle_to_avoid is useless
		//and we avoid an infinite recursion of this move_control_loop
		if(rotation == ACLOCKWISE_ROTATION || rotation == CLOCKWISE_ROTATION){
			right_motor_set_speed(speed);
			left_motor_set_speed(-speed);
		}
		else return; //rotation parameter is badly given to function

		if(get_pitch_changed()){//la fonction get_pitch_changed acquéris un static pitch_changed du module sound qui indique si le pitch a changé
			return;
			//dans le thread il faut que il'y ait un truc qui dès que tu return, il check
			//le static actual_pitch puis il déclenche la prochaine fonction de séquence
		}
		chThdSleepUntilWindowed(time, time + MS2ST(DELTA_T));
	}
}


void move_stop(){
	right_motor_set_speed(MOTOR_STOP_SPEED);
	left_motor_set_speed(MOTOR_STOP_SPEED);
}


//direction : 1 	- robot is going forward
//direction : -1 	- robots is going backward
void obstacle_to_avoid(int8_t direction, uint8_t collision_states){
//the response of the system must be fast so we like to put max_speed for the rotation
	if(direction == DIR_FORWARD  && (collision_states & FRONT_IR_MASK)){
		if(collision_states & FRONT_RIGHT_IR_MASK) move_rotate(DEG_TO_STEPS(COLLISION_AVOIDANCE_ANGLE), MAX_SPEED);
		if(collision_states & FRONT_LEFT_IR_MASK)  move_rotate(DEG_TO_STEPS(COLLISION_AVOIDANCE_ANGLE),-MAX_SPEED);

	}
	if(direction == DIR_BACKWARD && (collision_states & BACK_IR_MASK)){
		if(collision_states & BACK_RIGHT_IR_MASK)  move_rotate(DEG_TO_STEPS(COLLISION_AVOIDANCE_ANGLE),-MAX_SPEED);
		if(collision_states & BACK_LEFT_IR_MASK)   move_rotate(DEG_TO_STEPS(COLLISION_AVOIDANCE_ANGLE), MAX_SPEED);
	}

}

//--------- END OF FILE ---------
