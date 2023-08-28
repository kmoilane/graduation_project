#if !defined(TEMPERATURE_SENSOR_HPP)
#define TEMPERATURE_SENSOR_HPP

#include "random_between.hpp"
#include "units.hpp"

class TemperatureSensor
{
public:
    TemperatureSensor(double heater_factor)
    {
        heater_factor_ = heater_factor;
        is_broken = false;
    }

    const celsius get_temperature() const
    {
        return temperature;
    }

    const celsius get_voltage() const
    {
        //std::cout << "voltage: " << voltage << "\n";
        return voltage;
    }

    void update(celsius ambient_temp, celsius heater_temp, celsius conveyor_temp, celsius cooler_temp)
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
            celsius tmp = ambient_temp + (heater_temp * heater_factor_) + conveyor_temp + SENSOR_TEMP + cooler_temp;
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
        //std::cout << "sensor " << id_ << ": " << temperature << "\n";
    }

    
    void configure(Configuration &config)
    {
        auto tmp_probability = config.data["Simulation"]["temperature_sensor"]["break_probability"];
        if (!tmp_probability.is_null())
        {
            set_probability(tmp_probability);
        }
    }

    void set_probability(double break_probability)
    {
        if (break_probability >= 0 && break_probability <= 100)
        {
            break_probability_ = break_probability;
        }
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
    double break_probability_{0.0001};
    bool is_broken{};
};

#endif
