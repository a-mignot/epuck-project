//Filename 		: sound_module.c
//Description  	: this module takes the input of the microphones and processes
//				  it to find which pitch is played
//Authors 		: Antonin Mignot, David Junqueira
//Version	 	: 1

//--------- INCLUDES --------

#include <sound_module.h>

#include <arm_math.h>
#include <arm_const_structs.h>
#include <audio/microphone.h>


//#define DEBUG

#ifdef DEBUG
#include <debug.h>
#include <chprintf.h>
#endif


//--------- DEFINES ---------
#define FFT_SIZE 	1024

#define MIN_VALUE_THRESHOLD	10000

//we can obtain the FREQ_x position of the corresponding frequency in the buffer
//by dividing the frequency of the pitch by 15.625 which is the frequency resolution

//motors frequency appear to be near C frequency which creates a functionality problem

#define MIN_FREQ	35	//we don't analyze before this index to not use resources for nothing
#define FREQ_D		38	//587.33 Hz	(D5)
#define FREQ_E		42	//659.25 Hz (E5)
#define FREQ_F		45	//698.46 Hz (F5)
#define FREQ_G		50	//783.99 Hz (G5)
#define FREQ_A		57	//880.00 Hz (A5) the index was manually increased from 56 to 57
#define FREQ_B		65	//987.77 Hz (B5) the index was manually increased from 63 to 65
#define MAX_FREQ	67	//we don't analyze after this index to not use resources for nothing


//10 samples gives a good safety regarding noise but slows the reaction time

#define SAMPLE_SIZE 10

//--------- STATIC VARIABLES ---------


static float micBack_cmplx_input[2 * FFT_SIZE];
static float micBack_output[FFT_SIZE];

static uint8_t pitch_changed = PITCH_UNCHANGED; //value is 0 if pitch didn't change since last sampling, 1 if it did
static pitch current_pitch = PITCH_ERR; //default value for "no pitch in range received"
static pitch last_notes[SAMPLE_SIZE]; //array containing last received notes

//----INTERNAL FUNCTIONS DECLARATION----

void processAudioData(int16_t *data, uint16_t num_samples);
pitch pitch_finder(float* data);
uint8_t check_all_same(void);
void add_note(pitch pitch_to_add);


//--------- FUNCTIONS ---------

void processAudioData(int16_t *data, uint16_t num_samples)
{
	/*  We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.	*/
	static uint16_t nb_samples = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4)
	{
		//construct an array of complex numbers. Put 0 to the imaginary part
		micBack_cmplx_input[nb_samples] = (float)data[i + MIC_BACK];
		nb_samples++;
		micBack_cmplx_input[nb_samples] = 0;
		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE))
		{
			break;
		}
	}
	if(nb_samples >= (2 * FFT_SIZE))
	{
		//computes the FFT of the signal then the magnitude of complex FFT value
		arm_cfft_f32(&arm_cfft_sR_f32_len1024, micBack_cmplx_input, 0, 1);
		arm_cmplx_mag_f32(micBack_cmplx_input, micBack_output, FFT_SIZE);

		nb_samples = 0;

		//read current pitch and update pitch_changed and current_pitch depending on receiver information

		pitch output = pitch_finder(micBack_output);
		add_note(output);

		// checks if pitch has changed and if the note has been played enough time for a sequence change
		if((output != current_pitch) && (check_all_same()) && (output != PITCH_ERR))
		{
			current_pitch = output;
			pitch_changed = PITCH_CHANGED;
		}

#ifdef DEBUG
		char note = freqToPitchName(output);
		chprintf((BaseSequentialStream *)&SD3,"%note:%-c \r\n",note);
#endif

	}
}

//Checks what frequency was sampled by extracting the FFt index with the
//highest absolute value and returns the corresponding pitch

pitch pitch_finder(float* data)
{
	float max_norm = MIN_VALUE_THRESHOLD;
	int16_t max_norm_index = -1;

	//search for the highest peak
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++)
	{
		if(data[i] > max_norm)
		{
			max_norm = data[i];
			max_norm_index = i;
		}
	}
	if(max_norm_index >= (FREQ_D-1) && max_norm_index <= (FREQ_D+1)) return PITCH_D;
	if(max_norm_index >= (FREQ_E-1) && max_norm_index <= (FREQ_E+1)) return PITCH_E;
	if(max_norm_index >= (FREQ_F-1) && max_norm_index <= (FREQ_F+1)) return PITCH_F;
	if(max_norm_index >= (FREQ_G-1) && max_norm_index <= (FREQ_G+1)) return PITCH_G;
	if(max_norm_index >= (FREQ_A-1) && max_norm_index <= (FREQ_A+1)) return PITCH_A;
	if(max_norm_index >= (FREQ_B-1) && max_norm_index <= (FREQ_B+1)) return PITCH_B;
	else return PITCH_ERR;

}

//adds a note to an array of the last sampled notes.
//last_notes works like a circular buffer

void add_note(pitch pitch_to_add)
{
	static uint8_t position = 0;
	last_notes[position] = pitch_to_add;
	if(position == (SAMPLE_SIZE-1))
	{
		position = 0;
	}
	else
	{
		position += 1;
	}
}


//checks if the same note has been sampled a certain number of times, this allows to get rid of some
//unexpected reactions due to noise

uint8_t check_all_same()
{
	pitch tested_pitch = last_notes[0];
	for(uint8_t i = 1; i < SAMPLE_SIZE ; i++)
	{
		if(last_notes[i] != tested_pitch)
		{
			return 0;
		}
	}
	return 1;
}

void sound_module_start()
{
	//inits the microphone and the callback function
	mic_start(&processAudioData);
}


//getters and setters for the module static variables that are needed externally


int get_pitch_changed()
{
	return pitch_changed;
}

void set_pitch_changed(uint8_t new_state)
{
	pitch_changed = new_state;
}

int get_current_pitch()
{
	return current_pitch;
}

//--------- END OF FILE ---------
