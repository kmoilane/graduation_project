#if !defined(DEVICES)
#define DEVICES


#include "random_between.hpp"
#include "product.hpp"
#include "heater.hpp"
#include "quality_control.hpp"
#include "cooler.hpp"
#include "utils.hpp"


namespace devices
{
    
class Bolter
{
private:
    const double bolter_fail_prob {0.02};
    bool state {false};
    
public:
    Bolter(bool state) : state(state) {}

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
        state = get_from_json(config_state, false);
    }

    void set_state(bool state){
        this->state = state;
    }

    bool get_state() const{
        return state;
    }
};

class Shaper
{
private:
    bool state {false};
public:
    Shaper(bool initial_state) : state(state) {}

    // produces a defective product if not on
    ProductState process(ProductState product_state){
        if ((product_state == ProductState::good) && (state == false)){
            return ProductState::bad;
        }
        
        return product_state;
    }

    void configure(Configuration &config){
        auto config_state = config.data["Simulation"]["Shaper"]["state"];
        state = get_from_json(config_state, false);
    }

    void set_state(bool state){
        this->state = state;
    }

    bool get_state() const{
        return state;
    }
};

} // namespace devices


#endif // DEVICES
