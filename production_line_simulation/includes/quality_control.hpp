#if !defined(QC)
#define QC

#include "units.hpp"
#include "random_between.hpp"
#include "product.hpp"
#include <algorithm>
#include <limits>
#include "utils.hpp"
#include "config.hpp"
#include <bitset>


// Simulates 16 bit Camera memory of defected products (1 == bad)
struct CameraMemory
{
    const uint16_t init     {std::numeric_limits<uint16_t>::max()};
    uint16_t cur_batch      {0};
    uint16_t last_batch     {init};
    uint16_t index          {0};
    uint16_t max_index      {sizeof(decltype(index)) * 8};


    void update(ProductState product_state){
        cur_batch |= static_cast<uint16_t>(product_state == ProductState::bad) << index;
        index += 1;

        if (index == max_index){
            last_batch = cur_batch;
            index = cur_batch = 0;
        }
    }
};


namespace devices
{
    
// Simulates Quality Control (camera) at the end of the production line
class QualityControl
{

public:

    QualityControl(bool initial_state);

    // applies simulated camera to a product
    ProductState process(speed conveyor_speed, ProductState product_state);

    // returns qualified objects in memory, represented as a bitmask
    uint16_t get_output() const;

    // sets camera on/off
    void set_state(uint8_t new_state);

    // gets camera on/off
    bool get_state() const;

    // apply configuration
    void configure(Configuration &config);

private:

    bool state {false};
    CameraMemory cam_memory;

    const speed qc_speed_max = 542; 
    const double false_positive_prob {0.005};
    const double false_negative_prob {0.01};
};


QualityControl::QualityControl(bool initial_state)
              : state(initial_state) {}


ProductState QualityControl::process(speed conveyor_speed, ProductState product_state){

    auto rvalue = rand_between::rand_between(0.0, 1.0);

    if (conveyor_speed > qc_speed_max){
        product_state = ProductState::bad;
    }

    else if (product_state == ProductState::bad && rvalue < false_positive_prob){
        product_state == ProductState::good;
    }
    else if (product_state == ProductState::good && rvalue < false_negative_prob){
        product_state == ProductState::bad;
    }
    if (state == false){
        product_state = ProductState::bad;
    }
    

    cam_memory.update(product_state); 

    return product_state;
}

uint16_t QualityControl::get_output() const {
    return cam_memory.last_batch;
}

void QualityControl::set_state(uint8_t new_state){
    state = (new_state == 1);
}

bool QualityControl::get_state() const {
    return state;
}

void QualityControl::configure(Configuration &config){
    auto config_data = config.data["Simulation"]["Camera"]["state"];
    state = get_from_json(config_data, state);
}

} // namespace devices

#endif // QC
