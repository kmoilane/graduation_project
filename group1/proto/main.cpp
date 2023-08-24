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


class Communicator{

public:
    
    simulation_shm_wrapper shm {"simulation_shm"};

    Communicator(){}

    // set simulation variables here
    void listen(Simulation& sim){
        // This would come from shared memory
        sim.conveyor.set_speed_target(shm.read_conveyor_target_speed());
    }

    // send simulation variables here
    void send(Simulation& sim){
        shm.set_conveyor_speed_sensor(sim.conveyor.get_speed_current());

        for (size_t i = 0; i < sim.temp_sensors.size(); i++)
        {
            shm.set_temperature_sensor(i + 1, sim.temp_sensors[i].get_voltage());
        }
    }
};


int main(int argc, char const *argv[])
{
    // for testing purposes
    // final simulation must take account realtime operation

    std::string path{};
    if (argc == 2)
        path = argv[1];

    Configuration config("config/config_full.json");
    Simulation sim(config);
    Communicator communicator;

    auto start = std::chrono::high_resolution_clock::now();
    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(800, 800, 800);

    int test_max_loop {100};
    sim.q_control.set_state(true);
    for (size_t i = 0; i < test_max_loop; i++)
    {
        sim.step();               // final solution should check if step can be taken

        sim.update_heaters();     // make update based on propability or/and time
        sim.update_sensors();     // make update based on propability or/and time
        sim.conveyor.update();    // make update based on propability or/and time

        sim.random_sensor_breakdown();
        sim.random_conveyor_breakdown();

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread
        std::cout <<  "Current upm: " << sim.conveyor.get_upm_current() << '\n';
        std::cout <<  "Current speed: " << (int)sim.conveyor.get_speed_current() << '\n';
        std::cout <<  "Quality Control: " << std::bitset<16>(sim.q_control.get_output()) << '\n';


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}