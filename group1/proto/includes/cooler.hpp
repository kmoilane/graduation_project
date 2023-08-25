#if !defined(COOLER)
#define COOLER

#include "config.hpp"
#include "random_between.hpp"
#include "units.hpp"
#include <bitset>
#include <chrono>

#include <iostream>

class Cooler
{
public:
    uint8_t state{};
    Cooler(uint8_t initial_state)
    {
        state = initial_state;
    }

    void set_state(uint8_t new_state)
    {
        if(is_functioning)
        {
            if(new_state != state)
            {
                last_update_time = std::chrono::high_resolution_clock::now();
                state = new_state;
            }
        }
    }

    void update()
    { 
        time_stamp end = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - last_update_time).count();
        
        watts new_power {power_current};

        if(state == ON)
        {
            // duration 
            new_power += (duration / 1000.0) * power_increase_1s;
        }
        else
        {
            new_power -= (duration / 1000.0) * power_decrease_1s;
        }

        if(rand_between::rand_between(0.0, 100.0) < break_probability)
        {
           state = false;
           is_functioning = false;
        }

        power_current = std::clamp(new_power, POWER_MIN, power_max);
        last_update_time = end;
        temperature_current = power_to_celsius(power_current);
        //std::cout << "last update time: " << "\n";
        //std::cout << "current power: " << power_current << "\n";
        //std::cout << "current temp: " << temperature_current << "\n";
    }

    void configure(Configuration &config)
    {
        auto tmp_power_max = config.data["Simulation"]["Cooler"]["max_power"];
        auto tmp_probability = config.data["Simulation"]["Cooler"]["break_probability"];
        if(!tmp_power_max.is_null())
        {
            if(tmp_power_max > 0 && tmp_power_max < 1000)
            {
                power_max = config.data["Simulation"]["Cooler"]["max_power"];
            }
            else{
                throw std::invalid_argument("Power max has to be between 0-1000");
            }
        }  
        if(!tmp_probability.is_null())
        {
            if(tmp_probability >= 0 && tmp_probability <= 100)
            {
                break_probability = tmp_probability;
            }
            else
            {
                throw std::invalid_argument("Probability has to be between 0-100 (set in configuration file)");
            }
        }  

    }

    celsius power_to_celsius(watts power){
        //std::cout << "power_to_celsius: " << power << "\n";
        return (power/ power_max) * temperature_min;
    }

    celsius get_temperature()
    {
        //std::cout << "cooler: " << temperature_current << "\n";
        return temperature_current;
    }

private:
    const watts POWER_MIN{0};
    const uint8_t ON = 0b00000001;
    const uint8_t OFF = 0b00000000;
    // configurable
    watts power_max{500};
    watts power_increase_1s {2.5};
    watts power_decrease_1s {1.2};
    double break_probability{0.0001};
    celsius temperature_min {-10};

    time_stamp last_update_time{};
    watts power_current{0};
    celsius temperature_current{0};
    bool is_functioning{true};


};

#endif