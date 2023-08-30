#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include "random_between.hpp"
#include "ambient_temperature.hpp"
#include "temperature_sensor.hpp"
#include "units.hpp"
#include "conveyor.hpp"
#include "config.hpp"
#include "devices.hpp"
#include "product.hpp"
#include "config.hpp"

struct Simulation
{

private:

    milliseconds    step_duration       {100}; 
    double          prev_offset         {0};

public:
    // Product states at production line steps
    std::array<ProductState, 4> stages {ProductState::good,
                                        ProductState::not_present,
                                        ProductState::not_present,
                                        ProductState::not_present};
    // System devices
    AmbientTemperature ambient_temperature     {20};
    devices::Heater heater                     {false};
    devices::Cooler cooler                     {false};
    devices::QualityControl quality_control    {false};
    devices::Bolter bolter                     {false};
    devices::Shaper shaper                     {false};
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

        auto config_duration = config.data["Simulation"]["step_time_ms"];
        step_duration = config_duration.is_null() ? 100 : int(config_duration);
        
    };                                      

    // average temperature of sensors
    celsius get_average_temperature() const{
        celsius average{0};
        for (const TemperatureSensor& sensor : temp_sensors){
            average += sensor.get_temperature();
        }
        return average / temp_sensors.size();
    }

    // simulate products going through the manufacturing
    void step(milliseconds step_time = -1){

        if (step_time < 0){
            step_time = step_duration;
        }

        // Units gone through single step in manufacturing process since last step
        double steps = (((conveyor.get_upm_current() / 60'000) * step_time) * 3.0) + prev_offset;

        size_t full_shifts = std::floor(steps);
        for (size_t i = 0; i < full_shifts; i++){
    
            // process item in current new stage
            stages[1] = heater.process(stages[1]);
            stages[2] = shaper.process(stages[2]);
            stages[3] = bolter.process(stages[3]);
            quality_control.process(conveyor.get_upm_current(), stages.back());

            // shift elements to a new position and add a new item to the production line
            std::rotate(stages.rbegin(), stages.rbegin() - 1, stages.rend());
            stages[0] = ProductState::good;
        }

        prev_offset = steps - full_shifts;

    }

    // set internal updates to all devices
    void update(milliseconds step_time = -1){

        if (step_time < 0){
            step_time = step_duration;
        }
        
        heater.update(step_time);
        conveyor.update(step_time);    
        cooler.update(step_time);
        ambient_temperature.update(step_time); 
        conveyor.check_breakpoint(get_average_temperature());

        for (auto& sensor : temp_sensors){
            sensor.update(ambient_temperature.get_temperature(),
                          heater.get_temperature(),
                          conveyor.get_temperature(),
                          cooler.get_temperature());
        }
    }

};


#endif
