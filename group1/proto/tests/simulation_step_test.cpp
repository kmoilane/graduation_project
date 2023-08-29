#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/simulation.hpp"
#include <limits>
#include <bitset>
#include "product.hpp"



TEST_CASE("SIMULATION STEP TESTS"){
    Configuration config;
    config.data["Simulation"]["Conveyor"]["speed_current"] = 127;
    config.data["Simulation"]["Camera"]["state"] = true;
    config.data["Simulation"]["Bolter"]["state"] = true;
    config.data["Simulation"]["Shaper"]["state"] = true;

    Simulation sim(config);
    sim.heater.set_state(0b00000111);
    sim.heater.force_power_levels(2000, 2000, 2000);
    sim.update(2000);

    SUBCASE("QUALITY_CONTROL"){
        // start should output zero
        CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());

        // should most likely return a good product 
        CHECK(sim.bolter.process(ProductState::good) == ProductState::good);
        // should most likely return a good product 
        CHECK(sim.shaper.process(ProductState::good) == ProductState::good);
        // should most likely return a good product 
        CHECK(sim.heater.process(ProductState::good) == ProductState::good);
        // should most likely return good product if speed is below 542
        CHECK(sim.quality_control.process(1, ProductState::good) == ProductState::good);
        CHECK(sim.quality_control.process(542, ProductState::good) == ProductState::good);
        CHECK(sim.quality_control.process(543, ProductState::good) == ProductState::bad);

        // if heaters are below 3000w every batch should fail
        sim.heater.force_power_levels(900, 900, 900);
        sim.step(5000);
        CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());

        // if heaters are at full power (6000w) most of following batch should be ok
        sim.heater.force_power_levels(2000, 2000, 2000);
        sim.step(6'000);
        CHECK(std::bitset<16>(sim.quality_control.get_output()).count() < 2);

        // if bolter, shaper or camera is offline every following batch should fail
        SUBCASE("OFFLINE DEVICES SHAPER"){
            sim.shaper.state = false;
            sim.step(6'000);
            CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());
        }
        SUBCASE("OFFLINE DEVICES BOLTER"){
            sim.bolter.state = false;
            sim.step(6'000);
            CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());
        }

        SUBCASE("OFFLINE DEVICES_QC"){
            sim.quality_control.set_state(false);
            sim.step(6'000);
            CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());
        }

        // TEST SPEED AND QC CORRELATION

        std::cout << sim.bolter.state << '\n'
                  << sim.shaper.state << '\n'
                  << sim.quality_control.get_state() << '\n'
                  << std::bitset<8>(sim.heater.get_state()) << '\n'
                  << sim.bolter.state << '\n'
                  << sim.conveyor.get_upm_current() << '\n'
                  << sim.heater.get_power() << '\n';


    }
}