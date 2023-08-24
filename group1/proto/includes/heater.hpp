#if !defined(HEATER)
#define HEATER

#include "units.hpp"
#include "config.hpp"
#include <array>
#include <chrono>
#include <algorithm>
#include <bitset>
#include <iostream>

class HeaterElement
{

private:
    time_stamp last_update_time;
    int duration_on_max_power_ms            {0};
    const seconds breakdown_time            {60};
    const watts power_increase_1s           {10};
    const watts power_decrease_1s           {5};

public:
    const watts power_min                   {0};
    const watts power_max                   {2000};

    bool is_functioning                     {true};
    bool state                              {false};
    watts power                             {0};

    HeaterElement(bool initial_state) : state(initial_state) {}
    ~HeaterElement() {}

    // new control signal received
    // will not respond to activating commands if gets broken
    void set_state(bool new_state){
        if (is_functioning){
            state = new_state;
            last_update_time = std::chrono::high_resolution_clock::now();
        }
    }

    // updates element's power level
    void update(){

        // update power
        time_stamp end = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - last_update_time).count();

        watts new_power {power};
        if(state == true){
            new_power += (duration / 1000.0) * power_increase_1s;
        }
        else{
            new_power -= (duration / 1000.0) * power_decrease_1s;
        }
    
        // first time power reached max level
        if (power < power_max && new_power >= power_max){
            duration_on_max_power_ms = 0;
        }
        // power remained at max level
        else if (power >= power_max && new_power >= power_max){
            duration_on_max_power_ms += duration; 
        }
        // power drop below max level
        else {
            duration_on_max_power_ms = 0;
        }

        // there is a chance of element breakdown
        if(duration_on_max_power_ms > 0){
            double prob = rand_between::rand_between(0.0, 1.0);
            double prob2 = (duration_on_max_power_ms / double(breakdown_time * 1000.0));
            is_functioning = prob > prob2;

            if (!is_functioning){
                state = false;
            }
        }

        power = std::clamp(new_power, power_min, power_max);
        last_update_time = end;
    }
};


class HeaterUnits
{

private:
    std::array<HeaterElement, 3> elements   {0,0,0};
    celsius temperature_max                 {100};  
    const watts max_power                   {6000};
    celsius temperature                     {0};
    watts power_level                       {0};

public:

    HeaterUnits(uint8_t initial_states) {
        set_state(initial_states);
    }

    // update power levels of all elements
    void update(){
        watts new_power = 0;
        for (auto &&element : elements){
            element.update();
            new_power += element.power;
        }
        power_level = new_power;
        temperature = power_to_celsius(power_level);
    }

    // set all states for heating elements
    void set_state(uint8_t states){
        std::bitset<sizeof(states) * 8> bitrepr(states);
        for (size_t i = 0; i < elements.size(); i++){
            elements[i].set_state(bitrepr[i]);
        }
    }

    uint8_t get_state() const {
        std::bitset<sizeof(uint8_t) * 8> bitrepr;
        for (size_t i = 0; i < elements.size(); i++){
            bitrepr[i] = elements[i].state;
        }
        return uint8_t(bitrepr.to_ulong());
    }

    // manually set power levels to elements
    template<size_t S = 0, typename T, typename... Args>
    void force_power_levels(T power, Args... args){
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>);
        static_assert(sizeof...(args) < std::tuple_size<decltype(elements)>::value);

        if constexpr(sizeof...(args) > 0){
            elements[S].power = power;
            force_power_levels<S+1>(args...);
        }
        elements[S].power = power;
        update();
    }

    celsius power_to_celsius(watts power) const {
        return (power / max_power) * temperature_max;
    }

    celsius get_temperature() const {
        return temperature;
    }

    celsius get_power() const {
        return power_level;
    }

    bool configure(Configuration &config)
    {

        temperature_max = config.data["Simulation"]["Heater"]["temperature_max"];
        return true;
    }

    // applies linear probability of success on power range (0 - 3000 w) 
    int process(int product_state) const {
        if (product_state == 1){
            double success_prob = power_level / 3000.0;
            return rand_between::rand_between(0.0, 1.0) < success_prob;
        }
        return product_state;
    }

};


#endif // HEATER
