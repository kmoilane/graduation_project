#if !defined(DEVICES)
#define DEVICES


#include "random_between.hpp"
#include "product.hpp"
#include "heater.hpp"
#include "quality_control.hpp"
#include "cooler.hpp"


namespace devices
{
    
class Bolter
{
private:
    const double bolter_fail_prob {0.02};
    
public:
    bool state {false};
    Bolter() {}
    ~Bolter() {}

    // applies fixed probability to produce a defective product
    // produces a defective product if not on
    ProductState process(ProductState product_state){
        if (product_state == ProductState::good && ((state == false) ||
            rand_between::rand_between(0.0, 1.0) < bolter_fail_prob)){
                return ProductState::bad;
        }
        return product_state;
    }

    void configure(Configuration &config){
        auto config_state = config.data["Simulation"]["Bolter"]["state"];
        if (!config_state.is_null())
        {
            state = config_state;
        }
    }
};

class Shaper
{
public:
    bool state {false};
    Shaper() {}
    ~Shaper() {}

    // produces a defective product if not on
    ProductState process(ProductState product_state){
        if ((product_state == ProductState::good) && (state == false)){
            return ProductState::bad;
        }
        
        return product_state;
    }

    void configure(Configuration &config){
        auto config_state = config.data["Simulation"]["Shaper"]["state"];
        if (!config_state.is_null())
        {
            state = config_state;
        }
    }
};

} // namespace devices


#endif // DEVICES
