#ifndef SIMULATION_HPP
#define DIMULATION_HPP

#include "ti_lm35c.hpp"
#include <cstdint>

namespace Simulation
{

    class Production_line
    {
        public:
            Production_line() = default;
            Conveyor    c_belt;
            Heater      heater;
            Shaper      shaper;
            Bolter      bolter;
            Cooler      cooler;
            Camera      camera;

            LM35C       sensor1;
            LM35C       sensor2;
            LM35C       sensor3;
            LM35C       sensor4;
            LM35C       sensor5;
            LM35C       sensor6;
            LM35C       sensor7;
            LM35C       sensor8;
            LM35C       sensor9;
            LM35C       sensor10;

        private:

    };

    class Conveyor
    {
        /*
        ** The convoyer uses power linearly from 350W to 20000W, depending on
        ** the speed of the production line (350W at stop, 20000W at full speed)

        ** The convoyer works perfectly up to 80°C

        ** Convoyer's efficiency is 0% at minimum speed and rises linearly to
        ** 44% at 10% speed (8800W), after which it is constant
        **      - efficiency here measures how much of the power is converted to
        **        heat (100% at min speed, 56% at 10% speed)
        */
        public:
            void set_speed(uint8_t new_speed)
            {
                /*
                **  Decrease/Increase speed gradually (max 1 unit/min/sec)
                **  And also adjust upm accordingly speed 0 = 0 upm, 255 = 600 upm
                */
            }

        private:
            uint8_t     target_speed;
            uint8_t     speed;
            uint16_t    upm; // units per minute
    };

    class Heating_element
    {
        public:
            bool        is_on;
            uint16_t    power;
    };

    class Heater
    {
        constexpr static uint8_t ELEM1_ONLINE = 0b1;
        constexpr static uint8_t ELEM2_ONLINE = 0b10;
        constexpr static uint8_t ELEM3_ONLINE = 0b100;
        public:
            Heater() = default;
            void set_status(uint8_t controls);
        private:
            uint8_t         status;
            Heating_element elem1;
            Heating_element elem2;
            Heating_element elem3;
            uint16_t        total_power{elem1.power+elem2.power+elem3.power};

    };

    class Shaper
    {
        public:
            bool is_on;
    };

    class Bolter
    {
        public:
            const float fail_rate = 0.2; // %
    };

    class Cooler_element
    {
        public:
            bool        is_on;
            uint16_t    power;
    };

    class Cooler
    {
        public:
            void toggle(uint8_t controls);
        private:
            uint8_t         status;
            Cooler_element  elem1;
            Cooler_element  elem2;
            Cooler_element  elem3;
            uint16_t        total_power{elem1.power+elem2.power+elem3.power};
    };

    class Camera
    {
        public:
            void toggle(bool power) { is_on = power; }
            void send_data() { /*Save qc_bitmask to some memory address etc*/ }

        private:
            bool        is_on;
            uint16_t    qc_bitmask;
            constexpr static uint16_t upm = 542; // units per minute (16 units at a time)
            constexpr static float false_positives = 0.005; // %
            constexpr static float false_negatives = 0.01;  // %
    };

}

#endif
