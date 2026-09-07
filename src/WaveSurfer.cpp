#include "WaveSurfer.h"

WaveSurfer* pulse_skipper;

//Empty constructor
WaveSurfer::WaveSurfer(){};


//Not so empty constructor
WaveSurfer::WaveSurfer(uint8_t ZC_pin, uint8_t PSM_pin)
{
  pulse_skipper = this;
  this->ZC_pin = ZC_pin;
  this->PSM_pin = PSM_pin;
}


//Not so empty constructor, specify mains frequency
WaveSurfer::WaveSurfer(uint8_t ZC_pin, uint8_t PSM_pin, uint8_t freq)
{
  pulse_skipper = this;
  this->ZC_pin = ZC_pin;
  this->PSM_pin = PSM_pin;
  this->freq = freq;
}
  

//Begin listening for zero-crossings and controlling dimmer
void WaveSurfer::begin()
{
  pinMode(PSM_pin, OUTPUT);
  uint8_t ZC = digitalPinToInterrupt(ZC_pin);
  if(ZC != -1)
  {
    attachInterrupt(ZC, on_ZC, RISING);
    if(Serial) {Serial.println("PSM interrupt set.");}
  }
  else 
  {
    if(Serial)
    {
      char buffer[40];
      sprintf(buffer, "Interrupt on pin %02d is not available.", ZC);
      Serial.println(buffer);
    }
  }
}


//Detach interrupt, end control, and reset variables
void WaveSurfer::end()
{
  detachInterrupt(ZC_pin);
  digitalWrite(PSM_pin, LOW);
  
  set_Power(0);
  skip_rate = 0;
  num_skipped = 1;
  num_fired = 0;
  curr_skipping = true;
}


//Set precentage of full-waves allowed to pass
void WaveSurfer::set_Power(uint8_t pwr)
{
  if(pwr < 1)
  {
    pwr = 0;
    skip_rate = 1;
  }

  else if(pwr > 99)
  {
    pwr = 100;
    skip_rate = 0;
  }

  else
  {
    skip_rate = 1 - (power / 100.0);
  }
  
  power = pwr;
}


//Returns percentage of full-waves allowed to pass
uint8_t WaveSurfer::get_Power()
{
  return pulse_skipper->power;
}


//Returns true when zero crossings commensurate with the set frequency are detected
long WaveSurfer::AC_On()
{
  return (millis() - pulse_skipper->last_ZC) < 1000.0/pulse_skipper->freq; //If the last zero-crossing detected was within twice the expected window
}

//Triggered on interrupt attached to zero crossing pulse from dimmer.
//Attempts to achieve requested power level.
void WaveSurfer::on_ZC()
{
  pulse_skipper->last_ZC = millis();
  if(pulse_skipper->curr_skipping)
  {
    pulse_skipper->num_skipped += 1;
    if(pulse_skipper->num_skipped % 2 == 0 && 1.0 * (pulse_skipper->num_fired + 1) / (pulse_skipper->num_skipped + pulse_skipper->num_fired) <= (1 - pulse_skipper->skip_rate))
    {
      digitalWrite(pulse_skipper->PSM_pin, HIGH);
      pulse_skipper->curr_skipping = false;
    }
  }
  else
  {
    pulse_skipper->num_fired += 1;
    if(pulse_skipper->num_fired % 2 == 0 && 1.0 * (pulse_skipper->num_skipped) / (pulse_skipper->num_skipped + pulse_skipper->num_fired) <= pulse_skipper->skip_rate)
    {
      digitalWrite(pulse_skipper->PSM_pin, LOW);
      pulse_skipper->curr_skipping = true;
      pulse_skipper->num_skipped = 0;
      pulse_skipper->num_fired = 0;
    }
  }
}
