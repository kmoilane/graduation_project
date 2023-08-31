#include <array>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include "../shm/shm.hpp"

template <typename T>
void print_bits(const T& value)
{
    using decay_type = std::decay_t<T>;

    for (int i = sizeof(decay_type) * 8 - 1; i >= 0; --i)
    {
        std::cout << ((value & (1ULL << i)) ? 1 : 0);
        if (i % 8 == 0)
            std::cout << ' ';
    }

    std::cout << '\n';
}

int main()
{
    simulation_shm_wrapper shm("../simulation_shm");

    while(!shm.read_simulation_status())
    {
        std::cout << "\033c";
        std::cout << "\033[1;31mSimulation is offline!\033[0m\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    uint8_t current_speed {};
    std::array<double, 10> sensors {};
    std::string deg {"°"};
    while (true)
    {
        std::cout << "\033c";
        std::cout << "\033[1;32mSimulation online!\033[0m\n";
        if (shm.read_simulation_status() == 0)
        {
            std::cout << "\033c";
            std::cout << "\033[1;31mSimulation is offline!\033[0m\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            break;
        }
        current_speed = shm.read_conveyor_speed_sensor();
        for (int i = 1; i <= sensors.size(); i++)
        {
            sensors[i - 1] = (int)shm.read_temperature_sensor(i) / 10.0;
            std::cout << "\033[36mSensor" << i << "\033[0m:\t" << sensors[i - 1] << deg << "C\t\t";
            if (i % 2 == 0)
                std::cout << '\n';

        }
        std::cout << "Conveyor current speed: " << (int)current_speed  << std::endl;
        std::cout << "QC Data: ";
        print_bits(shm.read_qc_camera_feed());
        std::cout << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
