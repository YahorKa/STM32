#ifndef SYS_MANAGER_H
#define SYS_MANAGER_H
#include <vector>
#include "module.h"
// SystemManager.h
class SystemManager {
public:
    ~SystemManager();
    void init();
    void loop();
    SystemManager& add(Module*);
private:
    std::vector<Module*> activeModules;
};


#endif