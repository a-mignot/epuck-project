//Filename 		: ir_detection_module.c
//Description  	: This module contains collision detection functions determined by configured thresholds
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1

//--------- INCLUDES --------

#include "ir_detection_module.h"
#include <ch.h>
#include <msgbus/messagebus.h>
#include "hal.h"

//#define DEBUG

#ifdef DEBUG
#include <debug.h>
#include <chprintf.h>
#endif
//--------- DEFINES --------

#define DEFAULT_IR_TRESHOLD 100

//Front sensors
#define IR0_THRESHOLD DEFAULT_IR_TRESHOLD
#define IR7_THRESHOLD IR0_THRESHOLD

//Front 45deg sensors
#define IR1_THRESHOLD DEFAULT_IR_TRESHOLD
#define IR6_THRESHOLD IR1_THRESHOLD

//Side sensors
#define IR2_THRESHOLD DEFAULT_IR_TRESHOLD
#define IR5_THRESHOLD IR2_THRESHOLD

//Back sensors
#define IR3_THRESHOLD DEFAULT_IR_TRESHOLD
#define IR4_THRESHOLD IR3_THRESHOLD

extern messagebus_t bus;

//--------- FUNCTIONS ---------


/* input : proximity sensors values
 * output: 8 bits binary number.
 *
 * A 1 means that there is a collision detected on
 * the sensor associated to the rank of the bit
 *
 * A 0 means that there is no collision detected.
 */


uint8_t collision_detection(proximity_msg_t *prox_values){
	uint8_t collision_state = 0;


	//creating a static table to make a more compact function
	static const int threshold_table[PROXIMITY_NB_CHANNELS] = {IR0_THRESHOLD, IR1_THRESHOLD,
															   IR2_THRESHOLD, IR3_THRESHOLD,
															   IR4_THRESHOLD, IR5_THRESHOLD,
															   IR6_THRESHOLD, IR7_THRESHOLD};


	for(int i=0;i<PROXIMITY_NB_CHANNELS;i++){
		//the calibrated value can be negative so we cast the delta and initValues from uint to int
		int calibrated_value = (int)prox_values->delta[i] - (int)prox_values->initValue[i];

		if(calibrated_value >= threshold_table[i]){
			collision_state = (collision_state | (1 << i));
		}
	}

	return collision_state;
}

static THD_FUNCTION(collision_detection_thd, arg){
	(void) arg;
	chRegSetThreadName(__FUNCTION__);

	systime_t time;

	messagebus_topic_t *prox_topic = messagebus_find_topic_blocking(&bus, "/proximity");
	proximity_msg_t prox_values;
	calibrate_ir();

	for(;;){
		time = chVTGetSystemTime();

		messagebus_topic_wait(prox_topic, &prox_values, sizeof(prox_values));


		uint8_t collision_states = collision_detection(&prox_values);

#ifdef DEBUG
		//chprintf((BaseSequentialStream *)&SD3,"%Proximity=%-7d Calib. Proximity=%-7d Ambient light=%-7d \r\n"
				//,prox_values.delta[2],prox_values.delta[2]-prox_values.initValue[2],prox_values.ambient[2]);
		chprintf((BaseSequentialStream *)&SD3,toBinary(collision_states,8));
#endif

		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}

}

void collision_detection_start(){
	 static THD_WORKING_AREA(collision_detection_thd_wa, 1024);
	 chThdCreateStatic(collision_detection_thd_wa, sizeof(collision_detection_thd_wa), NORMALPRIO, collision_detection_thd, NULL);
}





//--------- END OF FILE ---------
