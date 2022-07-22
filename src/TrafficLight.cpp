#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <thread>
#include <chrono>
#include <mutex>
/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function.
     std::unique_lock<std::mutex> lock(mtx);     
    _condition.wait(lock,[this]{return !_queue.empty();});
    TrafficLightPhase rxMsg = std::move(_queue.front()); 
    return rxMsg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    std::lock_guard<std::mutex> lockg(mtx);
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

 
void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        TrafficLightPhase currentPhase = msg_queue.receive();
        if(currentPhase == green){return;}
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase phase)
{
    _currentPhase = phase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called.
    // To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    std::random_device seed;
    std::mt19937 gen(seed());
    std::uniform_int_distribution<> dist(4000, 6000);
    
    while(true)
    {
       
        if (_currentPhase == red)
        {
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(dist(gen))); 
            setCurrentPhase(green);
            std::cout<<"Green light"<<std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 

         if (_currentPhase == green)
        {
           
            
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(dist(gen))); 
            setCurrentPhase(red);
            std::cout<<"Red Light"<<std::endl;
        }
        msg_queue.send(std::move(getCurrentPhase()));
        std::cout<<"cycle through lights "<<std::endl;
    }
}

