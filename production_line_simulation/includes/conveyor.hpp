#ifndef CONVEYOR_HPP
#define CONVEYOR_HPP

#include "config.hpp"
#include "random_between.hpp"
#include "units.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace devices
{

class Conveyor
{
public:
                    Conveyor() = default;
    void            update(milliseconds time_step);
    void            configure(Configuration& config);
    void            set_speed_target(uint8_t value);
    void            set_speed_current(uint8_t value);
    void            check_breakpoint(double temperature);

    uint8_t         get_speed_current() const {return speed_current;}
    uint8_t         get_speed_target()  const {return speed_target;}
    celsius         get_temperature()   const {return temperature;}
    double          get_upm_current()   const {return std::floor(upm_current);}
    bool            is_broken()         const {return broken;}
    double          units_passed_in(milliseconds timestep);
    double          get_direction();

private:
    const speed     upm_min                 {0};
    const speed     upm_max                 {600};
    const uint8_t   speed_min               {0};
    const uint8_t   speed_max               {255};
    const watts     power_min               {350};
    const watts     power_max               {20'000};
    const double    acceleration_upms2      {1.0 / 1000};
    const double    deacceleration_upms2    {-1.0 / 1000};
    const double    one_upm_in_watts        {32.75};
    const double    upm_per_speed           {600.0 / 255};
    const double    efficiency_min          {100};
    const double    efficiency_max          {56.0};
    const double    max_efficiency_upm      {60};
    const watts     watts_per_celsius       {1000};
    const celsius   breakdown_temperature   {80};
    const speed     speed_breakdown         {540};
    const double    breakdown_prob          {2};

    speed           upm_target              {0};
    speed           upm_current             {600};
    uint8_t         speed_target            {0};
    uint8_t         speed_current           {0};
    double          efficiency_current      {56};
    watts           power_current           {20'000};
    double          temperature             {11.2};  
    uint8_t         broken_speed_max        {200};
    bool            broken                  {false};

    uint8_t         calc_speed(double upm);
    double          calc_upm(uint8_t speed);
    double          calc_power(double upm_current);
    double          calc_efficiency(double upm);
    double          calc_temperature(double power, double efficiency);

};

/*
*   Configures conveyors current speed and related values based on json data.
*   If json data is not set, uses default values.
*/
void Conveyor::configure(Configuration& config)
{
    auto data   = config.data["Simulation"];
    int current = get_from_json(data["Conveyor"]["speed_current"], 0);
    
    if (current >= 0 && current <= 255)
    {
        speed_current       = static_cast<uint8_t>(current);
        speed_target        = speed_current;
        upm_current         = calc_upm(current);
        upm_target          = upm_current;
        efficiency_current  = calc_efficiency(upm_current);
        power_current       = calc_power(upm_current);
        temperature         = calc_temperature(power_current, efficiency_current);
    }
    else
    {
        std::cout << "Configuration error: Conveyor speeds must be 0-255\n";
        throw;
    }
}

/*
*  Updates conveyor speed, current_upm, power, efficiency
*  and temperature. Does this time_step amount of times.
*/
void Conveyor::update(milliseconds time_step)
{
    upm_target              = calc_upm(speed_target);
    
    if (upm_current != upm_target)
    {
        double direction    {};

        direction           = get_direction();
        upm_current         += (time_step * direction);
        upm_current         = std::clamp(upm_current, upm_min, upm_max);
        speed_current       = calc_speed(upm_current);
        power_current       = calc_power(upm_current);
        efficiency_current  = calc_efficiency(upm_current);
        temperature         = calc_temperature(power_current, efficiency_current);
    }
}

/*
*   Get (de)acceleration direction 
*/
double Conveyor::get_direction(){
    double direction = 0;
    if (upm_target > upm_current){
        direction = acceleration_upms2;
    }
    else if (upm_target < upm_current){
        direction = deacceleration_upms2;
    }
    return direction;
}

/*
*   Get number of conveyor half rotations during timestep
*/
double Conveyor::units_passed_in(milliseconds timestep){
    double direction = get_direction();
    double n_items = ((upm_current / 60'000.0) * timestep) + 0.5 * (direction / 1000.0) * pow(timestep, 2.0);
    return n_items;
}

/*
*   Sets the conveyor target speed (0-255).
*/
void Conveyor::set_speed_target(uint8_t value)
{
    if (broken)
    {
        speed_target = std::min(static_cast<unsigned>(value),
        static_cast<unsigned>(broken_speed_max));
    }
    else
        speed_target = static_cast<unsigned>(value);
}

/*
*   Sets the conveyor's current speed (0-255)
*/
void Conveyor::set_speed_current(uint8_t value)
{
    if (value <= speed_max && value >= speed_min)
        speed_current = value;
}

/*
*   If temperature is over breakdown temperature (80°C) or if conveyor
*   is running ove the breakdown speed (90%), there's a chance it will break.
*/
void Conveyor::check_breakpoint(double temperature)
{
    if (temperature > breakdown_temperature || upm_current > speed_breakdown)
    {
        if (!broken)
        {
            int rand = rand_between::rand_between(0, 100);
            if (rand < breakdown_prob)
                broken = true;
        }
    }
}

/*
*   Calculates and return upm (0-600) based on given speed (0-255).
*   Rounds the value down.
*/
double Conveyor::calc_upm(uint8_t speed)
{
    return std::floor(static_cast<unsigned>(speed) * upm_per_speed);
}

/*
*   Calculates speed (0-255) from given upm (0-600).
*   Returns the value rounded up.
*/
uint8_t Conveyor::calc_speed(double upm)
{
    return std::ceil(upm / upm_per_speed);
}

/*
*   Calculates power for given units per minute speed.
*   Return value is in watts.
*/
double Conveyor::calc_power(double upm_current)
{
    return power_min + upm_current * one_upm_in_watts;
}

/*
*   Calculates conveyors efficiency for given units per minute speed.
*   Returns a percentage between 100 and 56.
*/
double Conveyor::calc_efficiency(double upm)
{
    return std::max(efficiency_max,
        (efficiency_min -
        ((efficiency_min - efficiency_max) * upm / max_efficiency_upm)));
}
/*
*   Calculates temperature from power and efficiency, returns it as a
*   degree Celsius.
*/
double Conveyor::calc_temperature(double power, double efficiency)
{
    return power * (efficiency / 100) / watts_per_celsius;
}

} // namespace devices
#endif