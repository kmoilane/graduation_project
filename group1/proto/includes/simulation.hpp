#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "random_between.hpp"
#include "units.hpp"
#include "heater.hpp"
#include "ambient_temperature.hpp"
#include "conveyor.hpp"
#include "temperature_sensor.hpp"

struct Simulation
{
    int speed_multiplier {1};
    std::deque<bool> qc_storage;
    const speed qc_speed_max = 542; 
    const double qc_false_positive_prob {0.005};
    const double bolter_fail_prob {0.02};

    Conveyor conveyor;

     // joku enum, vaihtoehdot 0.05, 0.1, 0.15, 0.20, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5
    std::array<TemperatureSensor, 10> temp_sensors {0.4, 0.45, 0.5, 0.45, 0.4, 0.35, 0.3, 0.25, 0.2, 0.15};
    AmbientTemperature ambient_temperature {20};

    const std::array<double, 10> temp_sensor_heater_effect{
        0.05, 0.15, 0.25, 0.5, 0.75, 0.65, 0.45, 0.3, 0.1, 0.05
    };
    const std::array<double, 10> temp_sensor_cooler_effect{
        0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15
    };

    HeaterUnits heater {0b0};

    bool Bolter_state_on {false};
    bool Shaper_state_on {false};
    bool Camera_state_on {false};
    bool Cooler_state_on {false};

    // initialize variables from file
    Simulation(Configuration& config){
        conveyor.upm_current = config.data["Simulation"]["Conveyor"]["upm_current"];
        conveyor.efficiency_current = config.data["Simulation"]["Conveyor"]["efficiency_current"];
        conveyor.power_current = config.data["Simulation"]["Conveyor"]["power_current"];
        conveyor.temperature = config.data["Simulation"]["Conveyor"]["temperature"];
        conveyor.upm_target = conveyor.upm_current;
    };

    // shift a product one step forward
    void step(){
        
        bool product_state {true};
        product_state = apply_heating(product_state);
        product_state = apply_shaping(product_state);
        product_state = apply_bolting(product_state);
        product_state = apply_quality_control(product_state);

        if (qc_storage.size() == 16){
            qc_storage.clear();
        }

        // Push to left most spot
        // Every object has now shifted one index to the right
        qc_storage.push_front(product_state); 

        // sensors may break
        random_sensor_breakdown();
        // convoyer may break
        random_convoyer_breakdown();
    }

    bool apply_heating(bool product_state){return product_state;}
    bool apply_shaping(bool product_state){return product_state;}
    bool apply_bolting(bool product_state){return product_state;}
    bool apply_quality_control(bool product_state){return product_state;}

    void random_sensor_breakdown(){}
    void random_convoyer_breakdown(){}

    void update_sensors(){};
    void update_convoyer(){};

    void update_heaters(){
        heater.update();
    };

    // simulates 'natural' ambient temperature change over time
    void update_ambient_temperature(){
        ambient_temperature.update();
    }

};


#endif
