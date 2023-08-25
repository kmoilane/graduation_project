#include "../shm/shm.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <array>
#include <chrono>
#include <thread>
#include <string>
#include <iomanip>
#include <cstdlib>

int main()
{

    simulation_shm_wrapper shm("../simulation_shm");
    uint8_t target_speed = 200;
    shm.set_conveyor_target_speed(target_speed);
    uint8_t current_speed = 0;
    std::array<double, 10> sensors {};
    std::string ch {"°"};

    while (current_speed != target_speed)
    {
        system("clear");
        current_speed = shm.read_conveyor_speed_sensor();
        for (int i = 1; i <= sensors.size(); i++)
        {
            sensors[i - 1] = (int)shm.read_temperature_sensor(i) / 10.0;
            std::cout << "Sensor" << i << " temperature:\t" << sensors[i] << ch << "C\n";

        }
        std::cout << "Conveyor current speed: " << (int)current_speed  << std::endl;
        std::cout << std::endl;


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } 

    return 0;
}
