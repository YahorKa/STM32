#ifndef TIMER_H
#define TIMER_H

#include <memory>
#include "module.h"
#include <cstdint>
#include <vector>

extern uint32_t HAL_GetTick();

class Timer : public Module
{
public:
   virtual void init() override {};
   virtual void loop() override {
      uint32_t now = HAL_GetTick();
      for (auto it = _tasks.begin(); it != _tasks.end();)
      {
         if ((*it)->execution_time <= now) {
            (*it)->execute();
            // delete *it;
            it = _tasks.erase(it);
         } else ++it;
      }
   };

   // Using base class and polymorphism to store collable object another types at vector<TaskBase*>
   struct TaskBase 
   {
      virtual ~TaskBase() = default;
      virtual void execute() = 0;
      uint32_t execution_time;
   };
   template<typename CallableFunc>
   struct Task : TaskBase
   {
      using TaskBase::execution_time;
      Task(uint32_t time, CallableFunc&& func) 
         : lambda(std::forward<CallableFunc>(func))
      {
         execution_time = HAL_GetTick() + time;
      }
      CallableFunc lambda;
      void execute() override {lambda();};
   };
   // AddTask for all callable objects
   template<typename CallableFunc>
   void AddTask(uint32_t delayMs, CallableFunc&& func){
      //Task<CallableFunc>* newTask = new Task<CallableFunc>{HAL_GetTick() + delayMs, std::forward<CallableFunc>(func)};
      std::unique_ptr<TaskBase> newTask = std::make_unique<Task<CallableFunc>>(delayMs, std::forward<CallableFunc>(func));
      _tasks.push_back(std::move(newTask));
   }

private:
   //std::vector<TaskBase*> _tasks;
   std::vector<std::unique_ptr<TaskBase>> _tasks;
};

constexpr uint32_t operator"" _ms(unsigned long long ms) { return ms; }
constexpr uint32_t operator"" _s(unsigned long long s) { return s * 1000; }
constexpr uint32_t operator"" _m(unsigned long long m) { return m * 60 * 1000; }
#endif

/* 
   Heater heater;
   Timer timer;
   timer.AddTask(300MS, [heater&](){heater.off()});


   timer.AddTask({300, [heater&](){heater.off()});
*/