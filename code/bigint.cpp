#include "bigint.hpp"


// @TODO: Optimize sum and difference

template <u64 digits> BigUint<digits> BigUint<digits>::operator+(u64 n){
    BigUint<digits> result;
    u64 index;
    for(index=0; index<digits; index++)
        result.nums[index] = this->nums[index];
    result[0] = this->nums[0]+n;
    
    // We check if there is overflow
    while(result.nums[index] < this->nums[index]){
        index++;
        if(index >= digits)
            break;
        result.nums[index]++;
    }
    
    return result;
}
template <u64 digits> BigUint<digits> BigUint<digits>::operator-(u64 n){
    BigUint<digits> result;
    u64 index;
    for(index=0; index<digits; index++)
        result.nums[index] = this->nums[index];
    result[0] = this->nums[0]-n;
    
    // We check if there is overflow
    while(result.nums[index] > this->nums[index]){
        index++;
        if(index >= digits)
            break;
        result.nums[index]--;
    }
    
    return result;
}

template <u64 digits> BigUint<digits> BigUint<digits>::operator++(){
    return *this + (u64)1;
}
template <u64 digits> BigUint<digits> BigUint<digits>::operator--(){
    return *this - (u64)1;
}
