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
#include <bitset>

using milliseconds = int16_t;
using time_stamp = std::chrono::system_clock::time_point;

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

void print_control(simulation_shm_wrapper& shm)
{
    milliseconds loop_time = 1000;
    time_stamp start_time{};
    time_stamp end_time{};
    milliseconds sleep_time{};
    while (true)
    {
        start_time = std::chrono::high_resolution_clock::now();
        system("clear");
        std::cout   << "speed" << " | "
                    << "sens1" << " | "
                    << "sens2" << " | "
                    << "sens3" << " | "
                    << "sens4" << " | "
                    << "sens5" << " | "
                    << "sens6" << " | "
                    << "sens7" << " | "
                    << "sens8" << " | "
                    << "sens9" << " | "
                    << "sens10"<< " | " << "\n\n";

        std::cout   << std::setw(4) << (int)shm.read_conveyor_speed_sensor() << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(1)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(2)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(3)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(4)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(5)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(6)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(7)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(8)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(9)/10   << std::setw(4) << " | "
                    << std::setw(4) << shm.read_temperature_sensor(10)/10  << std::setw(5) << " | \n\n";
        
        std::cout << "QC: " << std::setw(8) << std::bitset<16>(shm.read_qc_camera_feed()) << '\n';
        std::cout << "1) Heater element 1 on/off\n";
        std::cout << "2) Heater element 2 on/off\n";
        std::cout << "3) Heater element 3 on/off\n";
        std::cout << "4) Decrease speed\n";
        std::cout << "5) Increase speed\n";

        end_time = std::chrono::high_resolution_clock::now();
        
        auto diff = end_time - start_time;
        auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
        
        sleep_time = loop_time - diff_ms.count();
        if (sleep_time < 0)
            sleep_time = 0;

        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
    }
}

int main()
{

    simulation_shm_wrapper shm("../simulation_shm");
    uint8_t target_speed = 68;
    uint8_t heaters {0b00000111};
    shm.set_conveyor_target_speed(target_speed);
    shm.set_heaters(heaters);
    shm.set_camera_status(0b00000001);
    shm.set_cooler(0b00000001);
    uint8_t current_speed = 0;
    std::array<double, 10> sensors {};
    std::string ch {"°"};
    std::jthread output_thread(print_control, std::ref(shm));

    while (true)
    {
        int input {};
        std::cin >> input;
        if (input >= 1 && input <= 3)
        {
            // Toggle the corresponding bit using bitwise XOR (^) operation
            uint8_t mask = 1 << (input - 1); // Create a mask with the corresponding bit set
            heaters ^= mask;
            shm.set_heaters(heaters);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    } 

    return 0;
}
