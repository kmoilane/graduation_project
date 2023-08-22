#include <iostream>
#include <deque>
#include <array>
#include <chrono>
#include <thread>
#include <algorithm>
#include <memory>


class Configuration
{
public:
    std::string filepath;

    Configuration(std::string&& path) : filepath(path) {}
    ~Configuration() {}
};



using celcius = double;
using watts = int16_t;
using seconds = int16_t;
using speed = int16_t;
using milliseconds = int16_t;


struct Simulation
{
    int speed_multiplier {1};
    std::deque<bool> qc_storage;
    const speed qc_speed_max = 542; 
    const double qc_false_positive_prob {0.005};
    const double bolter_fail_prob {0.02};
    const double convoyer_breakdown_prob {0.05};

    std::array<celcius, 10> temp_sensors;
    celcius ambient_temperature {20};
    celcius convoyer_temperature {20};

    const std::array<double, 10> temp_sensor_heater_effect{
        0.05, 0.15, 0.25, 0.5, 0.75, 0.65, 0.45, 0.3, 0.1, 0.05
    };
    const std::array<double, 10> temp_sensor_cooler_effect{
        0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15, 0.15
    };

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

    const seconds heater_breakdown_time {10};
    const watts heater_power_increase_1s {10};
    const watts heater_power_decrease_1s {5};
    const watts heater_power_min {0};
    const watts heater_power_max {2000};
    std::array<watts, 3> heater_power_levels {0,0,0};

    bool Bolter_state_on {false};
    bool Shaper_state_on {false};
    bool Camera_state_on {false};
    bool Cooler_state_on {false};

    // initialize variables from file
    Simulation(Configuration& config){
        std::fill(temp_sensors.begin(), temp_sensors.end(), ambient_temperature);
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
    
    void update_heaters(){};
    void update_sensors(){};
    void update_convoyer(){};
    void update_ambient_temperature(){}

};

class Communicator{

public:

    Communicator(){}

    // set simulation variables here
    void listen(){}

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

    int test_max_loop {10};

    for (size_t i = 0; i < test_max_loop; i++){

        sim.step();            // final solution should check if step can be taken
        sim.update_heaters();  // make update based on propability or/and time
        sim.update_sensors();  // make update based on propability or/and time
        sim.update_convoyer(); // make update based on propability or/and time

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(); // ideally in a another thread
        std::cout << sim.temp_sensors[0] << '\n';
    }
    
    return 0;
}
