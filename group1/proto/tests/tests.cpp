#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/conveyor.hpp"
#include "../includes/units.hpp"
#include "../includes/config.hpp"
#include "../includes/simulation.hpp"
#include "../includes/temperature_sensor.hpp"
#include <thread>

TEST_CASE("CONVEYOR TESTS")
{
    Configuration config;
    config.data["Simulation"]["Conveyor"]["speed_current"]  = 150;
    devices::Conveyor conveyor;
    conveyor.configure(config);
    SUBCASE("TESTING SET AND GET CURRENT SPEED")
    {
        CHECK(conveyor.get_speed_current() == 150);
        conveyor.set_speed_current(50);
        CHECK(conveyor.get_speed_current() == 50);
        conveyor.set_speed_current(0);
        CHECK(conveyor.get_speed_current() == 0);
        conveyor.set_speed_current(255);
        CHECK(conveyor.get_speed_current() == 255);
    }
    SUBCASE("TESTING SET AND GET CURRENT SPEED")
    {
        conveyor.set_speed_target(255);
        CHECK(conveyor.get_speed_target() == 255);
        conveyor.set_speed_target(125);
        CHECK(conveyor.get_speed_target() == 125);
        conveyor.set_speed_target(0);
        CHECK(conveyor.get_speed_target() == 0);
        conveyor.set_speed_target(1);
        CHECK(conveyor.get_speed_target() == 1);
    }
    SUBCASE("TESTING UPDATE")
    {
        conveyor.set_speed_target(255);
        conveyor.update(450'000);
        CHECK(conveyor.get_speed_current() == 255);
        CHECK(conveyor.get_upm_current() == 600);
        CHECK(conveyor.get_temperature() == doctest::Approx(11.2));
        conveyor.set_speed_target(254);
        conveyor.update();
        CHECK(conveyor.get_speed_current() == 255);
        CHECK(conveyor.get_upm_current() == 599);

        conveyor.update();
        CHECK(conveyor.get_speed_current() == 255);
        CHECK(conveyor.get_upm_current() == 598);

        conveyor.update();
        CHECK(conveyor.get_speed_current() == 254);
        CHECK(conveyor.get_upm_current() == 597);
        conveyor.set_speed_target(0);
        conveyor.update(600'000);
        CHECK(conveyor.get_speed_current() == 0);
        CHECK(conveyor.get_upm_current() == 0);
        for (int i = 0; i < 10; i++)
        {
            conveyor.update(1500);
            CHECK(conveyor.get_speed_current() == 0);
            CHECK(conveyor.get_upm_current() == 0);
        }
        conveyor.set_speed_target(255);
        conveyor.update(600'000);
        CHECK(conveyor.get_speed_current() == 255);
        CHECK(conveyor.get_upm_current() == 600);
        for (int i = 0; i < 10; i++)
        {
            conveyor.update(1500);
            CHECK(conveyor.get_speed_current() == 255);
            CHECK(conveyor.get_upm_current() == 600);
        }
    }
    SUBCASE("TESTING UPDATE AND TEMPERATURE CHANGES")
    {
        config.data["Simulation"]["Conveyor"]["speed_current"]  = 0;
        config.data["Simulation"]["step_time_ms"] = 1000;
        conveyor.configure(config);
        conveyor.set_speed_target(1);

        conveyor.update();
        CHECK(conveyor.get_speed_current() == 1);
        CHECK(conveyor.get_upm_current() == 1);
        double temp = 382.75 * ((100 - (100 - 56) * 1 / 60.0) / 100) / 1000;
        CHECK(conveyor.get_temperature() == doctest::Approx(temp));

        conveyor.set_speed_target(0);
        conveyor.update();
        CHECK(conveyor.get_speed_current() == 0);
        CHECK(conveyor.get_upm_current() == 0);
        temp = 350 * ((100 - (100 - 56) * 0 / 60.0) / 100) / 1000;
        CHECK(conveyor.get_temperature() == doctest::Approx(temp));

        conveyor.set_speed_target(30);
        conveyor.update(60'000);
        CHECK(conveyor.get_upm_current() == 60);
        temp = (350 + (60 * 32.75)) * ((100 - (100 - 56) * 60 / 60.0) / 100) / 1000;
        CHECK(conveyor.get_temperature() == doctest::Approx(temp));
        
        conveyor.update();
        CHECK(conveyor.get_upm_current() == 61);
        CHECK(conveyor.get_speed_current() == doctest::Approx(26));
        double power = 350 + (61 * 32.75);
        temp = power * (56.0 / 100) / 1000;
        CHECK(conveyor.get_temperature() == doctest::Approx(temp));
    }
}

TEST_CASE("HEATER TESTS")
{
    Configuration config;
    config.data["Simulation"]["Heater"]["temperature_max"]  = 100;
    Simulation sim(config);

    SUBCASE("CORRECT POWER UPDATE"){
        // without update (init point)
        CHECK(sim.heater.get_power() == 0);
        sim.heater.set_state(0b00000111);
        sim.update(1000);
        CHECK(sim.heater.get_power() == doctest::Approx(30));
        sim.update(10'000);
        CHECK(sim.heater.get_power() == doctest::Approx(330));
        sim.heater.set_state(0b00000110);
        sim.update(1000);
        CHECK(sim.heater.get_power() == doctest::Approx(345));
        sim.update(250'000);
        CHECK(sim.heater.get_power() == doctest::Approx(4000));
        sim.heater.set_state(0b00000000);
        sim.update(1000);
        CHECK(sim.heater.get_power() == doctest::Approx(3990));
        sim.update(398'000);
        CHECK(sim.heater.get_power() == doctest::Approx(10));
    }
    SUBCASE("TESTING BREAKPOINT")
    {
        sim.heater.set_state(0b00000111);
        sim.heater.force_power_levels(2000, 2000, 2000);
        CHECK(sim.heater.get_state() == 0b00000111);
        sim.update(600'000);
        CHECK(sim.heater.get_state() == 0b00000000);
    }
    SUBCASE("TESTING TEMPERATURE")
    {
        CHECK(sim.heater.get_temperature() == 0);
        sim.heater.set_state(0b00000111);
        sim.heater.force_power_levels(2500, 2500, 2500);
        CHECK(sim.heater.get_power() == 6000);
        CHECK(sim.heater.get_temperature() == 100);
        sim.heater.set_state(0b00000000);
        sim.update(200'000);
        CHECK(sim.heater.get_temperature() == 50);
        sim.update(250'000);
        CHECK(sim.heater.get_temperature() == 0);
    }
}

TEST_CASE("COOLER TESTS")
{
    Configuration config;
    config.data["Simulation"]["Cooler"]["max_power"]  = 500;
    config.data["Simulation"]["Cooler"]["state"]  = true;
    Simulation sim(config);

    SUBCASE("CORRECT POWER UPDATE")
    {
        // without update (init point)
        CHECK(sim.cooler.get_power() == 0);
        sim.cooler.set_state(true);
        sim.update(1000);
        CHECK(sim.cooler.get_power() == doctest::Approx(2.5));
        sim.update(10'000);
        CHECK(sim.cooler.get_power() == doctest::Approx(27.5));
        sim.cooler.set_state(false);
        sim.update(1000);
        CHECK(sim.cooler.get_power() == doctest::Approx(26.25));
        sim.cooler.set_state(true);
        sim.update(400'000);
        CHECK(sim.cooler.get_power() == doctest::Approx(500));
        sim.cooler.set_state(false);
        sim.update(1000);
        CHECK(sim.cooler.get_power() == doctest::Approx(498.75));
        sim.update(398'000);
        CHECK(sim.cooler.get_power() == doctest::Approx(1.25));
    }
    SUBCASE("TESTING BREAKPOINT")
    {
        sim.cooler.set_state(true);
        CHECK(sim.cooler.get_state() == true);
        sim.update(200'000);
        CHECK(sim.cooler.get_power() == 500);
        sim.update(60'000);
        CHECK(sim.cooler.get_state() == false);
    }
    SUBCASE("TESTING TEMPERATURE")
    {
        sim.cooler.set_state(0b00000000);
        sim.update(400'000);
        CHECK(sim.cooler.get_temperature() == 0);
        sim.cooler.set_state(0b0000001);
        sim.update(200'000);
        CHECK(sim.cooler.get_power() == 500);
        CHECK(sim.cooler.get_temperature() == -10);
        sim.cooler.set_state(0b00000000);
        sim.update(200'000);
        CHECK(sim.cooler.get_temperature() == -5);
        sim.update(250'000);
        CHECK(sim.cooler.get_temperature() == 0);
    }
}

TEST_CASE("TEMPERATURE SENSOR")
{
    std::vector<TemperatureSensor> temp_sensors{{0.4},
                                                {0.45},
                                                {0.5},
                                                {0.45},
                                                {0.4},
                                                {0.35},
                                                {0.3},
                                                {0.25},
                                                {0.2},
                                                {0.15}};
    SUBCASE("UPDATE")
    {
        temp_sensors[0].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[0].get_temperature() == 70);
        temp_sensors[1].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[1].get_temperature() == 75);
        temp_sensors[2].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[2].get_temperature() == 80);
        temp_sensors[3].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[3].get_temperature() == 75);
        temp_sensors[4].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[4].get_temperature() == 70);
        temp_sensors[5].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[5].get_temperature() == 65);
        temp_sensors[6].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[6].get_temperature() == 60);
        temp_sensors[7].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[7].get_temperature() == 55);
        temp_sensors[8].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[8].get_temperature() == 50);
        temp_sensors[9].update(20, 100, 15, -5.08);
        CHECK(temp_sensors[9].get_temperature() == 45);
    }
    SUBCASE("MIN AND MAX TEMEPRATURE")
    {
        temp_sensors[0].update(40, 200, 30, -5);
        CHECK(temp_sensors[0].get_temperature() == 110);
        temp_sensors[0].update(0, 0, 0, -50);
        CHECK(temp_sensors[0].get_temperature() == -40);
    }
    SUBCASE("BROKEN SENSOR")
    {
        temp_sensors[0].set_probability(100);
        CHECK(temp_sensors[0].get_temperature() == 0);
    }
}
