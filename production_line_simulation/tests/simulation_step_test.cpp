#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/simulation.hpp"
#include <limits>
#include <bitset>
#include "product.hpp"



TEST_CASE("SIMULATION STEP TESTS"){
    Configuration config;
    config.data["Simulation"]["Conveyor"]["speed_current"] = 200;
    config.data["Simulation"]["Camera"]["state"] = true;
    config.data["Simulation"]["Bolter"]["state"] = true;
    config.data["Simulation"]["Shaper"]["state"] = true;

    SUBCASE("START"){
        // simulation parameters made optimal for production
        Simulation sim(config);
        sim.heater.set_state(0b00000111);
        sim.heater.force_power_levels(2000, 2000, 2000);

        // at start quality control should report all as defects
        CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());
    }
    
    // should most likely return good product if speed is below 542 (camera has a chance of false recoqnition)
    SUBCASE("QC SPEED"){
        std::array<int, 3> results {0,0,0};
        Simulation sim(config);
        int loops = 100;
        for (size_t i = 543-50; i < 543+50; i++){
            results[0] += sim.quality_control.process(i, ProductState::good) == ProductState::good;
            results[1] += sim.quality_control.process(i, ProductState::good) == ProductState::good;
            results[2] += sim.quality_control.process(i, ProductState::good) == ProductState::good;
        }
        
        CHECK(results[0] == 50);
        CHECK(results[1] == 50);
        CHECK(results[2] == 50);
    }

    // offline devices should make only defected products
    SUBCASE("SEPARATE PROCESS STEPS"){
        // optimal parameters
        Simulation sim(config);
        sim.heater.force_power_levels(2000, 2000, 2000);
        sim.update(100);
    }

    SUBCASE("BOLTER_OFFLINE")
    {
        Simulation sim(config);
        CHECK(sim.bolter.get_state() == true);
        CHECK(sim.bolter.process(ProductState::good) == ProductState::good);
        sim.bolter.set_state(false);
        CHECK(sim.bolter.get_state() == false);
        CHECK(sim.bolter.process(ProductState::good) == ProductState::bad);
        sim.bolter.set_state(true);
        CHECK(sim.bolter.get_state() == true);
        CHECK(sim.bolter.process(ProductState::good) == ProductState::good);
    }

    SUBCASE("SHAPER_OFFLINE")
    {
        Simulation sim(config);
        CHECK(sim.shaper.get_state() == true);
        CHECK(sim.shaper.process(ProductState::good) == ProductState::good);
        sim.shaper.set_state(false);
        CHECK(sim.shaper.get_state() == false);
        CHECK(sim.shaper.process(ProductState::good) == ProductState::bad);
        sim.shaper.set_state(true);
        CHECK(sim.shaper.get_state() == true);
        CHECK(sim.shaper.process(ProductState::good) == ProductState::good);
    }

    SUBCASE("QC_OFFLINE")
    {
        Simulation sim(config);
        CHECK(sim.quality_control.get_state() == true);
        CHECK(sim.quality_control.process(400, ProductState::good) == ProductState::good);
        sim.quality_control.set_state(false);
        CHECK(sim.quality_control.get_state() == false);
        CHECK(sim.quality_control.process(400, ProductState::good) == ProductState::bad);
        sim.quality_control.set_state(true);
        CHECK(sim.quality_control.get_state() == true);
        CHECK(sim.quality_control.process(400, ProductState::good) == ProductState::good);
    }
    
    // if heaters are below 3000w every batch should fail
    SUBCASE("QC_HEATER-LOW-CORRELATION"){
        config.data["Simulation"]["Conveyor"]["speed_current"] = 200;
        Simulation sim(config);
        sim.heater.force_power_levels(900, 900, 900);

        sim.update(100);
        int loops {1000};
        int total {0};
        for (size_t i = 0; i < loops; i++){
            sim.step(2100);
            total += std::bitset<16>(sim.quality_control.get_output()).count();
        }

        CHECK((total / (loops * 16.0)) > 0.99);
    }

    SUBCASE("QC_HEATER-HIGH-CORRELATION"){

        // if heaters are at full power (6000w) most of products should be ok
        config.data["Simulation"]["Conveyor"]["speed_current"] = 200;
        Simulation sim(config);
        sim.heater.force_power_levels(2000, 2000, 2000);

        sim.update(1000);
        int loops {1000};
        int total {0};
        for (size_t i = 0; i < loops; i++){
            sim.step(2100);
            total += std::bitset<16>(sim.quality_control.get_output()).count();
        }

        CHECK((total / (loops * 16.0)) < 0.05);
    }

    // from known start time to time t with const acceleration, there should/shouldn't be new data in qc
    SUBCASE("SPEED_QC_CORRELATION"){
        config.data["Simulation"]["Conveyor"]["speed_current"] = 200;
        
        Simulation sim(config);
        sim.heater.force_power_levels(2000, 2000, 2000);

        // 600 * (200 / 255) = 470..upm = 7.843 ups
        // time = (16/ups) = 2.04 s
        sim.step(2000); // there should not be data before 2.04s
        CHECK(sim.quality_control.get_output() == std::numeric_limits<uint16_t>::max());
        sim.step(100); // quality control ouput should have changed by now
        CHECK(sim.quality_control.get_output() != std::numeric_limits<uint16_t>::max());
    }
}