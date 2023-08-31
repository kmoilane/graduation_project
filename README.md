# Production Line Simulation

This is a C++ software that simulates a production line environment.
The detailed subject can be found from subject.md file and from the README.md in
the production_line_simulation directory.

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

1. To run the simulation, start by cloning this repository.

    `git clone https://github.com/kmoilane/graduation_project.git Simulation`

2. Go to the build folder and build it with CMake.

    `cd Simulation/production_line_simulation/build`

    `cmake .. && make`

### Run all programs at once (requires tmux)

3. If you want to run all 3 programs (simulation, UI and controller), make sure you
have execute rights for the run script.
    
    `chmod +x run_simulation.sh`

4. To run all programs with default config, run the run_simulation script.
This works best on full window sized terminal.

    `./run_simulation.sh`

### Run separately

You can also run programs separately, but for simulation to start producing data
you also need to start the controller on a separate terminal.

**Simulation must be ran from build directory to be abled to read config files**

3. Run the simulation inside build directory with desired configuration.
Current options are:

    `./simulation` - starts simulation using config_default.json

    `./simulation config_default.json` - uses files default configuration

    `./simulation config_stop.json` - uses hault state configuration

    `./simulation config_full.json` - uses full speed full power state
    configuration
    
    (More about configuration later on)

4. Open a new terminal and navigate yourself to the build folder, then run
   controller

    `./controller`

Simulation should start producing data and you can control the environment with
control ui.

5. (Optional) If you want to see some more data, you can also run ui, which once
   again requires fresh terminal and for you to navigate to build folder. Then
   run ui:

    `./ui`

**To exit the program properly, use the controllers exit option. This detaches
programs from the shared memory in a controlled way.** 


## Configuration

We have created 3 starting stages for the simulation as config files. By
default, the simulation uses config_default.json file, if no other file is given.
These files are located in the config folder. You
can also create your own configurations using the same layout as those examples.
The file needs to end with `.json`, it needs to be in the config directory and
it should only contain same fields that are used in the example configurations.
You can play with the field values, and if you go over ranges, the values will be
overwritten with default values. Ranges for different variable values, will be
posted here later... :)








