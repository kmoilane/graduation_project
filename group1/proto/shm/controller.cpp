#include "shm.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    simulation_shm_wrapper shm("../simulation_shm");
    uint8_t target_speed = 200;
    shm.set_conveyor_target_speed(target_speed);
    uint8_t current_speed = 0;
    int sensor1 {};
    int sensor2 {};

    while (current_speed != target_speed)
    {
        current_speed = shm.read_conveyor_speed_sensor();
        sensor1 = (int)shm.read_temperature_sensor(1);
        sensor2 = (int)shm.read_temperature_sensor(2);

        std::cout << "Conveyor current speed: " << (int)current_speed  << std::endl;
        std::cout << "Sensor1 millivoltages: " << sensor1 << std::endl;
        std::cout << "Sensor2 millivoltages: " << sensor2 << std::endl;


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}