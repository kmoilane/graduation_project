#include "includes/random_between.hpp"
#include "includes/units.hpp"
#include "includes/heater.hpp"
#include "includes/ambient_temperature.hpp"
#include "includes/conveyor.hpp"
#include "includes/temperature_sensor.hpp"
#include "includes/simulation.hpp"

#include <iostream>
#include <deque>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <fstream>

class Configuration
{
public:
    std::string filepath;
    json data;

    Configuration(std::string&& path) : filepath(path) {
        std::ifstream file(filepath);
        data = json::parse(file);
    }
    ~Configuration() {}
};

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

    Configuration config("some_path.json");
    Simulation sim(config);
    Communicator communicator;

    auto start = std::chrono::high_resolution_clock::now();

    int test_max_loop {100};
    for (size_t i = 0; i < test_max_loop; i++){

        sim.step();               // final solution should check if step can be taken
        sim.update_heaters();     // make update based on propability or/and time
        sim.update_sensors();     // make update based on propability or/and time
        sim.update_convoyer();    // make update based on propability or/and time

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    
    return 0;
}
