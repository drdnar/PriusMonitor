#ifndef CAN_PROCESS_LOOP_H
#define CAN_PROCESS_LOOP_H

#include "CANBus.hpp"

namespace Vehicle
{

/**
 * Process traffic on a CAN bus.
 */
class CANTrafficProcessor
{
    public:
        void Begin();
        CANTrafficProcessor(CANBus& bus);
        ~CANTrafficProcessor();
    private:
        CANBus& Bus;
        static void* CANTrafficProcessorLoop(void* param);
        /**
         * This hack prevents this header from needing the whole pthread.h header.
         */
        void* Thread_ID = nullptr;
};

} /* namespace Vehicle */
#endif /* CAN_PROCESS_LOOP_H */
