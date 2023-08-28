#ifndef SHM_HPP
#define SHM_HPP

#include <cstdint>
#include <iostream>
#include <string>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

constexpr int16_t SHM_ERROR{ -1 };


class simulation_shm_wrapper
{
public:

    simulation_shm_wrapper() = default;
    simulation_shm_wrapper(const std::string& new_filename, std::size_t new_block_size = 4096);
    ~simulation_shm_wrapper();

    simulation_shm_wrapper(const simulation_shm_wrapper&) = delete;
    simulation_shm_wrapper(simulation_shm_wrapper&) = default;

    simulation_shm_wrapper& operator=(const simulation_shm_wrapper&) = delete;
    simulation_shm_wrapper& operator=(simulation_shm_wrapper&&) = default;

    int16_t read_temperature_sensor(uint8_t index) const;
    uint8_t read_conveyor_speed_sensor() const;
    uint16_t read_qc_camera_feed() const;

    void set_conveyor_target_speed(uint8_t target_speed);
    void set_heaters(uint8_t heaters);
    void set_cooler(uint8_t cooler);
    void set_camera_status(uint8_t camera_status);

    void set_temperature_sensor(int index, uint16_t value);
    void set_conveyor_speed_sensor(uint8_t speed);
    void set_qc_camera_feed(uint16_t feed);

    uint8_t read_conveyor_target_speed();
    uint8_t read_heaters();
    uint8_t read_cooler();
    uint8_t read_camera_status();

    uint8_t* value_ptr;

private:
    std::string filename;
    std::size_t block_size;
    void* ptr_to_memory;

    int get_shared_block(std::size_t size);
    void* attach_memory_block();
    //Detach the memory block from process, and return boolean if succesful or not
    bool detach_memory_block();
    bool destroy_memory_block();

};


#endif
