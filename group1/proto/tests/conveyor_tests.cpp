#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/conveyor.hpp"
#include "../includes/units.hpp"
#include "../includes/config.hpp"
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
