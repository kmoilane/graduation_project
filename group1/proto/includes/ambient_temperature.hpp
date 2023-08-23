#if !defined(AMBIENT_TEMPERATURE)
#define AMBIENT_TEMPERATURE

#include "units.hpp"
#include "random_between.hpp"
#include <chrono>


class AmbientTemperature
{
private:
    const celsius initial_temperature {20};
    const celsius update_value {0.2};
    const celsius max_offset {5};

    celsius target_temp {generate_target_temperature()};
    celsius step_value {get_step_value()};

public:
    celsius current_temperature {initial_temperature};

    AmbientTemperature(celsius initial_temperature) : initial_temperature(initial_temperature) {}
    ~AmbientTemperature() {}

    // generates new target temperature
    celsius generate_target_temperature(){
        return rand_between::rand_between(initial_temperature - max_offset,
                                          initial_temperature + max_offset);
    }

    // computes new step value to reach set target temperature
    celsius get_step_value(){
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

    celsius get_temperature(){
        return current_temperature;
    }

};


#endif // AMBIENT_TEMP
