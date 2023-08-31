#include "includes/simulation.hpp"
#include "includes/communicator.hpp"
#include "includes/utils.hpp"
#include "shm/shm.hpp"

#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <iomanip>
#include <bitset>

const static std::string CONFIG_DEFAULT {"../config/config_default.json"};

bool file_exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::string get_config_path(int argc, const char* argv[])
{
    std::string config_file_path {};
    std::string call_path {};
    size_t n = call_path.rfind("/");
    std::string call_dir = call_path.substr(0, n);
    if (argc == 2)
    {
        config_file_path = call_dir + "../config/" + static_cast<std::string>(argv[1]);
    }
    else if (argc == 1)
        config_file_path = CONFIG_DEFAULT;
    else
    {
        std::cout << "Too many parameters!\n"
                  << "Usage: ./Simulation configuration_file\n";
        exit(0);
    }
    if (file_exists(config_file_path))
    {
        if (std::filesystem::path(config_file_path).extension() != ".json")
        {
            std::cout << "Configuration file must be of type json!\n";
            exit(0);
        }
        return config_file_path;
    }
    std::cout << "Configuration file does not exist!\n"
              << "Run the program from build folder to avoid pathing issues\n";
    exit(0);
}


void print_header(){
    std::cout << "\033c";
    std::cout << "\033[1;47;30mupm " << std::setw(9) << " || " 
              << std::setw(9)  << "heater W  || " 
              << std::setw(9)  << "heater C  || "
              << std::setw(9)  << "heater on || "
              << std::setw(20) << "qc        || "
              << std::setw(9)  << "ambient   || "
              << std::setw(9)  << "cooler C  || "
              << std::setw(9)  << "cooler on || "
              << std::setw(9)  << "cooler W  || " 
              << std::setw(9)  << "time m    || \033[0m" << '\n' << '\n'; 
}

void print_data(Simulation& sim, int step){
    std::cout << std::setprecision(2) << std::fixed << std::boolalpha;
    std::cout << std::setw(9)  << sim.conveyor.get_upm_current()                     << " || "
              << std::setw(9)  << sim.heater.get_power()                             << " || "
              << std::setw(9)  << sim.heater.get_temperature()                       << " || "
              << std::setw(9)  << std::bitset<3>(sim.heater.get_state())             << " || "
              << std::setw(9)  << std::bitset<16>(sim.quality_control.get_output())  << " || "
              << std::setw(9)  << sim.ambient_temperature.get_temperature()          << " || "
              << std::setw(9)  << sim.cooler.get_temperature()                       << " || "
              << std::setw(9)  << sim.cooler.state                                   << " || "
              << std::setw(9)  << sim.cooler.get_power()                             << " || " 
              << std::setw(9)  << (step * sim.step_duration) / 1000.0                << " || " << '\n';

}

int main(int argc, const char *argv[])
{
    // for testing purposes
    // final simulation must take account realtime operation

    Configuration config(get_config_path(argc, &(*argv)));
    Simulation sim(config);
    Communicator communicator;

    uint8_t simulation_on = communicator.shm.read_simulation_status();
    while (!simulation_on)
    {
        simulation_on = communicator.shm.read_simulation_status();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
    }
    std::cout << "Simulation online!\n";


    int i = 0;
    while (true){

        if (!simulation_on){
            std::cout << "Simulation offline.\n";
            break;
        }

        sim.step(); 
        sim.update();

        communicator.send(sim); 
        communicator.listen(sim);

        if (i % 15 == 0){
            print_header();   
        }
        print_data(sim, i);
        
        simulation_on = communicator.shm.read_simulation_status();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ++i;
    }
    
    return 0;
}

