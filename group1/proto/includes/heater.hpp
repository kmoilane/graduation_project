#if !defined(HEATER)
#define HEATER

#include "units.hpp"
#include "config.hpp"
#include "product.hpp"
#include "utils.hpp"
#include <array>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "random_between.hpp"


class HeaterElement
{

public:
    HeaterElement(bool initial_state);

    const watts power_max                   {2000};
    watts       power                       {0};
    bool        state                       {false};

    void        set_state(bool new_state);
    void        update(milliseconds time_step);

private:
    const int   breakdown_time_ms           {60'000};
    const watts power_increase_1ms          {10 / 1000.0};
    const watts power_decrease_1ms          {5 / 1000.0};
    const watts power_min                   {0};

    int         random_breakdown_point_ms   {generate_rand_breadown_point()};
    int         duration_on_max_power_ms    {0};
    bool        is_functioning              {true};

    int         generate_rand_breadown_point() const;

};

HeaterElement::HeaterElement(bool initial_state) : state(initial_state) {}

// new control signal received
// will not respond to activating commands if gets broken
void HeaterElement::set_state(bool new_state){
    state = new_state && is_functioning;
}

// updates element's power level
void HeaterElement::update(milliseconds time_step){

    watts new_power {power};
    if(state == true){
        new_power += time_step * power_increase_1ms;
    }
    else{
        new_power -= time_step * power_decrease_1ms;
    }

    // power remained at max level
    if ((power >= power_max) && (new_power >= power_max)){
        duration_on_max_power_ms += time_step; 
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

int HeaterElement::generate_rand_breadown_point() const{
    return rand_between::rand_between(0, breakdown_time_ms);
}


namespace devices
{

class Heater
{

public:

    Heater(uint8_t initial_states);

    void    update(milliseconds time_step);
    void    set_state(uint8_t states);
    uint8_t get_state() const;

    celsius power_to_celsius(watts power) const;
    celsius get_temperature() const;
    celsius get_power() const;

    void    configure(Configuration &config);

    ProductState process(ProductState product_state) const;

    // manually set power levels to elements
    template<size_t S = 0, typename T, typename... Args>
    void force_power_levels(T power, Args... args);

private:
    std::array<HeaterElement, 3> elements           {0,0,0};
    celsius                      temperature_max    {100};  
    const watts                  max_power          {6000};
    watts                        power_level        {0};
};


Heater::Heater(uint8_t initial_states) {
    set_state(initial_states);
}

// update power levels of all elements
void Heater::update(milliseconds time_step){

    watts new_power = 0;
    for (auto &&element : elements){
        element.update(time_step);
        new_power += element.power;
    }
    power_level = new_power;
}

 // set all states for heating elements
void Heater::set_state(uint8_t states){
    for (size_t i = 0; i < elements.size(); i++){
        elements[i].set_state(bool(states & (1 << i)));
    }
}

// state on/off getter
uint8_t Heater::get_state() const {
    uint8_t out {0};
    for (size_t i = 0; i < elements.size(); i++){
        out |= (elements[i].state << i);
    }

    return out;
}

// converts radiating power to celsius
celsius Heater::power_to_celsius(watts power) const {
    return (power / max_power) * temperature_max;
}

 // current temperature getter
celsius Heater::get_temperature() const {
    return power_to_celsius(power_level);;
}

// current power getter
celsius Heater::get_power() const {
    return power_level;
}

 // applies configuration
void Heater::configure(Configuration &config){
    auto config_data = config.data["Simulation"];
    temperature_max = get_from_json(config_data["Heater"]["temperature_max"], temperature_max);

    if (temperature_max < 1){
        throw std::invalid_argument("Heater max temperature can't be below 1 (set in configuration file)");
    }

    // set initial powers to elements
    auto powers = config_data["Heater"]["element_power_levels"];
    int index = 0;
    for (auto &&level : powers){
        elements[index].power = level;
        ++index;
    }
}

// applies linear probability of success on power range (0 - 3000 w) 
ProductState Heater::process(ProductState product_state) const {
    if (product_state == ProductState::good){
        double success_prob = (power_level - 3000.0) / (max_power - 3000.0);
        if (rand_between::rand_between(0.0, 1.0) > success_prob){
            return ProductState::bad;
        }
    }
    return product_state;
}

template<size_t S, typename T, typename... Args>
void Heater::force_power_levels(T power, Args... args){
    static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>);
    static_assert(sizeof...(args) < std::tuple_size<decltype(elements)>::value);

    if constexpr(sizeof...(args) > 0){
        elements[S].power = power;
        force_power_levels<S+1>(args...);
    }
    elements[S].power = power;
    update(0);
}

} // namespace devices

#endif // HEATER
