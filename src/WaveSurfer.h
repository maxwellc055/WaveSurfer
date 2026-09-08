/*
Thank you to banoz and the PSM.Library at <https://github.com/banoz/PSM.Library>
which I used to figure out how to use a pointer to create a library like this one
*/


#ifndef WAVESURFER_H
#define WAVESURFER_H
#endif

#include <Arduino.h>

class WaveSurfer
{
  public:
    WaveSurfer(); //Empty constructor
    WaveSurfer(uint8_t ZC_pin, uint8_t PSM_pin); //Not so empty constructor
    WaveSurfer(uint8_t ZC_pin, uint8_t PSM_pin, uint8_t freq); //Not so empty constructor, specify mains frequency
    void begin(); //Begin listening for zero-crossings and controlling dimmer
    void end(); //Detach interrupt, end control, and reset variables
    void set_Power(uint8_t pwr); //Set precentage of full-waves allowed to pass
    uint8_t get_Power(); //Returns percentage of full-waves allowed to pass
    long AC_On(); //Returns true when zero crossings commensurate with the set frequency are detected


  private:
    static void on_ZC(); //Triggered on interrupt attached to zero crossing pulse from dimmer. Attempts to achieve requested power level.
    uint8_t power; //Percentage (%) of full-waves allowed to pass
    volatile float skip_rate; //Proportion (not %) of full-waves to be skipped
    volatile uint8_t num_skipped = 1; //Number of full-waves skipped
    volatile uint8_t num_fired = 0; //Number of full-waves let fire
    volatile bool curr_skipping = true; //True when the power is blocking power
    uint8_t ZC_pin;
    uint8_t PSM_pin;
    volatile long last_ZC; //Time of most recently observed zero crossing
    uint8_t freq = 60; //Hz
};

extern WaveSurfer* pulse_skipper;
