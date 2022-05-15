//Filename 		: motor_sequence.c
//Description  	: this file contains all the motors sequences possible for the motor
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//-- Additional information --
/*
 * In order to check if the pitch has changed or if there is a collision detected,
 * the movements of the robot need to be decomposed in tiny parts of 10 ms
 * We can't use messagebus_topic_wait() semaphores or mutexes and such as
 * because if the sleep delay of the thread is not consistent then the movement of
 * the robot won't be accurate.
 */

//--------- INCLUDES --------

#include <motors.h>

#include <motor_sequence.h>
#include <sound_module.h>
#include <ir_detection_module.h>
#include <leds_animation.h>

//--------- DEFINES ---------

#define MAX_SPEED 		 1000 //in step/s -> corresponds to 13 cm/s
#define MIN_SPEED 		 100 //step/s
#define MOTOR_STOP_SPEED 0 //in step/s

#define DELTA_T 10 //in ms

#define NO_ROTATION  0

#define DIR_FORWARD   1
#define DIR_BACKWARD -1

#define WHEEL_PERIMETER 13
#define WHEEL_DISTANCE  2.675f    //cm distance from center of the robot one wheel
#define PI 				3.14159265f

#define COLLISION_AVOIDANCE_ANGLE 90 //in °

#define ONE_TURN_DEGREES 360

//-------- MACROS ----------

#define CM_TO_STEPS(DIST) 			(int)((DIST*1000)/WHEEL_PERIMETER)
#define DEG_TO_STEPS(DEG,RADIUS) 	CM_TO_STEPS(DEG_TO_RAD(DEG)*RADIUS)
#define DEG_TO_RAD(DEG) 			(DEG*PI/180.0f)

//----INTERNAL FUNCTIONS DECLARATION----

//These are basic sequences used to form more complex shapes
void move_straight(uint32_t cm_needed, int16_t speed);
void move_rotate(uint32_t degree, int16_t speed);
void move_arc(uint32_t degree, float radius, int8_t rotation, int16_t speed);

//These are the two control loops of the motors
void move_control_loop(uint32_t steps_needed, int8_t rotation, int16_t speed);
void move_control_loop_curve(uint32_t steps_needed, int8_t rotation, int16_t speed, float radius);

//stops motors
void move_stop(void);

//rotates the e-puck whenever a future collision is detected
void obstacle_to_avoid(int8_t direction, uint8_t collision_states);

//assign ir sensors to led positions
uint8_t mask_modification(uint8_t collision_input);

//--------- FUNCTIONS ---------



void move_straight(uint32_t cm_needed, int16_t speed)
{
	if((speed <= -MIN_SPEED || speed >= MIN_SPEED) && cm_needed != 0)
	{
		move_control_loop(CM_TO_STEPS(cm_needed),NO_ROTATION,speed);
	}
	move_stop();
}

//positive speed : counter-clockwise rotation
//negative speed : clockwise rotation
void move_rotate(uint32_t degree, int16_t speed)
{
	if(degree != 0)
	{
		if(speed > MIN_SPEED)
		{
			move_control_loop(DEG_TO_STEPS(degree,WHEEL_DISTANCE),CCW_ROTATION,speed);
		}
		if(speed < -MIN_SPEED)
		{
			move_control_loop(DEG_TO_STEPS(degree,WHEEL_DISTANCE),CW_ROTATION,speed);
		}
	}
	move_stop();
}

void move_triangle(uint32_t vertice_size, int16_t speed)
{
	leds_triangle();
	while(1)
	{
		move_straight(vertice_size,speed);

		//if the pitch changed, we need to get out of the sequence to start another
		if(get_pitch_changed()) return;
		move_rotate(120,speed);
		if(get_pitch_changed()) return;
	}
}

void move_straight_back_forth(uint32_t size, int16_t speed)
{
	while(1)
	{
		move_straight(size,speed);
		if(get_pitch_changed()) return;

		for(uint8_t i= 1; i < FLASH_NUMBER_OF_STAGES+1 ; i++)
		{
			front_back_flash(FORWARD, i);
			if(get_pitch_changed()) return;
			chThdSleepMilliseconds(100);
		}
		if(get_pitch_changed()) return;

		move_straight(size,-speed);
		if(get_pitch_changed()) return;

		for(uint8_t i= 1; i < FLASH_NUMBER_OF_STAGES+1 ; i++)
		{
			front_back_flash(BACKWARD, i);
			if(get_pitch_changed()) return;
			chThdSleepMilliseconds(100);
		}
		if(get_pitch_changed()) return;
	}
}

void move_rotate_back_forth(uint32_t degree, int16_t speed)
{
	while(1)
	{
		move_rotate(degree,speed);
		if(get_pitch_changed()) return;
		move_rotate(degree,-speed);
		if(get_pitch_changed()) return;
		round_led_spin(MAX_INTENSITY,0,0);
	}
}

void move_circle(float radius, int8_t rotation, int16_t speed)
{
	side_leds_on();
	while(1)
	{
		move_arc(ONE_TURN_DEGREES,radius,rotation,speed);
		if(get_pitch_changed()) return;
	}
}

void move_eight_shape(uint32_t straight_size, int16_t speed)
{
	while(1)
	{
		diamond_shapes(LED_TYPE_NORMAL);
		move_arc(ONE_TURN_DEGREES,straight_size,CCW_ROTATION,speed);
		if(get_pitch_changed()) return;

		diamond_shapes(LED_TYPE_RGB);
		move_arc(ONE_TURN_DEGREES,straight_size,CW_ROTATION,speed);
		if(get_pitch_changed()) return;
		clear_top_leds();
	}
}


void move_arc(uint32_t degree, float radius, int8_t rotation, int16_t speed)
{
	if((speed <= -MIN_SPEED || speed >= MIN_SPEED) && degree != 0)
	{
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
		//and we avoid an infinite recursion of the move_control_loop function
		else if(rotation == CCW_ROTATION || rotation == CW_ROTATION)
		{
			right_motor_set_speed(speed);
			left_motor_set_speed(-speed);
		}
		else return; //rotation parameter is badly given to function

		if(get_pitch_changed()) return;
		chThdSleepUntilWindowed(time, time + MS2ST(DELTA_T));
	}
}

//This function is very similar to move_control_loop except that it can handle circular trajectories
void move_control_loop_curve(uint32_t steps_needed, int8_t rotation, int16_t speed, float radius)
{
	systime_t time;
	int8_t direction = (speed > 0) ? DIR_FORWARD : DIR_BACKWARD;
	int16_t speed_ext = speed*((radius+WHEEL_DISTANCE)/radius);

	//speed verification
	if(speed_ext > MAX_SPEED)
	{
		speed_ext = MAX_SPEED;
		speed = speed_ext*(radius)/(radius+WHEEL_DISTANCE);
	}
	int16_t speed_int = speed*((radius-WHEEL_DISTANCE)/radius);

	for(int32_t i = steps_needed ; i>0 ; i -= (direction*speed*DELTA_T)/1000)
	{
		//we multiply by direction to always subtract a positive value from i
		time = chVTGetSystemTime();

		if(rotation == CCW_ROTATION)
		{
			uint8_t collision_states = get_collision_states();
			obstacle_to_avoid(direction,collision_states);
			right_motor_set_speed(speed_ext);
			left_motor_set_speed(speed_int);
		}
		else if(rotation == CW_ROTATION)
		{
			uint8_t collision_states = get_collision_states();
			obstacle_to_avoid(direction,collision_states);
			right_motor_set_speed(speed_int);
			left_motor_set_speed(speed_ext);
		}

		else return; //rotation parameter is badly given to function

		if(get_pitch_changed()) return;

		chThdSleepUntilWindowed(time, time + MS2ST(DELTA_T));
	}
}

void move_stop()
{
	right_motor_set_speed(MOTOR_STOP_SPEED);
	left_motor_set_speed(MOTOR_STOP_SPEED);
}


//direction :  1 - robot is going forward
//direction : -1 - robot is going backward
void obstacle_to_avoid(int8_t direction, uint8_t collision_states){

//the response of the system must be fast so we chose to put max_speed for the rotation
	if(direction == DIR_FORWARD && (collision_states & FRONT_IR_MASK))
	{
		set_leds_from_byte(mask_modification(collision_states & FRONT_IR_MASK));
		if(collision_states & FRONT_RIGHT_IR_MASK)
		{
			move_rotate(COLLISION_AVOIDANCE_ANGLE, MAX_SPEED);
		}
		else if(collision_states & FRONT_LEFT_IR_MASK)
		{
			move_rotate(COLLISION_AVOIDANCE_ANGLE,-MAX_SPEED);
		}
		restore_all_leds_states();
	}
	if(direction == DIR_BACKWARD && (collision_states & BACK_IR_MASK))
	{
		set_leds_from_byte(mask_modification(collision_states & BACK_IR_MASK));
		if(collision_states & BACK_RIGHT_IR_MASK)
		{
			move_rotate(COLLISION_AVOIDANCE_ANGLE, -MAX_SPEED);
		}
		else if(collision_states & BACK_LEFT_IR_MASK)
		{
			move_rotate(COLLISION_AVOIDANCE_ANGLE, MAX_SPEED);
		}
		restore_all_leds_states();
	}
}

//Assigns the collision states to the leds next to each IR proximity sensor

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

//Inits sound and ir modules in order for motor_sequences to work

void ir_and_sound_init()
{
	collision_detection_start();
	sound_module_start();
}

void leds_reset()
{
	clear_top_leds();
}


//--------- END OF FILE ---------
