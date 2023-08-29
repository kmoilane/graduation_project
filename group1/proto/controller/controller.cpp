#include "../shm/shm.hpp"
#include <iostream>
#include <array>
#include <chrono>
#include <thread>
#include <string>
#include <iomanip>
#include <cstdlib>

template <typename T>
void print_bits(const T& value)
{
    using decay_type = std::decay_t<T>;

    std::cout << "0b";
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
    uint8_t target_speed = 200;
    shm.set_conveyor_target_speed(target_speed);
    shm.set_heaters(0b00000111);
    shm.set_camera_status(0b00000001);
    shm.set_cooler(0b00000001);
    uint8_t current_speed = 0;
    std::array<double, 10> sensors {};
    std::string ch {"°"};

    for(int i = 0; i < 500; i++)
    {
        system("clear");
        current_speed = shm.read_conveyor_speed_sensor();
        for (int i = 1; i <= sensors.size(); i++)
        {
            sensors[i - 1] = (int)shm.read_temperature_sensor(i) / 10.0;
            std::cout << "Sensor" << i << " temperature:\t" << sensors[i - 1] << ch << "C\n";

        }
        std::cout << "Conveyor current speed: " << (int)current_speed  << std::endl;
        std::cout << "QC Data: ";
        print_bits(shm.read_qc_camera_feed());
        std::cout << "\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } 

    return 0;
}
