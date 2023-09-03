# Test header
# Production Line Simulation

This is a C++ software that simulates a production line environment.
The detailed subject can be found from subject.md file and from the README.md in
the production_line_simulation directory.

**People behind the project**

[kirsimarianne](https://github.com/kirsimarianne)<br>
[holle1234](https://github.com/holle1234)<br>
[kmoilane](https://github.com/kmoilane)


## Summary

Production line produces realistic data from 10 temperature sensors and a quality
control camera which produces bitsets of failed and passed products. Production
line has a conveyor belt, heater process, shaper process, bolter process
and finally the quality control. The production line also has a cooler that
cools the environment.

Conveyor belt's speed can be changed between 0 and
255, which linearily translates to 0-600 units per minute. Quality control camera
processes 16 units at a time and it can handle the maximum of 542 untis per
minute, after which it fails every product. Different processes produce some
heat, processes can break, and they have failrates for the untis that can be
affeted by the environment and previous process steps.

## Dependecies

### OS
This project was coded and tested on Ubuntu 22.04 and Ubuntu 22.10.
To make sure you can run it without probelms, use one of these linux
distribution versions.

### Other dependencies
- CMake (version 3.22 or newer)
    - `sudo apt-get install cmake`
- g++
    - standard: C++20
    - version: Ubuntu 12.2.0-3ubuntu1
    - Comes with build-essentials
- [nlohmann/json](https://github.com/nlohmann/json)
    - `sudo apt-get install nlohmann-json3-dev`
- [doctest](https://github.com/doctest/doctest)
    - This is already in the project since it's a header only

### Optional

- [Tmux](https://github.com/tmux/tmux/wiki) (for runnging run_simulation.sh script that runs simulation, controller
and a little UI all at once in one terminal session) 
    - `sudo apt-get install tmux`

## Running the simulation

The following commands are examples on how to build and run this program.

1. To run the simulation, start by cloning this repository.<br>
    `git clone https://github.com/kmoilane/graduation_project.git Simulation`

2. Go to the build folder and build it with CMake.<br>
    `cd Simulation/production_line_simulation/build`<br>
    `cmake .. && make`

### Run all programs at once (requires tmux)

3. If you want to run all 3 programs (simulation, UI and controller), make sure you
have execute rights for the run script.<br>
    `chmod +x run_simulation.sh`

4. To run all programs with default config, run the run_simulation script.
This works best on full window sized terminal.<br>
    `./run_simulation.sh`

### Run separately

You can also run programs separately, but for simulation to start producing data
you also need to start the controller on a separate terminal.

**Simulation must be ran from build directory to be abled to read config files**

3. Run the simulation inside build directory with desired configuration.
Current options are:<br>
    `./simulation` - starts simulation using config_default.json<br>
    `./simulation config_default.json` - uses files default configuration<br>
    `./simulation config_stop.json` - uses hault state configuration<br>
    `./simulation config_full.json` - uses full speed full power state
    configuration
    
    (More about configuration later on)

4. Open a new terminal and navigate yourself to the build folder, then run
   controller<br>
    `./controller`

*Simulation should now start producing data and you can control the environment with
controller.*

5. (Optional) If you want to see some more data, you can also run ui, which once
   again requires fresh terminal and for you to navigate to build folder. Then
   run ui:<br>
    `./ui`

**To exit the programs properly, use the controllers exit option. This detaches
processes from the shared memory in a controlled way and closes all the programs.** 


## Configuration

We have created 3 starting stages for the simulation as config files. By
default, the simulation uses config_default.json file, if no other file is given.
These files are located in the config folder. You
can also create your own configurations using the same layout as those examples.
The file needs to end with `.json`, it needs to be in the config directory and
it should only contain same fields that are used in the example configurations.
You can play with the field values, and if you go over ranges, the values will be
overwritten with default values.

### Configurable values and ranges

**NOTE** although you can change the values in the configuration, keep in mind
that the simulation will act accordingly and for example with too high
temperatures, everything will break (real production line can't take too much
heat either).

Currently you can change the following values in the configuration within the
ranges mentioned inside the ().

```json
{
    "Simulation" :
    {
        "step_time_ms" : (range: int),
        
        "Conveyor" :
        {
            "speed_current": (range: 0 - 255) 
        },
        "Heater" :
        {
            "temperature_max": (range: double),
            "element_power_levels": [
                (range: 0 - 2000),
                (range: 0 - 2000),
                (range: 0 - 2000)
            ]
        },
        "Bolter" :
        {
            "state": (range: true/false)
        },
        "Shaper" :
        {
            "state": (range: true/false)
        },
        "Camera" :
        {
            "state": (range: true/false)
        },
        "Ambient_temperature" :
        {
            "ambient_temperature" : (range: double)
        },
        "Cooler" :
        {
            "max_power" : (range: double),
            "state" : (range: true/false)
        },
        "temperature_sensor" :
        {
            "break_probability" : (range: double)
        }
    }
}
```


## Shared memory

In case you want to create your own controller software for this simulation, you
can do so by using the shm wrapper class in the shm directory. It has functions
to each piece in the simulation, for both reading and writing data. Simulation
itself points to shared memory file in
`production_line_simulation/simulation_shm`, so make sure to point your
controllers shared memory in the same file.

Shared memory allocates a block of memory, where we have mapped either 1 or 2
bytes of memory for each of the devices depending on the data size that they
each send. You can access these memory addresses by using pointers to these addresses.

### Current shared memory mapping is as follows:

**shm_pointer+0   = temp_sesnsor1 (16 bits/2 bytes)<br>
shm_pointer+2 = temp_sesnsor2 (16 bits/2 bytes)<br>
shm_pointer+4 = temp_sesnsor3 (16 bits/2 bytes)<br>
shm_pointer+6 = temp_sesnsor4 (16 bits/2 bytes)<br>
shm_pointer+8 = temp_sesnsor5 (16 bits/2 bytes)<br>
shm_pointer+10 = temp_sesnsor6 (16 bits/2 bytes)<br>
shm_pointer+12 = temp_sesnsor7 (16 bits/2 bytes)<br>
shm_pointer+14 = temp_sesnsor8 (16 bits/2 bytes)<br>
shm_pointer+16 = temp_sesnsor9 (16 bits/2 bytes)<br>
shm_pointer+18 = temp_sesnsor10 (16 bits/2 bytes)<br>
shm_pointer+20 = conveyor_speed_current (8 bits/1 byte)<br>
shm_pointer+21 = conveyor_speed_target (8 bits/1 byte)<br>
shm_pointer+22 = heater_controls (8 bits/1 byte)<br>
shm_pointer+23 = cooler_control (8 bits/1 byte)<br>
shm_pointer+24 = qc_camera_control (8 bits/1 byte)<br>
shm_pointer+25 = qc_camera_feed (16 bits/2 bytes)<br>
shm_pointer+27 = simulation_on_off (8 bits/1 byte)<br>**

Data sent by temperature sensors come as millivolts between -400 and 1100.
These can be translated to degrees in celsius by following TI-LM35C temperature
sensors data sheet, but basically 10mV = 1 degree Celsius. So the temperature range
that these sesnors can measure is between -40 and 110.

Conveyor speeds are 0-255.

Heaters can be toggled on and off by changing the first 3 bits of the byte as
follows:<br>
heater_1 `0b00000001`<br>
heater_2 `0b00000010`<br>
heater_3 `0b00000100`<br>
Where 1 means on and 0 means off.<br>
All heaters on at the same time looks like this<br>
`0b00000111`<br>
and off<br>
`0b00000000`

Cooler can be toggled on and off by changing the first bit in the byte as follows:<br>
cooler `0b00000001`<br>
Where 1 means on and 0 means off.

Quality control camera can be turned on and off in the same way as cooler.

Quality control camera sends quality feed 16 bits at a time. 16 bits represent
a bitmask where 0 equals good quality product/unit and 1 equals failed unit that
didn't pass quality check for one reason or another. The interval of new bitsets
depends on the conveyors speed, since the camera processes 16 units after which
it sends the new bitmask to the shared memory address.

And finally we have a way to start and close the simulation through controller
via the simulation_on_off address. Simulation starts when the first bit in the
address is 1 and shuts down when the bit turns to 0. This way we can control the
detachment from shared memory and close it properly.




