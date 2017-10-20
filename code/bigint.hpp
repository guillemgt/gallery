#ifndef bigint_hpp
#define bigint_hpp

#include "Engine/engine.hpp"

// Big unsigned integers
// digits is the number of u64's they hold

template <u64 digits> struct BigUint {
    u64 nums[digits];
    
    inline u64 operator[](u64 index) const {
        return nums[index];
    }
    
    BigUint operator+(u64 n);
    BigUint operator-(u64 n);
    BigUint operator++();
    BigUint operator--();
};

#endif /* bigint_hpp */
