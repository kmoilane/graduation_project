#if !defined(HEATER)
#define HEATER

#include "units.hpp"
#include <array>
#include <chrono>
#include <algorithm>
#include <bitset>


class HeaterElement
{

private:
    time_stamp begin;
    const seconds heater_breakdown_time {10};
    const watts heater_power_increase_1s {10};
    const watts heater_power_decrease_1s {5};
    const watts heater_power_min {0};
    const watts heater_power_max {2000};

public:

    bool state {false};
    watts power {0};

    HeaterElement(bool initial_state) : state(initial_state) {}
    ~HeaterElement() {}

    // new control signal received
    void set_state(bool new_state){
        state = new_state;
        begin = std::chrono::high_resolution_clock::now();
    }

    // updates element's power level
    void update(){

        time_stamp end = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        if(state == true){
            power += (duration / 1000.0) * heater_power_increase_1s;
        }
        else{
            power -= (duration / 1000.0) * heater_power_decrease_1s;
        }

        power = std::clamp(power, heater_power_min, heater_power_max);
        begin = end;
    }
};


class HeaterUnits
{

private:
    const watts total_power {6000};
    const celsius heater_temperature_max {100}; 
    std::array<HeaterElement, 3> elements {0,0,0};
    watts power_level {0};
    celsius temperature {0};

public:

    HeaterUnits(uint8_t initial_states) {
        set(initial_states);
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
    void set(uint8_t states){
        std::bitset<sizeof(states) * 8> bitrepr(states);
        for (size_t i = 0; i < elements.size(); i++){
            elements[i].set_state(bitrepr[i]);
        }
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

    celsius power_to_celsius(watts power){
        return (power / total_power) * heater_temperature_max;
    }

    celsius get_temperature(){
        return temperature;
    }

    celsius get_power(){
        return power_level;
    }

};


#endif // HEATER
