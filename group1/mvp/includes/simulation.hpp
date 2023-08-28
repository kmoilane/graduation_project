#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "random_between.hpp"
#include "units.hpp"
#include "heater.hpp"
#include "ambient_temperature.hpp"
#include "conveyor.hpp"
#include "temperature_sensor.hpp"
#include "config.hpp"
#include "quality_control.hpp"
#include "devices.hpp"
#include "product.hpp"
#include "cooler.hpp"

struct Simulation
{
    int speed_multiplier {1};

    std::array<ProductState, 4> stages {ProductState::not_present,
                                        ProductState::not_present,
                                        ProductState::not_present,
                                        ProductState::not_present};

    AmbientTemperature ambient_temperature {20};
    devices::Heater heater {0b00000000};
    devices::Cooler cooler {0b00000000};
    devices::QualityControl quality_control {0b00000000};
    devices::Bolter bolter;
    devices::Shaper shaper;
    devices::Conveyor conveyor;



    // Init sensors 
   std::vector<TemperatureSensor> temp_sensors{{0.40},
                                               {0.45},
                                               {0.50},
                                               {0.45},
                                               {0.40},
                                               {0.35},
                                               {0.30},
                                               {0.25},
                                               {0.20},
                                               {0.15}};
    // average temperature of sensors
    celsius get_average_temperature()
    {
        celsius average{0};
        for (TemperatureSensor& sensor : temp_sensors)
        {
            average += sensor.get_temperature();
        }

        return average / temp_sensors.size();
    }

    // initialize variables from file
    Simulation(Configuration& config){
        conveyor.configure(config);
        heater.configure(config);
        ambient_temperature.configure(config);
        bolter.configure(config);
        shaper.configure(config);
        quality_control.configure(config);
        cooler.configure(config);
        for(auto& sensor : temp_sensors)
        {
            sensor.configure(config);
        }
    };

    // shift a product one step forward
    void step(){

        // process item in current new stage
        stages[1] = heater.process(stages[1]);
        stages[2] = shaper.process(stages[2]);
        stages[3] = bolter.process(stages[3]);

        quality_control.process(conveyor.get_upm_current(), stages.back());

        // shift elements to a new position and add a new item to the production line
        std::rotate(stages.rbegin(), stages.rbegin() - 1, stages.rend());
        stages[0] = ProductState::good;
    }

    void update_sensors()
    {
        for (auto& sensor : temp_sensors)
        {
            sensor.update(ambient_temperature.get_temperature(), heater.get_temperature(), conveyor.get_temperature(), cooler.get_temperature());
        }
    };

    void update_heaters(){
        heater.update();
    };

    // simulates 'natural' ambient temperature change over time
    void update_ambient_temperature(){
        ambient_temperature.update();
    }

};


#endif
