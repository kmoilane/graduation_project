# Simulation requirements

The goal here is to generate reasonable data that simulates the production line.
It should account for most common setups, be configurable and deliver the data
in a format similar to what the real sensors would provide. In the best-case
scenario, swapping software from using the simulation to real sensors and
hardware would be as seamless as possible.

Simulations like this are useful in planning/designing hardware systems, and
software testing. Making sure simulation behaves as expected is a
high-priority here, so testing is of paramount importance.

- Create a simulation environment for the production line
    - Recreate data as if it would be from the sensor directly (so, simulate the sensors)
        - You may package the data in any way you want, but the forwarded raw data should be what the sensors provide
    - Take into account environment (e.g. ambient temperature), unknown should be user-configurable
    - Simulate production line functionality
        - Handle sensor / component breakdowns (e.g. send no data if a sensor breaks)
        - Handle quality control data
    - Send sensor data
    - Read control data
    - Act according to control data
    - Way to monitor the simulation state
    - TESTING
        - Super important in tasks like this
    - Then if time allows:
        - Think about Using SHM for communication to actually simulate
          register-based control and how hardware devices work? (This is not too
          hard on linux, but probably requires a day to implement correctly and
          it can be hard to find correct tutorials from the Internet)
        - Documentation
        - Adjustable time scale (e.g. simulation runs faster/slower than real-time)
        - All the fancy stuff you can imagine.

## Extra details
- Heating elements can be assumed to heat the environment with their operational power.
    - It takes 10 seconds to raise their power by 100W, and 20 seconds to lower
    it by 100W. They won't get hotter (raise power) over 2000W each.
    - Heating elements have a chance to break if ran at full power for extended amount of time
- The convoyer has a chance to break if it ran at over 90% speed, but the broken convoyer still functions, just slower.

- The assembly lines have following failure conditions:
    - If heaters are below 3000W of output, shaping will fail automatically
    - If heaters are at full power (6000W total), shaping will succeed 100% of the time
    - Assume linear change in success rate
    - Bolter has outright 0.2% chance to fail
    - The imaging quality control system has 0.005% false positive rate (failed
    piece goes through quality control anyways) and 0.01% false negative rate
    (working piece gets discarded at quality control)
- Heater and cooling systems receive a single uint8_t value for their control
toggles. (So it contains info for all the components if they should be on or off)
- You should make initial version (even if it outputs garbage) available
somewhat quickly, in order to coordinate with others
