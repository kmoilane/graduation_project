#include "includes/simulation.hpp"
#include "includes/communicator.hpp"
#include "includes/utils.hpp"
#include "shm/shm.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>



int main(int argc, const char *argv[])
{
    // for testing purposes
    // final simulation must take account realtime operation

    Configuration config("../config/config_full.json");
    Simulation sim(config);
    Communicator communicator;

    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(2000, 2000, 2000);

    sim.quality_control.set_state(true);


    std::cout   << "upm       || " 
                << "heater W  || " 
                << "qc        || "
                << "heater on || "
                << "ambient   || "
                << "heater C  || "
                << "time ms   || "
                << "cooler C  || "
                << "cooler on || "
                << "cooler W  || " << '\n' << '\n'; 
    std::cout << std::setprecision(2) << std::fixed << std::boolalpha;

    // get step time from configuration
    auto config_step_time = config.data["Simulation"]["step_time_ms"];
    int step_time_ms = config_step_time.is_null() ? 1000 : int(config_step_time);

    uint8_t simulation_on = communicator.shm.read_simulation_status();
    while (!simulation_on)
    {
        simulation_on = communicator.shm.read_simulation_status();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
    }
    std::cout << "Simulation online!\n";

    int i = 0;
    while (true)
    {
        if (!simulation_on)
        {
            std::cout << "Simulation offline.\n";
            break;
        }
        sim.step(); // set step time
        sim.update();

        communicator.send(sim);   // ideally in a another thread
        communicator.listen(sim); // ideally in a another thread
        simulation_on = communicator.shm.read_simulation_status();

        std::cout << std::setw(9) << sim.conveyor.get_upm_current()                << " || "
                  << std::setw(9) << sim.heater.get_power()                        << " || "
                  << std::setw(16) << std::bitset<16>(sim.quality_control.get_output())   << " || "
                  << std::setw(9) << std::bitset<8>(sim.heater.get_state())        << " || "
                  << std::setw(9) << sim.ambient_temperature.get_temperature()     << " || "
                  << std::setw(9) << sim.heater.get_temperature()                  << " || "
                  << std::setw(9) << (i * step_time_ms) / 1000.0                     << " || "
                  << std::setw(9) << sim.cooler.get_temperature()                  << " || "
                  << std::setw(9) << sim.cooler.state                              << " || "
                  << std::setw(9) << sim.cooler.get_power()                        << " || " << '\n';


        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        ++i;
    }
    
    return 0;
}


