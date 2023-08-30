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
    Simulation sim{config};

    SUBCASE("TESTING SET AND GET CURRENT SPEED")
    {
        CHECK(sim.conveyor.get_speed_current() == 150);
        sim.conveyor.set_speed_current(50);
        CHECK(sim.conveyor.get_speed_current() == 50);
        sim.conveyor.set_speed_current(0);
        CHECK(sim.conveyor.get_speed_current() == 0);
        sim.conveyor.set_speed_current(255);
        CHECK(sim.conveyor.get_speed_current() == 255);
    }
    SUBCASE("TESTING SET AND GET CURRENT SPEED")
    {
        sim.conveyor.set_speed_target(255);
        CHECK(sim.conveyor.get_speed_target() == 255);
        sim.conveyor.set_speed_target(125);
        CHECK(sim.conveyor.get_speed_target() == 125);
        sim.conveyor.set_speed_target(0);
        CHECK(sim.conveyor.get_speed_target() == 0);
        sim.conveyor.set_speed_target(1);
        CHECK(sim.conveyor.get_speed_target() == 1);
    }
    SUBCASE("TESTING UPDATE")
    {
        sim.conveyor.set_speed_target(255);
        sim.conveyor.update(450'000);
        CHECK(sim.conveyor.get_speed_current() == 255);
        CHECK(sim.conveyor.get_upm_current() == 600);
        CHECK(sim.conveyor.get_temperature() == doctest::Approx(11.2));
        sim.conveyor.set_speed_target(254);
        sim.update();
        CHECK(sim.conveyor.get_speed_current() == 255);
        CHECK(sim.conveyor.get_upm_current() == 599);

        sim.update();
        CHECK(sim.conveyor.get_speed_current() == 255);
        CHECK(sim.conveyor.get_upm_current() == 598);

        sim.update();
        CHECK(sim.conveyor.get_speed_current() == 254);
        CHECK(sim.conveyor.get_upm_current() == 597);
        sim.conveyor.set_speed_target(0);
        sim.update(600'000);
        CHECK(sim.conveyor.get_speed_current() == 0);
        CHECK(sim.conveyor.get_upm_current() == 0);
        for (int i = 0; i < 10; i++)
        {
            sim.update(1500);
            CHECK(sim.conveyor.get_speed_current() == 0);
            CHECK(sim.conveyor.get_upm_current() == 0);
        }
        sim.conveyor.set_speed_target(255);
        sim.update(600'000);
        CHECK(sim.conveyor.get_speed_current() == 255);
        CHECK(sim.conveyor.get_upm_current() == 600);

        for (int i = 0; i < 10; i++)
        {
            sim.update(1500);
            if (sim.conveyor.broken)
                break;
            CHECK(sim.conveyor.get_speed_current() == 255);
            CHECK(sim.conveyor.get_upm_current() == 600);
        }
    }
    SUBCASE("TESTING UPDATE AND TEMPERATURE CHANGES")
    {
        // config ongelma ylhäältä

        config.data["Simulation"]["Conveyor"]["speed_current"]  = 0;
        config.data["Simulation"]["step_time_ms"] = 1000;
        Simulation sim{config};
        sim.conveyor.set_speed_target(1);

        sim.update();
        CHECK(sim.conveyor.get_speed_current() == 1);
        CHECK(sim.conveyor.get_upm_current() == 1);
        double temp = 382.75 * ((100 - (100 - 56) * 1 / 60.0) / 100) / 1000;
        CHECK(sim.conveyor.get_temperature() == doctest::Approx(temp));

        sim.conveyor.set_speed_target(0);
        sim.update();
        CHECK(sim.conveyor.get_speed_current() == 0);
        CHECK(sim.conveyor.get_upm_current() == 0);
        temp = 350 * ((100 - (100 - 56) * 0 / 60.0) / 100) / 1000;
        CHECK(sim.conveyor.get_temperature() == doctest::Approx(temp));

        sim.conveyor.set_speed_target(30);
        sim.update(60'000);
        CHECK(sim.conveyor.get_upm_current() == 60);
        temp = (350 + (60 * 32.75)) * ((100 - (100 - 56) * 60 / 60.0) / 100) / 1000;
        CHECK(sim.conveyor.get_temperature() == doctest::Approx(temp));
        
        sim.update();
        CHECK(sim.conveyor.get_upm_current() == 61);
        CHECK(sim.conveyor.get_speed_current() == doctest::Approx(26));
        double power = 350 + (61 * 32.75);
        temp = power * (56.0 / 100) / 1000;
        CHECK(sim.conveyor.get_temperature() == doctest::Approx(temp));
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
    std::vector<TemperatureSensor> temp_sensors{{0.5, 1.0},
                                                {0.45, 0.99},
                                                {0.4, 0.98},
                                                {0.35, 0.97},
                                                {0.3, 0.96},
                                                {0.25, 0.95},
                                                {0.2, 0.94},
                                                {0.15, 0.93},
                                                {0.1, 0.92},
                                                {0.05, 0.91}};
    SUBCASE("UPDATE")
    {
        temp_sensors[0].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 79.00) <= doctest::Approx(temp_sensors[0].get_temperature()));
        CHECK((0.500 + 80.00) >= doctest::Approx(temp_sensors[0].get_temperature()));
        temp_sensors[1].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 74.00) <= doctest::Approx(temp_sensors[1].get_temperature()));
        CHECK((0.500 + 75.00) >= doctest::Approx(temp_sensors[1].get_temperature()));
        temp_sensors[2].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 69.00) <= doctest::Approx(temp_sensors[2].get_temperature()));
        CHECK((0.500 + 70.00) >= doctest::Approx(temp_sensors[2].get_temperature()));
        temp_sensors[3].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 63.00) <= doctest::Approx(temp_sensors[3].get_temperature()));
        CHECK((0.500 + 64.00) >= doctest::Approx(temp_sensors[3].get_temperature()));
        temp_sensors[4].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 58.00) <= doctest::Approx(temp_sensors[4].get_temperature()));
        CHECK((0.500 + 59.00) >= doctest::Approx(temp_sensors[4].get_temperature()));
        temp_sensors[5].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 53.00) <= doctest::Approx(temp_sensors[5].get_temperature()));
        CHECK((0.500 + 54.00) >= doctest::Approx(temp_sensors[5].get_temperature()));
        temp_sensors[6].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 48.00) <= doctest::Approx(temp_sensors[6].get_temperature()));
        CHECK((0.500 + 49.00) >= doctest::Approx(temp_sensors[6].get_temperature()));
        temp_sensors[7].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 43.00) <= doctest::Approx(temp_sensors[7].get_temperature()));
        CHECK((0.500 + 44.00) >= doctest::Approx(temp_sensors[7].get_temperature()));
        temp_sensors[8].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 39.00) <= doctest::Approx(temp_sensors[8].get_temperature()));
        CHECK((0.500 + 40.00) >= doctest::Approx(temp_sensors[8].get_temperature()));
        temp_sensors[9].update(20, 100, 15, -5.08);
        CHECK((-0.500 + 34.00) <= doctest::Approx(temp_sensors[9].get_temperature()));
        CHECK((0.500 + 35.00) >= doctest::Approx(temp_sensors[9].get_temperature()));
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

