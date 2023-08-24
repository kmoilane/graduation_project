#if !defined(TEMPERATURE_SENSOR_HPP)
#define TEMPERATURE_SENSOR_HPP

#include "units.hpp"
#include "heater.hpp"
#include "conveyor.hpp"
#include "ambient_temperature.hpp"

class TemperatureSensor
{
public:
    celsius sensor_temperature{5};
    TemperatureSensor(double heater_factor, int id)
    {
        heater_factor_ = heater_factor;
        id_ = id;
    }
    // voltage in millivolts
    millivolts voltage{0};
    bool state{false};

    void update(celsius ambient_temp, celsius heater_temp, celsius conveyor_temp )
    {
        std::cout << "ambient: " << ambient_temp << " heater: " << heater_temp << " conveyor: " << conveyor_temp << "\n";
        sensor_temperature = ambient_temp + (heater_temp * heater_factor_) + conveyor_temp;
        std::cout << "sensor " << id_ << ": " << sensor_temperature << "\n";

        voltage = 10 * sensor_temperature;
    }

    celsius get_temperature()
    {
        return sensor_temperature;
    }

private:
    double heater_factor_;
    int id_;
};

#endif
