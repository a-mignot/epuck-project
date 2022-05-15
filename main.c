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
#include "sensors/proximity.h"
#include "audio/microphone.h"
#include <motors.h>
#include <spi_comm.h>

// PROJECT MODULES
#include <main.h>
#include <command_module.h>

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

//----------Defines-----------

//#define DEBUG

//--------- FUNCTIONS ---------

#ifdef DEBUG
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
#endif


int main(void)
{
	// Hardware initialization
    halInit();
    chSysInit();
    mpu_init();

	#ifdef DEBUG
    //starts the serial communication
    serial_start();
	#endif

    //starts the USB communication
//    usb_start();

    //inits the motors
    motors_init();

    //inits the IR sensors
    proximity_start();
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    //inits spi communication for rgb leds
    spi_comm_start();

    //inits the thread that triggers the different motor sequences
    command_start();

    //default infinite loop
    while(1)
    {
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
