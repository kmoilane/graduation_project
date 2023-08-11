# graduation_project

## Production line tech specs

### Sensors

- Production line convoyer has variable speed, measured in units pushed through/minute.
  - The maximum speed of the convoyer is 600 units / minute
  - The minimum speed of the convoyer is 0 units / minute
  - The convoyer uses power linearly from 350W to 20000W, depending on the speed of the production line (350W at stop, 20000W at full speed)
  - The convoyer works perfectly up to 80°C
  - Convoyer's efficiency is 0% at minimum speed and rises linearly to 44% at 10% speed, after which it is constant
    - efficiency here measures how much of the power is converted to heat (100% at min speed, 56% at 10% speed)
  - The convoyer has a sensor that reports its current speed in linear scale as a 8-bit unsigned integer (0 = 0u/m, 255 = 600u/m)

- Production line has three steps of assembly
  - A heater heats the product for shaping
    - Formed of 3 2000W heating elements, individually controllable
  - A shaper shaper
  - A bolter bolts

- Cooling system
  - The production line will be equipped with a cooling system, that can dissipate some heat, but exact details are still up in the air (or changing every week)
  
- 10 TI-LM35C temperature sensors are spaced out evenly across the product line, which measure the temperature.

- A DFR0833/OpenMV-H7 camera (OV7725 image sensor) is used at the end of the production line for quality control

  - The quality control system can handle maximum of 542 units / minute, after which it fails everything
  - The quality control system measures 16 units at one go, before checking the next 16
  - The incoming data from the camera is just uint16_t bitmask of failed QC checks

## Controls
- Convoyer target speed can be adjusted by user from 0 (0 / units) to 255 (600 units / minute)
  - Convoyer's acceleration/deceleration is at maximum 1 units/minute /second.
- Heating elements are simply large metal resistors that can be turned on or off individually.
- Cooling system can be turned on/off
- QC camera can be turned on/off
