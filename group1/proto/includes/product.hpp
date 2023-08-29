#if !defined(PRODUCT)
#define PRODUCT


// status of objects in production line stages
enum class ProductState{
    good,
    bad,
    not_present,
};



#endif // PRODUCT
