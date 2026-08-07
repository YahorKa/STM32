#pragma once
#include <array>

template<typename T, int MAX_SIZE>
class RingBuffer {
private:
    std::array<T, MAX_SIZE> data{};
    int head = 0;
    int count = 0;

public:
    void push(const T& value) {
        data[head] = value;
        head = (head + 1) % MAX_SIZE;
        if (count < MAX_SIZE) count++;
    }

    T get(int index) const {
        if (index >= count) return T{};
        int physical = (head - count + index + MAX_SIZE) % MAX_SIZE;
        return data[physical];
    }

    int size() const { return count; }
    bool empty() const { return count == 0; }
    bool full() const { return count == MAX_SIZE; }

    // Очистка (опционально)
    void clear() {
        head = 0;
        count = 0;
        data.fill(T{});
    }
};