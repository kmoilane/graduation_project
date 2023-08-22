#if !defined(HEATER)
#define HEATER

#include "units.h"
#include <array>
#include <chrono>
#include <algorithm>
#include <bitset>


class HeaterElement
{

private:
    std::chrono::system_clock::time_point begin;
    const seconds heater_breakdown_time {10};
    const watts heater_power_increase_1s {10};
    const watts heater_power_decrease_1s {5};
    const watts heater_power_min {0};
    const watts heater_power_max {2000};

public:

    bool state {false};
    watts power_current {0};

    HeaterElement(bool initial_state) : state(initial_state) {}
    ~HeaterElement() {}

    // new control signal received
    void set_state(bool new_state){
        state = new_state;
        begin = std::chrono::high_resolution_clock::now();
    }

    // updates element's power level
    void update(){

        auto end = std::chrono::high_resolution_clock::now();
        int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        if(state == true){
            power_current += (duration / 1000.0) * heater_power_increase_1s;
        }
        else{
            power_current -= (duration / 1000.0) * heater_power_decrease_1s;
        }

        power_current = std::clamp(power_current, heater_power_min, heater_power_max);
        begin = end;
    }
};


class HeaterUnits
{

private:
    std::array<HeaterElement, 3> elements {0,0,0};

public:
    HeaterUnits(int8_t initial_states) {}
    ~HeaterUnits() {}

    // update power levels of all elements
    void update(){
        for (auto &&element : elements){
            element.update();
        }
    }

    // set all states for heating elements
    void set_states(int8_t states){
        std::bitset<sizeof(states) * 8> bitrepr(states);
        for (size_t i = 0; i < elements.size(); i++){
            elements[i].set_state(bitrepr[i]);
        }
    }

    // get current power level of particular element
    watts get_power_level(size_t element_index){
        return elements[element_index].power_current;
    }

    // manually set power levels to elements
    template<size_t S = 0, typename T, typename... Args>
    void set_power_levels(T power, Args... args){
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>);
        static_assert(sizeof...(args) < std::tuple_size<decltype(elements)>::value);

        if constexpr(sizeof...(args) > 0){
            elements[S].power_current = power;
            set_power_levels<S+1>(args...);
        }
        elements[S].power_current = power;
    }

};






#endif // HEATER
