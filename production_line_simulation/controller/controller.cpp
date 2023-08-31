#include "../shm/shm.hpp"
#include <algorithm>
#include <array>
#include <iostream>
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

void clear_cin()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

   
void print_controls()
{
    std::cout << "\033c";
    std::cout << "\033[1;32mSimulation controls\033[0m\n";
    std::cout << "1) Heater 1 on/off\n";
    std::cout << "2) Heater 2 on/off\n";
    std::cout << "3) Heater 3 on/off\n";
    std::cout << "4) Cooler on/off\n";
    std::cout << "5) Camera on/off\n";
    std::cout << "6) Set speed\n";
    std::cout << "7) Exit\n";
}

void toggle_heater(simulation_shm_wrapper& shm, int heater)
{
    uint8_t heaters = shm.read_heaters();
    uint8_t mask = 1 << (heater - 1);
    heaters ^= mask;
    shm.set_heaters(heaters);
    std::cout << "Heater state changed!\n";
}

void toggle_cooler(simulation_shm_wrapper& shm)
{
    uint8_t cooler = shm.read_cooler();
    uint8_t mask = 1 << (0);
    cooler ^= mask;
    shm.set_cooler(cooler);
    std::cout << "Cooler state changed!\n";
}

void toggle_camera(simulation_shm_wrapper& shm)
{
    uint8_t camera = shm.read_camera_status();
    uint8_t mask = 1 << (0);
    camera ^= mask;
    shm.set_camera_status(camera);
    std::cout << "Camera state changed!\n";
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

void set_speed(simulation_shm_wrapper& shm)
{
    std::string input_str {};
    std::cout << "Enter new target speed (0-255): ";
    while(true)
    {
        std::getline(std::cin, input_str);
        if (std::cin.eof())
        {
            std::cout << "Ctrl + D pressed. Returning..." << std::endl;
            clear_cin();
            return;
        }
        else if (is_number(input_str))
        {
            int value = std::stoi(input_str);
            if (value >= 0 && value <= 255)
            {
                shm.set_conveyor_target_speed(value);
                std::cout << "Target speed set to: " << value << "\n";
                return;
            }
            else
                std::cout << "Invalid value\n";
            return;
        }
        std::cout << "Value must be a whole number between 0 and 255 (inclusive)\n";
    }
}

int controller(simulation_shm_wrapper& shm)
{  
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        print_controls();
        int input{};
        std::cin >> input;
        if (!std::cin || (input > 7  && input < 1))
        {
            if (std::cin.eof())
                return 0;
            clear_cin();
            std::cout << "\nInvalid option!\n";
            return 1;
        }
        else if (input >= 1 && input <= 3)
            toggle_heater(shm, input);
        else if (input == 4)
            toggle_cooler(shm);
        else if (input == 5)
            toggle_camera(shm);
        else if (input == 6)
            set_speed(shm);
        else if (input == 7)
            return 0;
    }
}

int main()
{
    simulation_shm_wrapper shm("../simulation_shm");
    uint8_t target_speed = 200;
    shm.set_conveyor_target_speed(target_speed);
    shm.set_heaters(0b00000111);
    shm.set_camera_status(0b00000001);
    shm.set_cooler(0b00000001);
    shm.set_simulation_status(0b00000001);
    uint8_t current_speed = 0;
    std::array<double, 10> sensors {};
    std::string ch {"°"};

    while(true)
    {
        if (controller(shm) == 0)
            break;
    }
    shm.set_simulation_status(0b00000000);
    return 0;
}
