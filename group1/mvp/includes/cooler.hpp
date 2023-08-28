#if !defined(COOLER)
#define COOLER

#include "config.hpp"
#include "random_between.hpp"
#include "units.hpp"
#include <bitset>
#include <iostream>

namespace devices
{
    

class Cooler
{
public:
    bool state   {false};

    Cooler(bool initial_state){
        set_state(initial_state);
    }

    void set_state(bool new_state){
        state = new_state && is_functioning;
    }

    void update(){ 

        watts new_power {power_current};

        // duration 
        if(state == true){
            new_power += (step_time_ms / 1000.0) * power_increase_1s;
        }
        else{
            new_power -= (step_time_ms / 1000.0) * power_decrease_1s;
        }

        // power remained at max level
        if ((power_current >= POWER_MAX) && (new_power >= POWER_MAX)){
            duration_on_max_power_ms += step_time_ms; 
        }
        // power dropped below max level
        else if((power_current >= POWER_MAX) && (new_power < POWER_MAX)){
            duration_on_max_power_ms = 0;
            random_breakdown_point_ms = generate_rand_breadown_point();
        }

        // there is a chance of element breakdown
        if(is_functioning && (duration_on_max_power_ms > random_breakdown_point_ms)){
            state = is_functioning = false;
        }

        power_current = std::clamp(new_power, POWER_MIN, POWER_MAX);
        temperature_current = power_to_celsius(power_current);
    }

    void configure(Configuration &config)
    {
        auto tmp_power_max = config.data["Simulation"]["Cooler"]["max_power"];
        auto tmp_probability = config.data["Simulation"]["Cooler"]["break_probability"];
        auto tmp_state = config.data["Simulation"]["Cooler"]["state"];
        auto tmp_step_time = config.data["Simulation"]["step_time_ms"];

        if(!tmp_power_max.is_null()){
            if(tmp_power_max > 0 && tmp_power_max < 1000){
                POWER_MAX = config.data["Simulation"]["Cooler"]["max_power"];
            }
            else{
                throw std::invalid_argument("Power max has to be between 0-1000");
            }
        }  
        if(!tmp_probability.is_null()){
            if(tmp_probability >= 0 && tmp_probability <= 100){
                break_probability = tmp_probability;
            }
            else{
                throw std::invalid_argument("Probability has to be between 0-100 (set in configuration file)");
            }
        } 
        
        // get step time from configuration
        step_time_ms = tmp_step_time.is_null() ? 100 : int(tmp_step_time);
        // get init state from configuration
        state = tmp_state.is_null() ? false : bool(tmp_state);
    }

    celsius power_to_celsius(watts power){
        return (power/ POWER_MAX) * temperature_min;
    }

    celsius get_temperature(){
        return temperature_current;
    }

    celsius get_power(){
        return power_current;
    }

    int generate_rand_breadown_point(){
        return rand_between::rand_between(0, breakdown_time_ms);
    }

private:
    int breakdown_time_ms           {60'000};;
    int random_breakdown_point_ms   {generate_rand_breadown_point()};
    int duration_on_max_power_ms    {0};
    const watts POWER_MIN           {0};
    int step_time_ms                {100};

    // configurable
    watts POWER_MAX                 {500};
    watts power_increase_1s         {2.5};
    watts power_decrease_1s         {1.2};
    double break_probability        {0.0001};
    celsius temperature_min         {-10};

    watts power_current             {0};
    celsius temperature_current     {0};
    bool is_functioning             {true};

};

} // namespace devices

#endif
