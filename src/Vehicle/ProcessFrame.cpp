#include "ProcessFrame.hpp"
#include "Parameter.hpp"

#include <map>
#include <functional>
#include <stdio.h>

using namespace Vehicle;
using namespace std;

map<int, function<void(CANFrame&)>> FrameHandlers { };

/**
 * Boilerplate reduction
 */
#define FRAME_HANDLER(ID) void Handle ## ID(CANFrame& frame)


FRAME_HANDLER(030)
{
    // 1 of 8 bytes known
    BrakePedal::Instance.Update(frame.Data[4]);
    //printf("Brake pressure: %.1f\n", BrakePedal::Instance.Value());
}


FRAME_HANDLER(03A)
{
    //printf("Accelerator position: %d\n", frame.Data[5]);
}


FRAME_HANDLER(03B)
{
    // 4 of 4 (5) bytes known, last byte checksum
    BatteryCurrent::Instance.Update((frame.Data[0] << 8) | frame.Data[1]);
    BatteryPotentional::Instance.Update((frame.Data[2] << 8) | frame.Data[3]);
    /*printf("Battery power: %+6.1f %s @ %.0f %s -> %+8.1f W\n", 
        BatteryCurrent::Instance.Value(),
        BatteryCurrent::Instance.GetUnits(),
        BatteryPotentional::Instance.Value(), 
        BatteryPotentional::Instance.GetUnits(),
        BatteryCurrent::Instance.Value() * BatteryPotentional::Instance.Value());
        */
}


FRAME_HANDLER(0B1)
{
    // 4 of 5 (6) bytes known, last byte checksum
    WheelFrontRight::Instance.Update((frame.Data[0] << 8) | frame.Data[1]);
    WheelFrontLeft::Instance.Update((frame.Data[2] << 8) | frame.Data[3]);
    //printf("Front wheels: L: %6.2f kph  R: %6.2f kph\n", left, right);
}


FRAME_HANDLER(0B3)
{
    // 4 of 5 (6) bytes known, last byte checksum
    WheelRearRight::Instance.Update((frame.Data[0] << 8) | frame.Data[1]);
    WheelRearLeft::Instance.Update((frame.Data[2] << 8) | frame.Data[3]);
    //printf("Rear wheels: L: %6.2f kph  R: %6.2f kph\n", left, right);
}


FRAME_HANDLER(0B4)
{
    // 2 of 7 (8) bytes known
    Speed::Instance.Update((frame.Data[5] << 8) | frame.Data[6]);
    //printf("Speed: %6.2f kph\n", speed);
}


FRAME_HANDLER(3CB)
{
    // 6 of 6 (7) bytes known
    BatterySoC::Instance.Update(frame.Data[3]);
    BatteryDeltaSoC::Instance.Update(frame.Data[2]);
    BatteryTempLowest::Instance.Update(frame.Data[4]);
    BatteryTempHighest::Instance.Update(frame.Data[5]);
    //printf("Battery health: max out: %3d A  max in: %3d A  delta SOC: %4.1f %%  SOC: %4.1f %%  t1: %-3d C  t2: %-3d C\n",
    //    frame.Data[0], frame.Data[1], delta_soc, soc, t1, t2);
}


FRAME_HANDLER(520)
{
    float injector = frame.Data[2];
}


FRAME_HANDLER(52C)
{
    // 1 of 2 bytes known, not sure what byte 0 does
    EngineCoolantTemp::Instance.Update(frame.Data[1]);
}



#define ADD_FRAME_HANDLER(ID) FrameHandlers[0x ## ID] = Handle ## ID
/**
 * Triggers initialization of frame handlers at startup.
 */
static struct ProcessFrameInitHook
{
    ProcessFrameInitHook()
    {
        ADD_FRAME_HANDLER(030);
        ADD_FRAME_HANDLER(03A);
        ADD_FRAME_HANDLER(03B);
        ADD_FRAME_HANDLER(0B1);
        ADD_FRAME_HANDLER(0B3);
        ADD_FRAME_HANDLER(0B4);
        ADD_FRAME_HANDLER(3CB);
        ADD_FRAME_HANDLER(52C);
    }
} instance;


void Vehicle::ProcessFrame(CANFrame& frame)
{
    if (FrameHandlers.count(frame.ID))
        FrameHandlers[frame.ID](frame);
}
