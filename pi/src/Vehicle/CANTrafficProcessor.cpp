#include "CANTrafficProcessor.hpp"
#include "ProcessFrame.hpp"
#include "Parameter.hpp"
#include <pthread.h>
#include <stdlib.h>
#include <stdexcept>

#define thread_id ((pthread_t*)Thread_ID)

using namespace Vehicle;


void* CANTrafficProcessor::CANTrafficProcessorLoop(void* param)
{
    CANBus& bus = ((CANTrafficProcessor*)param)->Bus;
    CANFrame frame;
    do
    {
        if (bus.Receive(frame))
        {
            ProcessFrame(frame);
        }
    } while (true);
}


void CANTrafficProcessor::Begin()
{
    Thread_ID = malloc(sizeof(pthread_t));
    if (pthread_create(thread_id, nullptr, &CANTrafficProcessorLoop, this))
    {
        free(Thread_ID);
        throw std::runtime_error("CANTrafficProcessor::Begin(): failed to create thread");
    }
}


CANTrafficProcessor::CANTrafficProcessor(CANBus& bus) : Bus(bus)
{
    // do nothing else special
}


CANTrafficProcessor::~CANTrafficProcessor()
{
    free(Thread_ID);
}
