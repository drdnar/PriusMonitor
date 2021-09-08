#ifndef PROCESS_FRAME_H
#define PROCESS_FRAME_H

#include "CAN.hpp"

namespace Vehicle
{

/**
 * Processes a data frame, updating all state.
 */
void ProcessFrame(CANFrame& frame);

} /* namespace Vehicle */
#endif /* PROCESS_FRAME_H */
