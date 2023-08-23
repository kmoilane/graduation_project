#if !defined(UNITS_HPP)
#define UNITS_HPP

#include <cstdint>
#include <nlohmann/json.hpp>

using celsius = double;
using watts = double;
using seconds = int16_t;
using speed = int16_t;
using milliseconds = int16_t;
using time_stamp = std::chrono::system_clock::time_point;
using json = nlohmann::json;

#endif // UNITS_HPP
