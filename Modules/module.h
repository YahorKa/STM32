// module.h
#ifndef MODULE_H
#define MODULE_H
#include <cstring>
#include <string>


template<typename... ARGS >
int println( const char* format, ARGS... args)
{
    if (!std::strchr(format, '\n')) {
        std::string s{format};
        s += "\n";
        return printf(s.c_str(), args...);
    }
    return printf(format, args...);
}


class Module
{
public:
    virtual ~Module() = default;
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual void loop(const uint32_t&) final;
    virtual uint32_t frequency() const = 0;

private:
    uint32_t _frequency;
protected:
    uint32_t _last_seen;
    //virtual void print_uart(std::string_view text);
};

#endif // MODULE_H