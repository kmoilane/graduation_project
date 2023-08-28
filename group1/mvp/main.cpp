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
#include <bitset>
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
        sim.quality_control.set_state(shm.read_camera_status());
    }

    // send simulation variables here
    void send(const Simulation& sim)
    {
        shm.set_conveyor_speed_sensor(sim.conveyor.get_speed_current());

        for (size_t i = 0; i < sim.temp_sensors.size(); i++)
        {
            shm.set_temperature_sensor(i + 1, sim.temp_sensors[i].get_voltage());
        }

    }
    void send_qc_feed(const Simulation& sim)
    {
        shm.set_qc_camera_feed(sim.quality_control.get_output());
    }
};

void print_simulation(Simulation& sim)
{
    milliseconds loop_time = 1000;
    time_stamp start_time{};
    time_stamp end_time{};
    milliseconds sleep_time{};
    while (true)
    {
        start_time = std::chrono::high_resolution_clock::now();
        system("clear");
        std::cout   << std::setw(10) << "upm"        << std::setw(8) << " || " 
                    << std::setw(10) << "speed"      << std::setw(8) << " || "
                    << std::setw(10) << "heater W"   << std::setw(8) << " || " 
                    << std::setw(10) << "avg temp"   << std::setw(8) << " || "
                    << std::setw(16) << "qc"         << std::setw(8) << " || "
                    << std::setw(16) << "heater on"  << std::setw(8) << " || "
                    << std::setw(16) << "ambient"    << std::setw(8) << " || "
                    << std::setw(10) << "heater temp"<< std::setw(8) << " || " 
                    << std::setw(10) << "steps"      << std::setw(8) << " || " << '\n' << '\n';

        std::cout   << std::setw(10) << sim.conveyor.get_upm_current()      << std::setw(8) << " || "
                    << std::setw(10) << int(sim.conveyor.get_speed_current())         << std::setw(8) << " || "
                    << std::setw(10) << sim.heater.get_power()                        << std::setw(8) << " || "
                    << std::setw(10) << sim.get_average_temperature()                 << std::setw(8) << " || "
                    << std::setw(16) << std::bitset<16>(sim.quality_control.get_output())   << std::setw(8) << " || "
                    << std::setw(16) << std::bitset<8>(sim.heater.get_state())        << std::setw(8) << " || "
                    << std::setw(16) << sim.ambient_temperature.get_temperature()     << std::setw(8) << " || "
                    << std::setw(10) << sim.heater.get_temperature()                  << std::setw(9) << " || " << '\n';

        end_time = std::chrono::high_resolution_clock::now();
        auto diff = end_time - start_time;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);

        sleep_time = loop_time - diff_ms.count();
        if (sleep_time < 0)
            sleep_time = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

void update_simulation(Simulation& sim, Communicator& communicator, double time_multiplier)
{
    milliseconds loop_time = 1000;
    time_stamp start_time{};
    time_stamp end_time{};
    milliseconds sleep_time{};

    while (true)
    {
        start_time = std::chrono::high_resolution_clock::now();

        sim.update_heaters(/* time_multiplier */);     // make update based on propability or/and time
        sim.update_sensors(/* time_multiplier */);     // make update based on propability or/and time
        sim.conveyor.update(time_multiplier);    // make update based on propability or/and time
        sim.cooler.update(/* time_multiplier */);
        sim.ambient_temperature.update(/* time_multiplier */);
        sim.conveyor.check_breakpoint(sim.get_average_temperature());

        communicator.send(sim);   // ideally in an another thread
        communicator.listen(sim); // ideally in an another thread

        end_time = std::chrono::high_resolution_clock::now();
        auto diff = end_time - start_time;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        
        sleep_time = loop_time * time_multiplier - diff_ms.count();
        if (sleep_time < 0)
            sleep_time = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time)); 
    }
}

int main(int argc, char const *argv[])
{
    
    Configuration config("config/config_full.json");
    Simulation sim(config);
    Communicator communicator;

    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(1500, 1500, 1500);
    sim.quality_control.set_state(true);

    double time_multiplier = 1; // timestep = 100 ms
    int update_time = 100;

    int steps = -2;
    uint16_t conveyor_speed = sim.conveyor.get_upm_current();
    milliseconds step_time = 60000 / conveyor_speed;
    milliseconds sleep_time = 0;
    time_stamp loop_start_time;
    time_stamp loop_end_time;

    std::cout << "Simulation running...\n";
    //std::jthread print_thread(print_simulation, std::ref(sim));
    std::jthread simulation_thread(update_simulation, std::ref(sim), std::ref(communicator), time_multiplier);

    while (true)
    {
        loop_start_time = std::chrono::high_resolution_clock::now();

        conveyor_speed = sim.conveyor.get_upm_current();
        if (conveyor_speed > 0)
            step_time = 60000 / conveyor_speed;
        else
            step_time = 0;

        sim.step();    
        steps++;

        if (steps % 16 == 0)
        {
            communicator.send_qc_feed(sim);
        }

        loop_end_time = std::chrono::high_resolution_clock::now();
        auto diff = loop_end_time - loop_start_time;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        
        sleep_time = step_time * time_multiplier - diff_ms.count();
        if (sleep_time < 0)
            sleep_time = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time)); 
    }
    
    return 0;
}




/* 

TODO:
    - stepping and timing
    - 



 */
