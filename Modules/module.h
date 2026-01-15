// module.h
#ifndef MODULE_H
#define MODULE_H
#include <cstring>
#include <string>


template<typename... ARGS >
int println( const char* ch, ARGS... args)
{
    if (!std::strchr(ch, '\n')) {
        std::string s{ch};
        s += "\n";
        return printf(s.c_str(), args...);
    }
    return printf(ch, args...);
}


class Module
{
public:
    Module():_last_seen (0){}
    virtual ~Module() = default;
    virtual void init() = 0;
    virtual void loop();
    virtual void loop_ms(const int freq);

private:
    uint32_t _frequency;
protected:
    uint32_t _last_seen;
    //virtual void print_uart(std::string_view text);
};

#endif // MODULE_H