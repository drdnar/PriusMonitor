#ifndef CAN_FRAME_TYPES_H
#define CAN_FRAME_TYPES_H

namespace Vehicle
{

/**
 * General class that records some kind of data about the vehicle.
 */
class Parameter
{
    public:
        /**
         * Returns the SI units of this parameter.
         */
        virtual const char* GetUnits() const noexcept = 0;
        /**
         * Returns the current raw integer value of this parameter.
         */
        signed int GetRawValue() const noexcept { return raw_value; };
        /**
         * Returns the scale factor of this parameter's integer value.
         */
        virtual float GetRawScale() const noexcept = 0;
        /**
         * Returns the value of this parameter, adjusted to match GetUnits().
         */
        virtual float GetValue() const noexcept = 0;
        /**
         * Updates this parameter.
         * @param new_value Raw, unadjusted value direct from CAN frame.
         */
        virtual void Update(signed int new_value) = 0;
    protected:
        signed int raw_value;
};


#define PARAMETERS(NAME, UNITS, SCALE)    private: \
        NAME() = default; \
        NAME(const NAME&) = delete; \
        NAME& operator=(const NAME&) = delete; \
    public: \
        static float Value() noexcept { return Instance.GetValue(); } \
        const char* GetUnits() const noexcept { return UNITS; } \
        static const char* Units() noexcept { return UNITS; } \
        float GetRawScale() const noexcept { return SCALE; } \
        static NAME Instance

#define SIMPLE_VALUE() public: float GetValue() const noexcept { return raw_value; }

#define DEFAULT_VALUE() public: float GetValue() const noexcept { return GetRawScale() * raw_value; }

#define DEFAULT_UPDATE() public: void Update(signed int new_value) { raw_value = new_value; }

#define PARAMETERS_DEFAULT(NAME, UNITS, SCALE) PARAMETERS(NAME, UNITS, SCALE); DEFAULT_VALUE(); DEFAULT_UPDATE();

#define PARAMETERS_SIMPLE(NAME, UNITS, SCALE) PARAMETERS(NAME, UNITS, SCALE); SIMPLE_VALUE(); DEFAULT_UPDATE();

/**
 * Brake pedal position
 */
class BrakePedal : public Parameter
{
        PARAMETERS_DEFAULT(BrakePedal, "%", 100.0f / 127);
};

class AcceleratorPosition : public Parameter
{
        PARAMETERS_SIMPLE(AcceleratorPosition, "?", 1.0f);
};

/**
 * Traction battery amperage, as periodically reported.
 */
class BatteryCurrent : public Parameter
{
        PARAMETERS(BatteryCurrent, "A", 0.1f);
        DEFAULT_VALUE();
    public:
        void Update(signed int new_value);
};

/**
 * Traction battery voltage, as periodically reported.
 */
class BatteryPotentional : public Parameter
{
        PARAMETERS_SIMPLE(BatteryPotentional, "V", 1.0f);
};


/**
 * Battery charge state
 */
class BatterySoC : public Parameter
{
        PARAMETERS_DEFAULT(BatterySoC, "%", 0.5f);
};


/**
 * Battery delta SoC.
 * This is the computed difference in charge between the most-charged and
 * least-charged battery modules.
 */
class BatteryDeltaSoC : public Parameter
{
        PARAMETERS_DEFAULT(BatteryDeltaSoC, "%", 0.5f);
};


/**
 * Battery lowest temp
 */
class BatteryTempLowest : public Parameter
{
        PARAMETERS_SIMPLE(BatteryTempLowest, "C", 1.0f);
};


/**
 * Battery highest temp
 */
class BatteryTempHighest : public Parameter
{
        PARAMETERS_SIMPLE(BatteryTempHighest, "C", 1.0f);
};


/**
 * Front left wheel speed (probably for ABS?)
 */
class WheelFrontLeft : public Parameter
{
        PARAMETERS_DEFAULT(WheelFrontLeft, "km/hr", 0.01f);
};

/**
 * Front right wheel speed (probably for ABS?)
 */
class WheelFrontRight : public Parameter
{
        PARAMETERS_DEFAULT(WheelFrontRight, "km/hr", 0.01f);
};

/**
 * Rear left wheel speed (probably for ABS?)
 */
class WheelRearLeft : public Parameter
{
        PARAMETERS_DEFAULT(WheelRearLeft, "km/hr", 0.01f);
};

/**
 * Rear right wheel speed (probably for ABS?)
 */
class WheelRearRight : public Parameter
{
        PARAMETERS_DEFAULT(WheelRearRight, "km/hr", 0.01f);
};


/**
 * Overall vehicle speed.
 */
class Speed : public Parameter
{
        PARAMETERS_DEFAULT(Speed, "km/hr", 0.01f);
};

class EngineCoolantTemp : public Parameter
{
        PARAMETERS_DEFAULT(EngineCoolantTemp, "C", 0.5f);
/*        DEFAULT_VALUE();
    public:
        void Update(signed int new_value);*/
};


#undef PARAMETERS
#undef SIMPLE_VALUE
#undef DEFAULT_VALUE
#undef PARAMETERS_DEFAULT
#undef PARAMETERS_SIMPLE

} /* namespace Vehicle */
#endif /* CAN_FRAME_TYPES_H */
