#if !defined(TEMPERATURE_SENSOR_HPP)
#define TEMPERATURE_SENSOR_HPP

#include "heater.hpp"
#include "conveyor.hpp"
#include "ambient_temperature.hpp"

class TemperatureSensor
{
public:
    TemperatureSensor(double heater_factor)
    {
        heater_factor = heater_factor;
    }

    // voltage in millivolts
    int16_t voltage{0};
    bool state{false};
    void update()
    {
        temperature = get_temperature() + HeaterUnits::get_temperature() * heater_factor 
        + Convoyer::get_temperature();
        voltage = 10 * temperature;
    }

private:
    // joku enum, vaihtoehdot 0.05, 0.1, 0.15, 0.20, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5
    double heater_factor;
    double temperature;
};

#endif
