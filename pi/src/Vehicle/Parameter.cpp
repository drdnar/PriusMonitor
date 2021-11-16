#include "Parameter.hpp"

#include <stdint.h>

using namespace Vehicle;

const char Parameter::format_strings[2][16] = { "%*.*f", "% *.*f" };

// From http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
template <typename T, unsigned B>
inline T sign_extend(const T x)
{
    struct {T x:B;} s;
    return s.x = x;
}

#define INSTANCE(NAME) NAME NAME::Instance { }

#define UPDATE(NAME) void NAME::update(signed int new_value)

INSTANCE(BrakePedal);

INSTANCE(AcceleratorPosition);

INSTANCE(BatteryCurrent);
UPDATE(BatteryCurrent)
{
    Parameter::Update(sign_extend<signed int, 12>(new_value));
}

INSTANCE(BatteryPotentional);
INSTANCE(BatterySoC);
INSTANCE(BatteryDeltaSoC);
INSTANCE(BatteryTempLowest);
INSTANCE(BatteryTempHighest);

INSTANCE(WheelFrontLeft);
INSTANCE(WheelFrontRight);
INSTANCE(WheelRearLeft);
INSTANCE(WheelRearRight);

INSTANCE(Speed);

INSTANCE(EngineCoolantTemp);
/*UPDATE(EngineCoolantTemp)
{
    raw_value = sign_extend<signed int, 8>(new_value);
}*/