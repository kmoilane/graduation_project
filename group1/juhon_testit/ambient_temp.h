#if !defined(AMBIENT_TEMP)
#define AMBIENT_TEMP

#include "units.h"
#include "random_between.h"
#include <chrono>


class AmbientTemperature
{
private:
    const celcius initial_temperature {20};
    const celcius update_value {0.2};
    const celcius max_offset {5};

    celcius target_temp {generate_target_temperature()};
    celcius step_value {get_step_value()};

public:
    celcius current_temperature {initial_temperature};

    AmbientTemperature(celcius initial_temperature) : initial_temperature(initial_temperature) {}
    ~AmbientTemperature() {}

    // generates new target temperature
    celcius generate_target_temperature(){
        return rand_between::rand_between(initial_temperature - max_offset,
                                          initial_temperature + max_offset);
    }

    // computes new step value to reach set target temperature
    celcius get_step_value(){
        return (target_temp > current_temperature) ? update_value : -update_value;
    }

    // simulates 'natural' ambient temperature change over time
    void update(){

        // calculate update
        current_temperature += step_value;

        // set new target if old target has been reached
        if ((step_value > 0 && current_temperature >= target_temp) || (step_value < 0 && current_temperature <= target_temp)){
            target_temp = generate_target_temperature();
            step_value = get_step_value();
        }
    }
};


#endif // AMBIENT_TEMP
