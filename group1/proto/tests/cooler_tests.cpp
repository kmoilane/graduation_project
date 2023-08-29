#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/simulation.hpp"

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
