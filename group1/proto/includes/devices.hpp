#if !defined(DEVICES)
#define DEVICES

#include "units.hpp"
#include "random_between.hpp"

class Bolter
{
private:
    const double bolter_fail_prob {0.02};
    
public:
    bool state {false};
    Bolter() {}
    ~Bolter() {}

    // applies fixed probability to produce a defective product
    int process(int product_state){
        if (product_state == 1){
            return rand_between::rand_between(0.0, 1.0) > bolter_fail_prob;
        }
        return product_state;
    }
};

class Shaper
{
public:
    bool state {false};
    Shaper() {}
    ~Shaper() {}

    int process(int product_state){
        return product_state;
    }
};


#endif // DEVICES
