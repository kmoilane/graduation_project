#if !defined(COOLER)
#define COOLER

#include "utils.hpp"
#include "config.hpp"
#include "random_between.hpp"
#include "units.hpp"
#include <iostream>


namespace devices
{

class Cooler
{
public:
    bool state   {false};

    Cooler(bool initial_state);

    // set device on/off
    void set_state(bool new_state);

    // get device state
    bool get_state() const ;
     
    // update device to match current time
    void update(milliseconds time_step);

    // apply configuration to device
    void configure(Configuration &config);

    // convert cooling power to celsius
    celsius power_to_celsius(watts power) const;

    // temperature getter
    celsius get_temperature() const;

    // power getter
    celsius get_power() const;


private:

    // generates a random breaking point when on max power
    int generate_rand_breakdown_point() const;

    int breakdown_time_ms           {60'000};
    int random_breakdown_point_ms   {generate_rand_breakdown_point()};
    int duration_on_max_power_ms    {0};
    const watts POWER_MIN           {0};
    int step_time_ms                {100};

    // configurable
    watts power_max                 {500};
    watts power_increase_1ms        {2.5 / 1000.0};
    watts power_decrease_1ms        {1.25 / 1000.0};
    celsius temperature_min         {-10};

    watts power_current             {0};
    celsius temperature_current     {0};
    bool is_functioning             {true};

};

Cooler::Cooler(bool initial_state){ set_state(initial_state);}

void Cooler::set_state(bool new_state){ state = new_state && is_functioning;}
bool Cooler::get_state() const { return state;}

void Cooler::update(milliseconds time_step = -1){

    // use default time step if argument is not set
    time_step = (time_step < 0) ? step_time_ms : time_step;

    watts new_power {power_current};

    // duration 
    if(state == true){
        new_power += time_step * power_increase_1ms;
    }
    else{
        new_power -= time_step * power_decrease_1ms;
    }

    // power remained at max level
    if ((power_current >= power_max) && (new_power >= power_max)){
        duration_on_max_power_ms += time_step; 
    }
    // power dropped below max level
    else if((power_current >= power_max) && (new_power < power_max)){
        duration_on_max_power_ms = 0;
        random_breakdown_point_ms = generate_rand_breakdown_point();
    }

    // there is a chance of element breakdown
    if(is_functioning && (duration_on_max_power_ms > random_breakdown_point_ms)){
        state = is_functioning = false;
    }

    power_current = std::clamp(new_power, POWER_MIN, power_max);
    temperature_current = power_to_celsius(power_current);
}

void Cooler::configure(Configuration &config)
{
    auto config_data = config.data["Simulation"]["Cooler"];
    power_max = get_from_json(config_data["max_power"], power_max);

    // get step time from configuration
    step_time_ms = get_from_json(config.data["Simulation"]["step_time_ms"], step_time_ms);

    // get init state from configuration
    state = get_from_json(config_data["state"], state);
}

celsius Cooler::power_to_celsius(watts power) const{
    return (power/ power_max) * temperature_min;
}

celsius Cooler::get_temperature() const{
    return temperature_current;
}

celsius Cooler::get_power() const{
    return power_current;
}

int Cooler::generate_rand_breakdown_point() const{
    return rand_between::rand_between(0, breakdown_time_ms);
}

} // namespace devices

#endif