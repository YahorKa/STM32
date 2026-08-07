
#ifndef DTH11_H
#define DTH11_H
#include "display_ssd1306.h"
#include "module.h"
#include "gpio.h"
#include <cstdint>
#include <array>
#include "ring_buffer.h"

enum class DHT11_Error : uint8_t {
    OK = 0,
    ERR_START_SIGNAL,
    ERR_NO_RESPONSE_LOW,
    ERR_NO_RESPONSE_HIGH,
    ERR_BIT_TIMEOUT,
    ERR_CHECKSUM,
    ERR_READ_BIT,
    ERR_UNKNOWN,
    ERR_NOVCC
};
class DTH11 : public Module
{
public:

    DHT11_Error getLastError() const { return _lastError; }
    const char* getErrorString() const; 
    explicit DTH11(GPIO_TypeDef* port, uint16_t data_pin, GPIO_TypeDef* vcc_port = nullptr, uint16_t vcc_pin = 0);
    DTH11(const DTH11&) = delete;
    DTH11& operator = (const DTH11&) = delete;
    virtual void init() override;
    virtual void loop() override;
    inline void setHigh() {HAL_GPIO_WritePin(_data_port, _data_pin, GPIO_PIN_SET);}
    inline void setLow()  {HAL_GPIO_WritePin(_data_port, _data_pin, GPIO_PIN_RESET);}
    inline bool readPin() {return HAL_GPIO_ReadPin(_data_port, _data_pin) == GPIO_PIN_SET;}
    inline int  readBit();
    inline float get_temperature() const {return _data[2] +  _data[3] * 0.1f;}
    inline float get_humidity() const {return _data[0]+_data[1] * 0.1f;}
    uint8_t getTempDec() const { return _data[3]; }
    uint8_t getTempH() const { return _data[1]; }
    uint8_t getReset() const { return _hard_reset_counter; }
    const RingBuffer<uint8_t, 128>& get_history() {return _history;}
    bool read();

private:
    std::array<char, 5> _data = {};
    //ifdef DTH11 (else RingBuffer<float>)
    RingBuffer<uint8_t, 128> _history;
    float _temperature;
    float _humidity;
    uint32_t _hard_reset_counter;
    GPIO_TypeDef* _data_port;
    uint16_t _data_pin;
    GPIO_TypeDef* _vcc_port ;
    uint16_t _vcc_pin;
    DHT11_Error _lastError = DHT11_Error::OK;
    bool hardReset();
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