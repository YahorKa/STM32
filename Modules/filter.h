/*filter.h*/
#ifndef FILTER_H
#define FILTER_H

template<typename T>
T FilterEMA(T prevVal, T val, float alpha = 0.3f){
    return alpha*val + (1 - alpha)*prevVal;
}   

#endif