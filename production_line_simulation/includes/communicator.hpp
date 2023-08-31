#if !defined(COMMUNICATOR)
#define COMMUNICATOR


#include "units.hpp"
#include "heater.hpp"
#include "ambient_temperature.hpp"
#include "conveyor.hpp"
#include "simulation.hpp"
#include "../includes/config.hpp"
#include "../shm/shm.hpp"


class Communicator{

public:
    
    simulation_shm_wrapper shm {"../simulation_shm"};

    Communicator(){}

    // set simulation variables here
    void listen(Simulation& sim)
    {
        sim.conveyor.set_speed_target(shm.read_conveyor_target_speed());
        sim.heater.set_state(shm.read_heaters());
        sim.cooler.set_state(shm.read_cooler());
        sim.quality_control.set_state(shm.read_camera_status());
    }

    // send simulation variables here
    void send(const Simulation& sim)
    {
        shm.set_conveyor_speed_sensor(sim.conveyor.get_speed_current());

        for (size_t i = 0; i < sim.temp_sensors.size(); i++)
        {
            shm.set_temperature_sensor(i + 1, sim.temp_sensors[i].get_voltage());
        }

        shm.set_qc_camera_feed(sim.quality_control.get_output());
    }
};



#endif // COMMUNICATOR
