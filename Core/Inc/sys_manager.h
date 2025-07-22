#ifndef SYS_MANAGER_H
#define SYS_MANAGER_H
#include <vector>
#include "module.h"
// SystemManager.h
class SystemManager {
public:
    void init();
    void loop();
private:
    std::vector<Module*> activeModules;
};


#endif