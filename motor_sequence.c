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
#include <sound_module.h>
#include <ir_detection_module.h>
#include <leds_animation.h>

//--------- DEFINES ---------

#define MAX_SPEED 		 1000 //in step/s -> corresponds to 13 cm/s !!!NEEDS TO BE VERIFIED EXPERIMENTALLY!!!
#define MIN_SPEED 		 100 //step/s NEEDS TO BE VERIFIED EXPERIMENTALLY AS WELL
#define MOTOR_STOP_SPEED 0 //in step/s

#define DELTA_T 10 //in ms

#define FRONT_IR_MASK 		0b11000011 //je sais pas si c'est le meilleur endroit (module) pour mettre ces define
#define BACK_IR_MASK 		0b00011000
#define FRONT_RIGHT_IR_MASK 0b00000011
#define FRONT_LEFT_IR_MASK  0b11000000
#define BACK_LEFT_IR_MASK 	0b00010000
#define BACK_RIGHT_IR_MASK 	0b00001000

#define NO_ROTATION  0
#define CCW_ROTATION 1
#define CW_ROTATION -1

#define DIR_FORWARD   1
#define DIR_BACKWARD -1

#define WHEEL_PERIMETER 13
#define WHEEL_DISTANCE  2.675f    //cm distance from center of the robot one wheel
#define PI 				3.14159265f
#define WHEEL_RADIUS 	2.069f    //cm  =13/2pi

#define EIGHT_SIZE_TO_RADIUS 0.59588f
#define ARC_DEGREE_LENGTH	 280
//this constant C corresponds to find the radius r of the arc of an eight shape from the straight part d
//r = d*C. C corresponds to 0.5*tan(0.5a) where a is the angle (100°) between the two straight parts of the eight
#define COLLISION_AVOIDANCE_ANGLE 90 //in °

//-------- MACROS ----------

#define CM_TO_STEPS(DIST) 			(int)((DIST*1000)/WHEEL_PERIMETER)
#define DEG_TO_STEPS(DEG,RADIUS) 	CM_TO_STEPS(DEG_TO_RAD(DEG)*RADIUS)
#define DEG_TO_RAD(DEG) 			(DEG*PI/180.0f)

//--------- FUNCTIONS ---------

uint8_t mask_modification(uint8_t collision_input);


void move_straight(uint32_t cm_needed, int16_t speed){
	if((speed <= -MIN_SPEED ||speed >= MIN_SPEED) && cm_needed != 0){
		move_control_loop(CM_TO_STEPS(cm_needed),NO_ROTATION,speed);
	}
	move_stop();
}

//positive speed : counter-clockwise rotation
//negative speed : clockwise rotation
void move_rotate(uint32_t degree, int16_t speed){
	if(degree != 0){
		if(speed > MIN_SPEED){
			move_control_loop(DEG_TO_STEPS(degree,WHEEL_DISTANCE),CCW_ROTATION,speed);
		}
		if(speed < -MIN_SPEED){
			move_control_loop(DEG_TO_STEPS(degree,WHEEL_DISTANCE),CW_ROTATION,speed);
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

void move_straight_back_forth(uint32_t size, int16_t speed){
	while(1){
		move_straight(size,speed);
		if(get_pitch_changed()) return;
		move_straight(size,-speed);
		if(get_pitch_changed()) return;
	}
}

void move_rotate_back_forth(uint32_t degree, int16_t speed){
	while(1){
		move_rotate(degree,speed);
		if(get_pitch_changed()) return;
		move_rotate(degree,-speed);
		if(get_pitch_changed()) return;
		round_led_spin(100,0,0);
	}
}

void move_circle(float radius, int8_t rotation, int16_t speed){
	while(1){
		move_arc(360,radius,rotation,speed);
		if(get_pitch_changed()) return;
	}
}

void move_eight_shape(uint32_t straight_size, int16_t speed){
	while(1){
//		move_straight(straight_size,speed);
//		if(get_pitch_changed()) return;
		move_arc(360,straight_size,CCW_ROTATION,speed);
		if(get_pitch_changed()) return;
//		move_straight(straight_size,speed);
//		if(get_pitch_changed()) return;
		move_arc(360,straight_size,CW_ROTATION,speed);
		if(get_pitch_changed()) return;
	}
}


void move_arc(uint32_t degree, float radius, int8_t rotation, int16_t speed){
	if((speed <= -MIN_SPEED ||speed >= MIN_SPEED) && degree != 0){
		move_control_loop_curve(DEG_TO_STEPS(degree,radius),rotation,speed,radius);
	}
	move_stop();
}


//This function is the main control loop used to control the motors to the desired speed
//It can only handle same speed for the 2 motors which means it is suitable for rotation
//of the robot on itself and going forward and backward i.e no curved trajectories
void move_control_loop(uint32_t steps_needed, int8_t rotation, int16_t speed){

	systime_t time;
	int8_t direction = (speed > 0) ? DIR_FORWARD : DIR_BACKWARD;

	for(int32_t i = steps_needed ; i>0 ; i -= (direction*speed*DELTA_T)/1000){
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
		else if(rotation == CCW_ROTATION || rotation == CW_ROTATION){
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

//This function is very similar to move_control_loop except that it can handle circular trajectories
void move_control_loop_curve(uint32_t steps_needed, int8_t rotation, int16_t speed, float radius){

	systime_t time;
	int8_t direction = (speed > 0) ? DIR_FORWARD : DIR_BACKWARD;

	int16_t speed_ext = speed*((radius+WHEEL_DISTANCE)/radius);
	//speed verification
	if(speed_ext > MAX_SPEED){
		speed_ext = MAX_SPEED;
		speed = speed_ext*(radius)/(radius+WHEEL_DISTANCE);
	}
	int16_t speed_int = speed*((radius-WHEEL_DISTANCE)/radius);

	for(int32_t i = steps_needed ; i>0 ; i -= (direction*speed*DELTA_T)/1000){
		//we multiply by direction to always subtract a positive value from i
		time = chVTGetSystemTime();

		if(rotation == CCW_ROTATION){
			uint8_t collision_states = get_collision_states();
			obstacle_to_avoid(direction,collision_states);
			right_motor_set_speed(speed_ext);
			left_motor_set_speed(speed_int);
		}
		else if(rotation == CW_ROTATION){
			uint8_t collision_states = get_collision_states();
			obstacle_to_avoid(direction,collision_states);
			right_motor_set_speed(speed_int);
			left_motor_set_speed(speed_ext);
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


//direction :  1 - robot is going forward
//direction : -1 - robot is going backward
void obstacle_to_avoid(int8_t direction, uint8_t collision_states){

//the response of the system must be fast so we chose to put max_speed for the rotation
	if(direction == DIR_FORWARD  && (collision_states & FRONT_IR_MASK)){
		set_leds_from_byte(mask_modification(collision_states & FRONT_IR_MASK));
		if(collision_states & FRONT_RIGHT_IR_MASK) move_rotate(COLLISION_AVOIDANCE_ANGLE, MAX_SPEED);
		else if(collision_states & FRONT_LEFT_IR_MASK)  move_rotate(COLLISION_AVOIDANCE_ANGLE,-MAX_SPEED);
	}
	if(direction == DIR_BACKWARD && (collision_states & BACK_IR_MASK)){
		set_leds_from_byte(mask_modification(collision_states & BACK_IR_MASK));
		if(collision_states & BACK_RIGHT_IR_MASK)  move_rotate(COLLISION_AVOIDANCE_ANGLE, -MAX_SPEED);
		else if(collision_states & BACK_LEFT_IR_MASK)   move_rotate(COLLISION_AVOIDANCE_ANGLE, MAX_SPEED);
	}
	clear_top_leds();
}

//Modifies the collision states in order for them to correspond to the leds near to each IR sensor

uint8_t mask_modification(uint8_t collision_input)
{
	uint8_t converted_output = 0;
	if((collision_input & (0b10000001)) != 0) // Ir 1 and 8 mapped on led 1
	{
		converted_output |= 0b00000001;
	}
	if ((collision_input & (0b00000010)) != 0) // Ir 2 mapped on led 2
	{
		converted_output |= 0b00000010;
	}
	if ((collision_input & (0b00000100)) != 0) // Ir 3 mapped on led 3
	{
		converted_output |= 0b00000100;
	}
	if ((collision_input & (0b00001000)) != 0) // Ir 4 mapped on led 4
	{
		converted_output |= 0b00001000;
	}
	if ((collision_input & (0b000010000)) != 0) // Ir 5 mapped on led 6
	{
		converted_output |= 0b00100000;
	}
	if ((collision_input & (0b00100000)) != 0) // Ir 6 mapped on led 7
	{
		converted_output |= 0b01000000;
	}
	if ((collision_input & (0b01000000)) != 0) // Ir 7 mapped on led 8
	{
		converted_output |= 0b10000000;
	}
	return converted_output;
}

//--------- END OF FILE ---------
