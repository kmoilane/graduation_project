#if !defined(TEMPERATURE_SENSOR_HPP)
#define TEMPERATURE_SENSOR_HPP

#include "random_between.hpp"
#include "units.hpp"

class TemperatureSensor
{
public:
    TemperatureSensor(double heater_factor, int id, double break_probability)
    {
        heater_factor_ = heater_factor;
        id_ = id;
        break_probability_ = break_probability;
        is_broken = false;
    }

    const celsius get_temperature() const
    {
        return temperature;
    }

    const celsius get_voltage() const
    {
        std::cout << "voltage: " << voltage << "\n";
        return voltage;
    }

    void update(celsius ambient_temp, celsius heater_temp, celsius conveyor_temp)
    {
        if (is_broken)
        {
            // std::cout << "sensor " << id_ << ": " << temperature << "\n";
            return;
        }
        else if (rand_between::rand_between(0.0, 100.0) < break_probability_)
        {
            temperature = 0;
            is_broken = true;
        }
        else
        {
            celsius tmp = ambient_temp + heater_temp * heater_factor_ + conveyor_temp + SENSOR_TEMP;
            if (tmp < MIN_TEMP)
            {
                temperature = MIN_TEMP;
            }
            else if (tmp > MAX_TEMP)
            {
                temperature = MAX_TEMP;
            }
            else
            {
                temperature = tmp;
            }
            voltage = VOLTAGE_FACTOR * temperature;
        }
        std::cout << "sensor " << id_ << ": " << temperature << "\n";
    }

private:
    // voltage in millivolts
    const celsius SENSOR_TEMP{0.08};
    const celsius MAX_TEMP{110};
    const celsius MIN_TEMP{-40};
    const uint16_t VOLTAGE_FACTOR{10};

    millivolts voltage{0};
    celsius temperature{0};
    double heater_factor_{0};
    int id_{0};
    double break_probability_{0.01};
    bool is_broken{};
};

#endif