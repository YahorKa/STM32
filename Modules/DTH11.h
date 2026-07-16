
#ifndef DTH11_H
#define DTH11_H
#include "module.h"
#include "gpio.h"
#include <cstdint>
#include <array>
class DTH11 : public Module
{
public:
    explicit DTH11(GPIO_TypeDef* port, uint16_t pin);
    DTH11(const DTH11&) = delete;
    DTH11& operator = (const DTH11&) = delete;
    virtual void init() override;
     virtual void loop() override;
    inline void setHigh() {HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);}
    inline void setLow()  {HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);}
    inline bool readPin() {return HAL_GPIO_ReadPin(_port, _pin) == GPIO_PIN_SET;}
    inline int readBit();
    inline float get_temperature() const {return _data[2] + _data[3]  / 256.0f;}
    inline float get_humidity() const {return _data[0]+_data[1] / 256.0f;}

    bool read();

private:
    std::array<char, 5> _data;
    float _temperature;
    float _humidity;
    GPIO_TypeDef* _port ;
    uint16_t _pin;
    void setOutputMode();
    void setInputMode();
    void startSignal();
    void delayUs(uint32_t us);
    uint32_t getMicros();
};

#endif DTH11_H

/*
Communication with Microcontroller
DHT11 uses only one wire for communication. The voltage levels with certain time value defines the logic one or logic zero on this pin.
The communication process is divided in three steps, first is to send request to DHT11 sensor then sensor will send response pulse and then it starts sending data of total 40 bits to the microcontroller.
This Picture Shows Communication of DHT11 With Microcontroller
Communication process
 

Start pulse (Request)

DHT11 Start Pulse
To start communication with DHT11, first we should send the start pulse to the DHT11 sensor.
To provide start pulse, pull down (low) the data pin minimum 18ms and then pull up, as shown in diag.
 

Response

DHT11 Response
After getting start pulse from, DHT11 sensor sends the response pulse which indicates that DHT11 received start pulse.
The response pulse is low for 54us and then goes high for 80us.
 

Data

DHT11 Output bit representation
 

After sending the response pulse, DHT11 sensor sends the data, which contains humidity and temperature value along with checksum.
The data frame is of total 40 bits long, it contains 5 segments (byte) and each segment is 8-bit long.
In these 5 segments, first two segments contain humidity value in decimal integer form. This value gives us Relative Percentage Humidity. 1st 8-bits are integer part and next 8 bits are fractional part.
Next two segments contain temperature value in decimal integer form. This value gives us temperature in Celsius form.
Last segment is the checksum which holds checksum of first four segments.
Here checksum byte is direct addition of humidity and temperature value. And we can verify it, whether it is same as checksum value or not. If it is not equal, then there is some error in the received data.
Once data received, DHT11 pin goes in low power consumption mode till next start pulse.
 

DHT11 End Of Frame
After sending 40-bit data, DHT11 sensor sends 54us low level and then goes high. After this DHT11 goes in sleep mode.
*/