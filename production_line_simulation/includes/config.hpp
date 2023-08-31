#if !defined(CONFIGURATION)
#define CONFIGURATION

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>
#include "units.hpp"


class Configuration
{
public:
    std::string filepath;
    json data;

    Configuration() = default;
    Configuration(std::string&& path) : filepath(path) {
        std::ifstream file(filepath);
        try{
            data = json::parse(file);
        } catch (const std::exception e) {
            std::cout << "Error at config: " << e.what() << '\n';
        }
    }
    ~Configuration() {}

};

#endif // CONFIGURATION
