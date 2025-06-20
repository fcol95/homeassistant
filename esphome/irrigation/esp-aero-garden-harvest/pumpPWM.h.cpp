#include "esphome.h"
#include "pwmWrite.h"
using namespace esphome;

#define PUMP_PWM_A 32
#define PUMP_PWM_B 33

Pwm pwm = Pwm();
const char TAG[]="PUMP_PWM";
class AeroGardenPumpOutput : public Component, public BinaryOutput {
    public:
        bool runState=false;
        void setup() override {
            ESP_LOGI(TAG,"Started PWM Control.");
            //https://github.com/Dlloydev/ESP32-ESP32S2-AnalogWrite
            pwm.pause();

            //The following is done in the write_state function.
            //For the H-Bridge to truly turn off, both outputs need
            // to be 0 or both 1.
            //The inverted pin needs to be detached and reattached as non-inverted
            //for its output to go to 0 on pause or else it will go to 1.
            //So, I just made it detach and reattach both pins on both
            // on/off branches of write_state for simplicity and so that it's clear.

            //No longer need below
            //Attach to pwm channel sharing timer 2
            // pwm.attach(PUMP_PWM_A, 4, false);   //pin 32, channel 4, invert=false
            // pwm.attach(PUMP_PWM_B, 5, true);    //pin 33, channel 5, invert=true
            // pwm.write(PUMP_PWM_A, 128, 60);     //Set pin 32 to 50% duty, 60hz
            // pwm.write(PUMP_PWM_B, 128, 60);
        }
        bool state(){return runState;}
        void loop() override {
        }

        void write_state(bool state) override {
            if(!runState && state){
                ESP_LOGI(TAG,"Turning PWM ON.");
                 //resume pins 32 & 33 to 50% duty, 60hz
                pwm.detach(PUMP_PWM_A);    //pin 32, channel 4, invert=false
                pwm.detach(PUMP_PWM_B);    //pin 33, channel 5, invert=true
                pwm.attach(PUMP_PWM_A, 4, false);   //pin 32, channel 4, invert=false
                pwm.attach(PUMP_PWM_B, 5, true);    //pin 33, channel 5, invert=true
                pwm.write(PUMP_PWM_A, 128, 60); 
                pwm.write(PUMP_PWM_B, 128, 60);
                pwm.resume(4); //Resume channel 4
                pwm.resume(5); //Resume channel 5
            } else if (runState) {
                ESP_LOGI(TAG,"Turning PWM OFF.");

                //Roundabout way - detach and reattach without inverted pin
                pwm.detach(PUMP_PWM_A);
                pwm.detach(PUMP_PWM_B);
                pwm.attach(PUMP_PWM_A, 4, false);
                pwm.attach(PUMP_PWM_B, 5, false);
                pwm.write(PUMP_PWM_A, 0, 60);
                pwm.write(PUMP_PWM_B, 0, 60);
                pwm.pause(4); //Pause channel 4
                pwm.pause(5); //Pause channel 5
            }
            runState = state;
        }
};
