// module.h
#ifndef MODULE_H
#define MODULE_H
#include <string_view>
class Module
{
public:
    virtual ~Module() = default;
    virtual void init() = 0;
    virtual void loop() = 0;
    virtual void print_uart(std::string_view text);
};

#endif // MODULE_H