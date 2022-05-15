//Filename 		: main.c
//Description  	: contains the main program
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


//---------- DEFINES -----------
//#define DEBUG

//--------- INCLUDES --------

// E-PUCK LIBRARIES
#include "hal.h"
#ifdef DEBUG
#include <usbcfg.h>
#endif
#include "memory_protection.h"
#include "sensors/proximity.h"
#include "audio/microphone.h"
#include <motors.h>
#include <spi_comm.h>

// PROJECT MODULES
#include <main.h>
#include <command_module.h>

//---- GENERAL BUS DECLARATION ----
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

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
    //starts the USB communication
    usb_start();
	#endif

    //inits the motors
    motors_init();

    //inits the IR sensors and the message bus
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
