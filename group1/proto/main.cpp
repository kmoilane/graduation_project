#include "includes/random_between.hpp"
#include "includes/units.hpp"
#include "includes/heater.hpp"
#include "includes/ambient_temperature.hpp"
#include "includes/conveyor.hpp"
#include "includes/temperature_sensor.hpp"
#include "includes/simulation.hpp"
#include "includes/config.hpp"
#include "shm/shm.hpp"

#include <iostream>
#include <deque>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <filesystem>


class Communicator{

public:
    
    simulation_shm_wrapper shm {"../simulation_shm"};

    Communicator(){}

    // set simulation variables here
    void listen(Simulation& sim)
    {
        sim.conveyor.set_speed_target(shm.read_conveyor_target_speed());
        sim.heater.set_state(shm.read_heaters());
        sim.cooler.set_state(shm.read_cooler());
        sim.q_control.set_state(shm.read_camera_status());
    }

    // send simulation variables here
    void send(const Simulation& sim)
    {
        shm.set_conveyor_speed_sensor(sim.conveyor.get_speed_current());

        for (size_t i = 0; i < sim.temp_sensors.size(); i++)
        {
            shm.set_temperature_sensor(i + 1, sim.temp_sensors[i].get_voltage());
        }

        shm.set_qc_camera_feed(sim.q_control.get_output());
    }
};


int main(int argc, char const *argv[])
{
    // for testing purposes
    // final simulation must take account realtime operation

    std::string path{};
    if (argc == 2)
        path = argv[1];

    
    Configuration config("../config/config_full.json");
    Simulation sim(config);
    Communicator communicator;

    auto start = std::chrono::high_resolution_clock::now();

    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(1500, 1500, 1500);

    sim.q_control.set_state(true);


    std::cout   << std::setw(10) << "upm"        << std::setw(8) << " || " 
                << std::setw(10) << "speed"      << std::setw(8) << " || "
                << std::setw(10) << "heater W"   << std::setw(8) << " || " 
                << std::setw(10) << "avg temp"   << std::setw(8) << " || "
                << std::setw(16) << "qc"         << std::setw(8) << " || "
                << std::setw(16) << "heater on"  << std::setw(8) << " || "
                << std::setw(16) << "ambient"    << std::setw(8) << " || "
                << std::setw(10) << "time ms"    << std::setw(8) << " || "
                << std::setw(10) << "heater temp"<< std::setw(8) << " || " << '\n' << '\n'; 


    std::cout << std::setprecision(2) << std::fixed;

    int sleep_time;

    for (size_t i = 0; i < 1000; i++)
    {
        auto end = std::chrono::high_resolution_clock::now();
        int diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        sim.step();               // final solution should check if step can be taken

        sim.update_heaters();     // make update based on propability or/and time
        sim.update_sensors();     // make update based on propability or/and time
        sim.conveyor.update();    // make update based on propability or/and time
        sim.cooler.update();
        sim.ambient_temperature.update();

        sim.random_conveyor_breakdown();
        sim.random_sensor_breakdown();

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread

        std::cout << std::setw(10) << sim.conveyor.get_upm_current()                << std::setw(8) << " || "
                  << std::setw(10) << int(sim.conveyor.get_speed_current())         << std::setw(8) << " || "
                  << std::setw(10) << sim.heater.get_power()                        << std::setw(8) << " || "
                  << std::setw(10) << sim.get_average_temperature()                 << std::setw(8) << " || "
                  << std::setw(10) << std::bitset<16>(sim.q_control.get_output())   << std::setw(8) << " || "
                  << std::setw(10) << std::bitset<8>(sim.heater.get_state())        << std::setw(8) << " || "
                  << std::setw(10) << sim.ambient_temperature.get_temperature()     << std::setw(8) << " || "
                  << std::setw(10) << diff                                          << std::setw(8) << " || "
                  << std::setw(10) << sim.heater.get_temperature()                  << std::setw(8) << " || " << '\n';

        

        sleep_time = 1000 / (sim.conveyor.get_upm_current() / 60 * 4);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
    
    return 0;
}


int main2(int argc, char const *argv[])
{
    
    Configuration config("../config/config_full.json");
    Simulation sim(config);
    Communicator communicator;

    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(0, 0, 0);
    sim.q_control.set_state(true);

    double time_multiplier = 1; // timestep = 100 ms
    int update_time = 100;

    for (size_t timestep = 0; timestep < 1000; timestep++)
    {
/* 
           
        sim.update_heaters(time_multiplier);     // make update based on propability or/and time
        sim.update_sensors(time_multiplier);     // make update based on propability or/and time
        sim.conveyor.update(time_multiplier);    // make update based on propability or/and time
        sim.cooler.update(time_multiplier);
        sim.ambient_temperature.update(time_multiplier);


        if conveyor has moved enough 
            sim.step();    

        if enough time has passed 
            communicator.send(sim);   // ideally in a another thread
            communicator.listen(sim); // ideally in a another thread


            0.4         1                   0.5                   0.1
        sleeptime = update_time  * time_multiplier - loopin alusta tähän pisteeseen
        = loopin kesto 0.5 s


        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time)); 
        */
    }
    
    return 0;
}




/* 

TODO:
    - stepping and timing
    - 



 */