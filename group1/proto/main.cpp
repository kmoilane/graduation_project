#include "includes/random_between.hpp"
#include "includes/units.hpp"
#include "includes/heater.hpp"
#include "includes/ambient_temperature.hpp"
#include "includes/conveyor.hpp"
#include "includes/temperature_sensor.hpp"
#include "includes/simulation.hpp"
#include "includes/config.hpp"

#include <iostream>
#include <deque>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>


class Communicator{

public:

    Communicator(){}

    // set simulation variables here
    void listen(Simulation& sim){}

    // send simulation variables here
    void send(Simulation& sim){}
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
    sim.heater.set_state(0b00000001);

    int test_max_loop {100};
    sim.q_control.set_state(true);
    for (size_t i = 0; i < test_max_loop; i++){

        sim.step();               // final solution should check if step can be taken

        sim.update_heaters();     // make update based on propability or/and time
        sim.update_sensors();     // make update based on propability or/and time
        sim.conveyor.update();    // make update based on propability or/and time

        sim.random_sensor_breakdown();
        sim.random_conveyor_breakdown();

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    
    return 0;
}