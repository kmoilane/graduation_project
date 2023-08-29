#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/simulation.hpp"


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
