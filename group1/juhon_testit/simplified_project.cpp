#include "random_between.h"
#include "units.h"
#include "heater.h"
#include "ambient_temp.h"

#include <iostream>
#include <deque>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>




class Configuration
{
public:
    std::string filepath;

    Configuration(std::string&& path) : filepath(path) {}
    ~Configuration() {}
};


struct Simulation
{
    int speed_multiplier {1};
    std::deque<bool> qc_storage;
    const speed qc_speed_max = 542; 
    const double qc_false_positive_prob {0.005};
    const double bolter_fail_prob {0.02};
    const double convoyer_breakdown_prob {0.05};

    std::array<celcius, 10> temp_sensors;
    AmbientTemperature ambient_temperature {20};

    const std::array<double, 10> temp_sensor_heater_effect{
        0.05, 0.15, 0.25, 0.5, 0.75, 0.65, 0.45, 0.3, 0.1, 0.05
    };
    const std::array<double, 10> temp_sensor_cooler_effect{
        0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15
    };

    celcius convoyer_temperature {20};
    speed covoyer_speed_current {0};
    int8_t convoyer_speedsensor_current {0};
    milliseconds convoyer_shift_time {0};
    const speed covoyer_speed_breakdown {540};
    const speed covoyer_speed_min {0};
    const speed covoyer_speed_max {600};
    const speed convoyer_acceleration_1s {1};

    double convoyer_effiency_current {0.44};
    const watts convoyer_power_min {350};
    const watts convoyer_power_max {20'000};
    const celcius convoyer_temperature_breakdown {80};
    const double convoyer_effiency_min {0};
    const double convoyer_effiency_max {0.44};

    HeaterUnits heater {0b0};

    bool Bolter_state_on {false};
    bool Shaper_state_on {false};
    bool Camera_state_on {false};
    bool Cooler_state_on {false};

    // initialize variables from file
    Simulation(Configuration& config){};

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
    void update_heaters(){};


    // simulates 'natural' ambient temperature change over time
    void update_ambient_temperature(){
        ambient_temperature.update();
    }

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

    // TEMPORARY heater demo

    auto start = std::chrono::high_resolution_clock::now();
    sim.heater.set_states(0b00000101);
    sim.heater.set_power_levels(0,0,0);

    // TEMPORARY heater demo

    int test_max_loop {100};
    for (size_t i = 0; i < test_max_loop; i++){

        sim.step();               // final solution should check if step can be taken
        sim.update_heaters();     // make update based on propability or/and time
        sim.update_sensors();     // make update based on propability or/and time
        sim.update_convoyer();    // make update based on propability or/and time

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread



        // TEMPORARY heater demo

        sim.heater.update();

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed_shift = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        for (size_t i = 0; i < 3; i++){
            std::cout << std::setw(10) << sim.heater.get_power_level(i) << " || ";
        }

        std::cout << "time elapsed: " << elapsed_shift.count() << '\n';

        // TEMPORARY heater demo


        std::this_thread::sleep_for(std::chrono::seconds(1));


    }
    
    
    return 0;
}
