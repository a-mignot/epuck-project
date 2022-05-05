//Filename 		: main.c
//Description  	: contains the main program
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1

//--------- INCLUDES --------

// C-BASED LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// E-PUCK LIBRARIES
#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>
#include "sensors/proximity.h"

// PROJECT MODULES
#include <main.h>
#include <ir_detection_module.h>
#include <leds_animation.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

//--------- FUNCTIONS ---------

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}


int main(void)
{
	// Hardware initialization
    halInit();
    chSysInit();
    mpu_init();
    leds_animation_start();
    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //inits the motors
    //motors_init();
    //proximity_start();
    //collision_detection_start();

    messagebus_init(&bus, &bus_lock, &bus_condvar);
    set_leds_from_byte(21);
    //default infinite loop
    while(1){
    	//round_led_spin(100,0,0);
    	chThdSleepMilliseconds(100);
    }
}

//--------- STACK PROTECTOR ---------


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

//--------- END OF FILE ---------
