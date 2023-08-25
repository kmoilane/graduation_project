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
#include "cooler.hpp"

struct Simulation
{
    int speed_multiplier {1};


    // status of objects in production line stages (-1 = not present, 0 = invalid, 1 = valid)
    std::array<int, 4> stages {-1, -1, -1, -1};

    AmbientTemperature ambient_temperature {20};
    HeaterUnits heater {0b00000000};
    Cooler cooler {0b00000001};
    QualityControl q_control;
    Conveyor conveyor;
    Bolter bolter;
    Shaper shaper;


    // Init sensors 
   std::vector<TemperatureSensor> temp_sensors{{0.4, 0, 0.0},
                                                {0.45, 1, 0.0},
                                                {0.5, 2, 0.0},
                                                {0.45, 3, 0.0},
                                                {0.4, 4, 0.0},
                                                {0.35, 5, 0.0},
                                                {0.3, 6, 0.0},
                                                {0.25, 7, 0.0},
                                                {0.2, 8, 0.0},
                                                {0.15, 9, 0.0}};

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
    };

    // shift a product one step forward
    void step(){

        // process item in current new stage
        stages[1] = heater.process(stages[1]);
        stages[2] = shaper.process(stages[2]);
        stages[3] = bolter.process(stages[3]);

        q_control.apply_camera(conveyor.get_upm_current(), stages.back());

        // shift elements to a new position and add a new item to the production line
        std::rotate(stages.rbegin(), stages.rbegin() - 1, stages.rend());
        stages[0] = 1;
    }

    void random_sensor_breakdown(){}
    void random_conveyor_breakdown()
    {
        conveyor.check_breakpoint(get_average_temperature());
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
