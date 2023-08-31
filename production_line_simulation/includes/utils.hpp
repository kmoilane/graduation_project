#if !defined(UTILS)
#define UTILS

#include "units.hpp"
#include <nlohmann/json.hpp>
#include <tuple>
#include <iostream>


template<typename JSON, typename T>
T get_from_json(JSON& data, T default_value){
    if (data.is_null()){
        return default_value;
    }
    return data;
}


#endif // UTILS
