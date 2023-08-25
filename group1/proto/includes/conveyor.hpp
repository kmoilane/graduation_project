#ifndef CONVEYOR_HPP
#define CONVEYOR_HPP

#include "units.hpp"
#include "config.hpp"
#include "random_between.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <math.h>

class Conveyor
{
public:
    Conveyor() = default;
    void update();
    void configure(Configuration& config);
    void set_speed_target(uint8_t value);
    void set_speed_current(uint8_t value);
    void check_breakpoint(double temperature);

    uint8_t get_speed_current() const { return speed_current; }
    celsius get_temperature() const { return temperature; }
    double get_upm_current() const { return std::floor(upm_current); }

private:
    double upm_current {600};                   // 0-600 units per minute (conveyor speed)
    double upm_target {0};                      // 0-600 units per minute target (conveyor speed)
    uint8_t speed_target {0};                   // 0-255 conveyor speed set by controller
    uint8_t speed_current {0};                  // 0-255 convoyer speed sent to controller
    milliseconds shift_time {1000};             // Time to shift speed by 1upm
    time_stamp shift_begin = std::chrono::high_resolution_clock::now();
    //const speed upm_min {0}; unused
    //const speed upm_max {600}; unused
    const uint8_t speed_max {255};
    const uint8_t speed_min {0};

    double efficiency_current {56};             // current efficiency (56 - 100)
    watts power_current {20000};                // current power in watts (350 - 20000)
    const watts power_min {350};
    const watts power_max {20'000};
    const double one_upm_in_watts {32.75};      // power per 1 upm
    const double upm_per_speed {600.0 / 255};   // amount of units per second per speed
    const double efficiency_min {100};          // how much power is converted to heat
    const double efficiency_max {56.0};         // how much power is converted to heat
    const double max_efficiency_upm {60};       // upm at which max efficiency is reached
    const watts watts_per_celsius {1000};       // how many watts to increase temperature by 1 celsiud
    double temperature {11.2};                  // heat generated by conveyor
    
    const celsius breakdown_temperature {80};   // temp which after breakdown is possible
    const speed speed_breakdown {540};          // upm after which breakdown is possible
    const double breakdown_prob {5};            // probability for breakdonw in %
    bool broken {false};
    uint8_t broken_speed_max {200};             // max speed when broken

    double calc_upm(uint8_t speed);
    uint8_t calc_speed(double upm);
    double calc_power(double upm_current);
    double calc_efficiency(double upm);
    double calc_temperature(double power, double efficiency);
    
};

void Conveyor::configure(Configuration& config)
{
    auto target = config.data["Simulation"]["Conveyor"]["speed_target"];
    if (target.is_null())
        target = speed_target;
    auto current = config.data["Simulation"]["Conveyor"]["speed_current"];
    if (current.is_null())
        current = speed_current;
    if (target >= 0 && target <= 255 && current >= 0 && current <= 255)
    {
        speed_target = static_cast<uint8_t>(target);
        speed_current = static_cast<uint8_t>(current);
        upm_target = calc_upm(speed_target);
        upm_current = calc_upm(current);
        efficiency_current = calc_efficiency(upm_current);
        power_current = calc_power(upm_current);
        temperature = calc_temperature(power_current, efficiency_current);
    }
    else
    {
        std::cout << "Configuration error: Conveyor speeds must be 0-255\n";
        throw;
    }
}
void Conveyor::update()
{
    upm_target = calc_upm(speed_target);
    auto now = std::chrono::high_resolution_clock::now();
    
    if (upm_current != upm_target)
    {
        auto diff = now - shift_begin;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

        // POSSIBLE UPDATE
        double multiplier = (diff_ms.count() / 1000.0);
        double direction = upm_target > upm_current ? 1.0 : -1.0;
        upm_current += (multiplier * direction); // change_speed 1/s 

        // Increase or decrease current upm and speed
        speed_current = calc_speed(upm_current);
        
        // Increase or decrease current power
        power_current = calc_power(upm_current);

        // Increase or decrease current efficiency
        efficiency_current = calc_efficiency(upm_current);

        // Increase or decrease temperature
        temperature = calc_temperature(power_current, efficiency_current);

        // POSSIBLE UPDATE
    }

    shift_begin = now;
}
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
void Conveyor::set_speed_current(uint8_t value)
{
    if (value <= speed_max && value >= speed_min)
        speed_current = value;
}
void Conveyor::check_breakpoint(double temperature)
{
    if (temperature > breakdown_temperature || upm_current > speed_breakdown)
    {
        if (!broken)
        {
            int rand = rand_between::rand_between(1, 100);
            if (rand < breakdown_prob)
                broken = true;
        }
    }
}

double Conveyor::calc_upm(uint8_t speed)
{
    return std::floor(unsigned(speed) * upm_per_speed);
}
uint8_t Conveyor::calc_speed(double upm)
{
    return std::floor(upm / upm_per_speed);
}
double Conveyor::calc_power(double upm_current)
{
    return power_min + upm_current * one_upm_in_watts;
}
double Conveyor::calc_efficiency(double upm)
{
    return std::max(efficiency_max,
        (efficiency_min -
        ((efficiency_min - efficiency_max) * upm / max_efficiency_upm)));
}
double Conveyor::calc_temperature(double power, double efficiency)
{
    // efficiency / 100 for easier use since it represents percentage
    return power * (efficiency / 100) / watts_per_celsius;
}

#endif