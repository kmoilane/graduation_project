#if !defined(QC)
#define QC

#include "units.hpp"
#include "random_between.hpp"
#include "product.hpp"
#include <algorithm>
#include <limits>


// Simulates 16 bit Camera memory of defected products (1 == bad)
struct CameraMemory
{
    const uint16_t init     {std::numeric_limits<uint16_t>::max()};
    uint16_t cur_batch      {init};
    uint16_t last_batch     {init};
    uint16_t index          {0};

    void update(ProductState product_state, bool camera_state){
        if (index == sizeof(decltype(last_batch)) * 8){
            last_batch = cur_batch;
            index = cur_batch = 0;
        }
        
        cur_batch |= static_cast<uint16_t>((product_state == ProductState::bad) || not camera_state) << index;
        index += 1;
    }
};


namespace devices
{
    
// Simulates Quality Control (camera) at the end of the production line
class QualityControl
{

private:

    bool state {false};
    CameraMemory cam_memory;

    const speed qc_speed_max = 542; 
    const double false_positive_prob {0.005};
    const double false_negative_prob {0.01};

public:

    QualityControl(uint8_t initial_state) : state(initial_state) {}
    ~QualityControl() {}

    void process(speed conveyor_speed, ProductState product_state){

        if (product_state != ProductState::not_present){
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

            //std::cout << static_cast<int>(product_state) << '\n';
            cam_memory.update(product_state, state);
        }

    }

    // returns qualified objects represented by a bitmask
    // previously computed value is returned until new one becomes available
    uint16_t get_output() const {
        return cam_memory.last_batch;
    }

    void set_state(uint8_t new_state){
        state = (new_state == 1);
    }
    
    void configure(Configuration &config){
        auto config_state = config.data["Simulation"]["Camera"]["state"];
        if (!config_state.is_null()){
            state = config_state;
        }
    }
};

} // namespace devices

#endif // QC
