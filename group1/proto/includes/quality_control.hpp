#if !defined(QC)
#define QC

#include "units.hpp"
#include "random_between.hpp"
#include <deque>
#include <array>
#include <algorithm>
#include <bitset>


// simulates a step (one object shift from a process to another) in production line
class QualityControl
{

private:

    bool state {false};
    const speed qc_speed_max = 542; 
    const double false_positive_prob {0.005};
    const double false_negative_prob {0.01};
    std::deque<bool> qc_storage;
    uint16_t result {0};

    // compute uint16 representation of the current batch
    uint16_t get_int_repr(){
        std::bitset<16> bits;
        for (size_t i = 0; i < qc_storage.size(); i++){
            bits[i] = qc_storage[i];
        }
        return uint16_t(bits.to_ulong());
    }

public:

    QualityControl() {}
    ~QualityControl() {}


    void apply_camera(speed convoyer_speed, int item){

        // apply camera if there is an item in this stage [0 || 1]
        if (item != -1){

            // full storage must be cleared before next batch
            if (qc_storage.size() == 16){
                result = get_int_repr();
                qc_storage.clear();
            }

            // apply quality control error to product
            bool product_state = (item == 1);
            if (product_state == 1)
            {
                product_state -= (rand_between::rand_between(0.0, 1.0) < false_negative_prob);
            }
            else{
                product_state += (rand_between::rand_between(0.0, 1.0) < false_positive_prob);
            }

            // auto fail if speed too fast for the camera or camera state off
            qc_storage.push_front(!((convoyer_speed < qc_speed_max) && product_state && state));
            int a = 0;
        }
    }

    // returns qualified objects represented by a bitmask
    // previously computed value is returned until new one becomes available
    uint16_t get_output() const {
        return result;
    }

    void set_state(uint8_t new_state){
        state = (new_state == 1);
    }
        
};

#endif // QC
