#if !defined(AMBIENT_TEMPERATURE)
#define AMBIENT_TEMPERATURE

#include "units.hpp"
#include "random_between.hpp"
#include "config.hpp"
#include "utils.hpp"


class AmbientTemperature
{

public:
    
    AmbientTemperature(celsius initial_temperature);

    // simulates 'natural' ambient temperature change over time
    void update(milliseconds time_step);

    // temperature getter
    celsius get_temperature() const;

    // configure values
    void configure(Configuration& config);

private:
    milliseconds step_time_ms               {100};
    celsius temperature_avg                 {20};
    celsius current_temperature             {temperature_avg};
    const celsius temperature_change_1s     {0.01};
    const celsius max_offset                {5};
    celsius target_temp                     {generate_target_temperature()};
    celsius step_value                      {get_step_value()};

    // generates new target temperature
    celsius generate_target_temperature() const;

    // computes new step value to reach set target temperature
    celsius get_step_value() const;


};


AmbientTemperature::AmbientTemperature(celsius temperature_avg)
                     : temperature_avg(temperature_avg) {}

celsius AmbientTemperature::generate_target_temperature() const{
    return rand_between::rand_between(temperature_avg - max_offset,
                                        temperature_avg + max_offset);
}

celsius AmbientTemperature::get_step_value() const{
    return (target_temp > current_temperature) ? temperature_change_1s : -temperature_change_1s;
}

void AmbientTemperature::update(milliseconds time_step){

    // use default time step if argument is not set
    time_step = (time_step < 0) ? step_time_ms : time_step;
    current_temperature += (time_step / 1000.0) * step_value;

    // set new target if old target has been reached
    if ((step_value > 0 && current_temperature >= target_temp) || (step_value < 0 && current_temperature <= target_temp)){
        target_temp = generate_target_temperature();
        step_value = get_step_value();
    }
}

celsius AmbientTemperature::get_temperature() const{
    return current_temperature;
}

void AmbientTemperature::configure(Configuration& config){
    auto config_data = config.data["Simulation"];
    temperature_avg = get_from_json(config_data["Ambient_temperature"]["ambient_temperature"],
                                    temperature_avg);
                    
    step_time_ms = get_from_json(config_data["step_time_ms"],
                                 step_time_ms);
}

#endif // AMBIENT_TEMP
