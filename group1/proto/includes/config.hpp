#if !defined(CONFIGURATION)
#define CONFIGURATION

#include <nlohmann/json.hpp>
#include <fstream>
#include "units.hpp"


class Configuration
{
public:
    std::string filepath;
    json data;

    Configuration(std::string&& path) : filepath(path) {
         std::ifstream file(filepath);
        data = json::parse(file);
    }
    ~Configuration() {}
};

#endif // CONFIGURATION
