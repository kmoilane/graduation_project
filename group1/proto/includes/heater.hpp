#if !defined(HEATER)
#define HEATER

#include "units.hpp"
#include "config.hpp"
#include <array>
#include <algorithm>
#include <bitset>
#include <nlohmann/json.hpp>
#include "product.hpp"


class HeaterElement
{

private:
    const int breakdown_time_ms             {60'000};
    int random_breakdown_point_ms           {generate_rand_breadown_point()};
    int duration_on_max_power_ms            {0};
    const watts power_increase_1s           {10};
    const watts power_decrease_1s           {5};

public:
    int step_time_ms                        {100};
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
        state = new_state && is_functioning;
    }

    // updates element's power level
    void update(){

        watts new_power {power};
        if(state == true){
            new_power += (step_time_ms / 1000.0) * power_increase_1s;
        }
        else{
            new_power -= (step_time_ms / 1000.0) * power_decrease_1s;
        }
    
        // power remained at max level
        if ((power >= power_max) && (new_power >= power_max)){
            duration_on_max_power_ms += step_time_ms; 
        }
        // power dropped below max level
        else if((power >= power_max) && (new_power < power_max)){
            duration_on_max_power_ms = 0;
            random_breakdown_point_ms = generate_rand_breadown_point();
        }

        // there is a chance of element breakdown
        if(is_functioning && (duration_on_max_power_ms > random_breakdown_point_ms)){
            state = is_functioning = false;
        }

        // clamp power between 0 and max
        power = std::clamp(new_power, power_min, power_max);
    }

    int generate_rand_breadown_point(){
        return rand_between::rand_between(0, breakdown_time_ms);
    }
};

namespace devices
{

class Heater
{

private:
    std::array<HeaterElement, 3> elements   {0,0,0};
    celsius temperature_max                 {100};  
    const watts max_power                   {6000};
    celsius temperature                     {0};
    watts power_level                       {0};

public:

    Heater(uint8_t initial_states) {
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

    celsius power_to_celsius(watts power) const {
        return (power / max_power) * temperature_max;
    }

    celsius get_temperature() const {
        return temperature;
    }

    celsius get_power() const {
        return power_level;
    }

    void configure(Configuration &config){
        auto config_max_temp = config.data["Simulation"]["Heater"]["temperature_max"];
        if (!config_max_temp.is_null())
        {
            if (config_max_temp < 1){
                throw std::invalid_argument("Heater max temperature can't be below 1 (set in configuration file)");
            }
            temperature_max = config_max_temp;
        }

        // get step time from configuration
        auto config_step_time = config.data["Simulation"]["step_time_ms"];
        int step_time_ms = config_step_time.is_null() ? 100 : int(config_step_time);
        for (auto &&element : elements){
            element.step_time_ms = step_time_ms;
        }
        
    }

    // applies linear probability of success on power range (0 - 3000 w) 
    ProductState process(ProductState product_state) const {
        if (product_state == ProductState::good){
            double success_prob = (power_level - 3000.0) / (max_power - 3000.0);
            if (rand_between::rand_between(0.0, 1.0) > success_prob){
                return ProductState::bad;
            }
        }
        return product_state;
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

};

} // namespace devices

#endif // HEATER
