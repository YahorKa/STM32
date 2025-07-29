/*filter.h*/
#ifndef FILTER_H
#define FILTER_H

template<typename T>
T filterEMA(T prevVal, T val, float alpha = 0.3f){
    return alpha*val + (1 - alpha)*prevVal;
}   

class Kalman {
public:
    float angle = 0.0f;   // оценка угла
    float bias = 0.0f;    // смещение гироскопа
    float P[2][2] = {{1, 0}, {0, 1}}; // ковариационная матрица

    float Q_angle = 0.001f; // шум оценки
    float Q_bias = 0.003f;  // шум смещения
    float R_measure = 0.03f; // шум акселерометра

    float update(float newAngle, float newRate, float dt);
};


#endif