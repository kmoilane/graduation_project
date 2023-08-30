#if !defined(TEMPERATURE_SENSOR_HPP)
#define TEMPERATURE_SENSOR_HPP

#include "random_between.hpp"
#include "config.hpp"
#include "units.hpp"

class TemperatureSensor
{
public:
    TemperatureSensor(double heater_factor) : heater_factor(heater_factor) {}
    celsius get_temperature() const { return temperature; }
    const celsius get_voltage() const { return voltage; }
    void update(celsius ambient_temp, celsius heater_temp, celsius conveyor_temp, celsius cooler_temp);
    void configure(Configuration &config);
    void set_probability(double break_probability);

private:
    const uint16_t  VOLTAGE_FACTOR{10};
    const           celsius SENSOR_TEMP{0.08};
    const           celsius MAX_TEMP{110};
    const           celsius MIN_TEMP{-40};
    
    // Configurable
    double          break_probability{0.0001};

    millivolts      voltage{0};
    celsius         temperature{0};
    double          heater_factor{0};
    bool            is_functioning{true};
};

void TemperatureSensor::update(celsius ambient_temp, celsius heater_temp, celsius conveyor_temp, celsius cooler_temp)
{
    if (rand_between::rand_between(0.0, 100.0) < break_probability)
    {
        temperature = 0;
        is_functioning = false;
    }
    if (is_functioning)
    {
        celsius tmp = ambient_temp + (heater_temp * heater_factor) + conveyor_temp + SENSOR_TEMP + cooler_temp;
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
}

void TemperatureSensor::configure(Configuration &config)
{
    auto tmp_probability = config.data["Simulation"]["temperature_sensor"]["break_probability"];
    if (!tmp_probability.is_null())
    {
        set_probability(tmp_probability);
    }
}

void TemperatureSensor::set_probability(double tmp_probability)
{
    if (tmp_probability >= 0 && tmp_probability <= 100)
    {
        break_probability = tmp_probability;
    }
}

#endif