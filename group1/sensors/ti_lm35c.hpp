#ifndef TI_LM35C
#define TI_LM35C

#include <cstdint>

/*
**  
*/
class LM35C
{
    public:

        LM35C() = default;
        double generate_voltage(double temperature);
        double convert_to_celsius(double voltage)
        {
            // Convert simulated analog output to temperature
            return (voltage / 1024. * ref_voltage);
        }

    private:
        /*
        **  uint16_t values are millivolts
        */
        const double sensitivity = 10; // Sensitivity of LM35 (10 mV/°C/10)
        const double ref_temperature = 0.0; // Reference temperature in Celsius
        const double ref_voltage = 500; // Reference voltage at reference temperature (5V/100)
};

double LM35C::generate_voltage(double temperature)
{
    return temperature / ref_voltage * 1024.;
}

#endif
