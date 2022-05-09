//Filename 		: sound_module.h
//Description  	: Header of sound_module.c
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1


#ifndef SOUND_MODULE_H
#define SOUND_MODULE_H

//--------- INCLUDES --------
#include <stdint.h>

//--------- DEFINES ---------

#define FFT_SIZE 	1024
#define PITCH_UNCHANGED 0
#define PITCH_CHANGED 1

//--------- TYPEDEFS/STRUCT/ENUM ---------

typedef enum pitch{
				   PITCH_C,
				   PITCH_D,
				   PITCH_E,
				   PITCH_F,
				   PITCH_G,
				   PITCH_A,
				   PITCH_B,
				   PITCH_ERR
} pitch;

//--------- PROTOTYPES ---------

void doFFT_optimized(uint16_t size, float* complex_buffer);
void processAudioData(int16_t *data, uint16_t num_samples);
pitch pitch_finder(float* data);

void set_pitch_changed(uint8_t new_state);
int get_pitch_changed(void);
int get_current_pitch(void);


#endif

//--------- END OF FILE ---------
