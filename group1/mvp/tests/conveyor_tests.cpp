#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../includes/conveyor.hpp"
#include "../includes/units.hpp"

TEST_CASE("CONVEYOR TESTS")
{
    devices::Conveyor conveyor{};
    SUBCASE("TESTING SET AND GET CURRENT SPEED")
    {
        conveyor.set_speed_current(255);
        CHECK(conveyor.get_speed_current() == 255);
        conveyor.set_speed_current(125);
        CHECK(conveyor.get_speed_current() == 125);
        conveyor.set_speed_current(0);
        CHECK(conveyor.get_speed_current() == 0);
        conveyor.set_speed_current(1);
        CHECK(conveyor.get_speed_current() == 1);
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
        conveyor.set_speed_current(100);
        conveyor.set_speed_target(101);
        conveyor.update();
        
    }
}
