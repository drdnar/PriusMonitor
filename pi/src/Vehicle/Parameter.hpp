#ifndef CAN_FRAME_TYPES_H
#define CAN_FRAME_TYPES_H

#include <cstddef>
#include <cstdio>
#include <stdint.h>

namespace Vehicle
{

/**
 * General class that records some kind of data about the vehicle.
 * 
 * @internal Floats are used because the real-world sensors aren't accurate
 * enough to justify doubles.
 */
class Parameter
{
    public:
        const char* GetLongName() const noexcept { return long_name; }
        const char* GetShortName() const noexcept { return short_name; }
        const char* GetAbbreviation() const noexcept { return abbreviation; }
        /**
         * Returns the SI units of this parameter.
         */
        const char* GetUnits() const noexcept { return units; }
        /**
         * Returns the SI units of this parameter.
         */
        const char* GetUnitsImperial() const noexcept { return imperial_units; }
        /**
         * Returns the current raw integer value of this parameter.
         */
        signed int GetRawValue() const noexcept { return raw_value; }
        /**
         * Returns the scale factor of this parameter's integer value.
         */
        virtual float GetRawScale() const noexcept { return raw_scale; }
        /**
         * Returns the value of this parameter, adjusted to match GetUnits().
         */
        virtual float GetValue() const noexcept { return metric_value; };
        /**
         * Preferred number of digits to show before decimal point.
         */
        uint_fast8_t GetIntegerDigits() const noexcept { return integer_digits; }
        /**
         * Preferred number of digits to show after decimal point.
         */
        uint_fast8_t GetFractionalDigits() const noexcept { return fractional_digits; }
        /**
         * Can this parameter have negative values?
         */
        bool Signed() const noexcept { return is_signed; }
        /**
         * Updates this parameter.
         * @param new_value Raw, unadjusted value direct from CAN frame.
         */
        void Update(signed int new_value) { dirty_string = true; update(new_value); };
        /**
         * Maximum length of a string value.
         */
        static const std::size_t MaxStringLength = 31;
        /**
         * Returns a human-readable string representation.
         */
        const char* GetString() noexcept { if (dirty_string) update_string_do(); return cached_string; }
        /**
         * Returns a human-readable string representation.
         */
        const char* GetStringImperial() noexcept { if (cached_string_imperial) update_string_imperial_do(); return cached_string_imperial; }
    protected:
        /**
         * Used to convert a raw value to an SI value.
         */
        float raw_scale = 1.0f;
        /**
         * Used to convert raw degrees Celsius to kelvins.
         */
        float raw_offset = 0.0f;
        /**
         * Used to convert kelvins to degrees Celsius.  Should otherwise always be zero.
         */
        float metric_offset = 0.0f;
        /**
         * Used to convert an SI value to imperial.
         */
        float imperial_scale = 1.0f;
        /**
         * Used to convert kelvins to degrees Fahrenheit.  Should otherwise always be zero.
         */
        float imperial_offset = 0.0f;
        const char* long_name = "long name";
        const char* short_name = "short name";
        const char* abbreviation = "abbreviation";
        /**
         * Name of standard SI units, e.g. km.
         */
        const char* units = "units";
        /**
         * Name of imperial (US) units, e.g. mi.
         */
        const char* imperial_units = "units";
        /**
         * True if the formatted displayed value can be negative.
         */
        bool is_signed = true;
        /**
         * Number of digits to show before decimal point.
         */
        uint_fast8_t integer_digits = 2;
        /**
         * Number of digits to show after decimal point.
         */
        uint_fast8_t fractional_digits = 1;
        /**
         * Total width of displayed number, including sign and decimal point.
         */
        uint_fast8_t total_number_width = 5;
        /**
         * Cached pointer to format string.
         */
        const char* format_string = format_strings[0];
        /**
         * Internal initializer
         */
        Parameter(const char* long_name, const char* short_name, const char* abbreviation,
            const char* units, const char* imperial_units,
            float raw_scale, float raw_offset, float metric_offset, float imperial_scale, float imperial_offset, 
            bool is_signed, uint_fast8_t integer_digits, uint_fast8_t fractional_digits) :
            raw_scale{raw_scale}, raw_offset{raw_offset}, metric_offset{metric_offset}, imperial_scale{imperial_scale}, imperial_offset{imperial_offset},
            long_name{long_name}, short_name{short_name}, abbreviation{abbreviation},
            units{units}, imperial_units{imperial_units},
            is_signed{is_signed}, integer_digits{integer_digits}, fractional_digits{fractional_digits},
            total_number_width{(uint_fast8_t)(integer_digits + fractional_digits + is_signed + 1 - (fractional_digits == 0 ? 2 : 0))},
            format_string{format_strings[is_signed]}
            {
                // that's all, folks
            }
        signed int raw_value = 0;
        /**
         * SI base units (namely, kelvins instead of degrees Celsius)
         */
        float si_value = 0.0f;
        /**
         * Common metric units (namely, degrees Celsius instead of kelvins)
         */
        float metric_value = 0.0f;
        /**
         * Imperial units (but not Rankines)
         */
        float imperial_value = 0.0f;
        /**
         * True if the cached string value needs to be updated.
         */
        bool string_is_dirty() const noexcept { return dirty_string; }
        /**
         * True if the cached string value needs to be updated.
         */
        bool imperial_string_is_dirty() const noexcept { return cached_string_imperial; }
        /**
         * Updates this parameter (for real).
         * @param new_value Raw, unadjusted value direct from CAN frame.
         */
        virtual void update(signed int new_value)
        {
            raw_value = new_value;
            si_value = raw_value * raw_scale + raw_offset;
            metric_value = si_value + metric_offset;
            imperial_value = si_value * imperial_scale + imperial_offset;
        }
        /**
         * Makes sure the cached string is up-to-date.
         */
        inline void update_string_do() noexcept { dirty_string = false; update_string(); }
        /**
         * Makes sure the cached string is up-to-date.
         */
        inline void update_string_imperial_do() noexcept { dirty_string_imperial = false; update_string_imperial(); }
        /**
         * Updates the cached string value.
         */
        virtual void update_string() noexcept
        {
            sprintf(cached_string, format_string, total_number_width, fractional_digits, metric_value);
        }
        /**
         * Updates the cached string value.
         */
        virtual void update_string_imperial() noexcept
        {
            sprintf(cached_string_imperial, format_string, total_number_width, fractional_digits, imperial_value);
        }
    private:
        static const char format_strings[2][16];
        /**
         * Tracks whether cached_string needs to be updated.
         */
        bool dirty_string = true;
        /**
         * Tracks whether cached_string_imperial needs to be updated.
         */
        bool dirty_string_imperial = true;
    protected:
        // I'm placing these last as hint to try to align them to cache lines.
        // TODO: Probably look up the Pi's actual cache line size and check if it makes any difference.
        /**
         * Instead of using the heap a ton for lots of string, just recycle the same memory.
         */
        char cached_string[MaxStringLength + 1] = 
        {
            '(', 'n', 'o', 'v', 'a', 'l', ')', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' 
        };
        /**
         * Instead of using the heap a ton for lots of string, just recycle the same memory.
         */
        char cached_string_imperial[MaxStringLength + 1] = 
        {
            '(', 'n', 'o', 'v', 'a', 'l', ')', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' 
        };
};


// 0.01 km/hr  =  0.002777 m/s  =  0.00911344 ft/s  =  0.00621371 mi/hr
// 1 kgf  =  9.80665 N  =  2.204623 lbf
// 1 N  =  0.101972 kgf  =  0.224809 lbf
// 100 kPa  =  14.50377 lb/in^2
// 1 kW  =  1.341022 hp
// 1 hp  =  745.6999 W
// gas constant  =  8.3144621 J / (mol * K)
// Avogadro's number  =  6.02214129e23
// 360 degrees  =  6.283185307 radians
// 1 radian  =  57.29577951 degrees
#define KPH_TO_MPH_FACTOR 0.621371f

#define PARAMETERS(NAME, LONG_NAME, SHORT_NAME, ABBREV, UNITS, IMPERIAL_UNITS, RAW_SCALE, RAW_OFFSET, METRIC_OFFSET, IMPERIAL_SCALE, IMPERIAL_OFFSET, IS_SIGNED, INTEGER_DIGITS, FRACTIONAL_DIGITS) private: \
        NAME() : Parameter(LONG_NAME, SHORT_NAME, ABBREV, UNITS, IMPERIAL_UNITS, RAW_SCALE, RAW_OFFSET, METRIC_OFFSET, IMPERIAL_SCALE, IMPERIAL_OFFSET, IS_SIGNED, INTEGER_DIGITS, FRACTIONAL_DIGITS) { } \
        NAME(const NAME&) = delete; \
        NAME& operator=(const NAME&) = delete; \
    public: \
        static const char* Units() noexcept { return Instance.GetUnits(); } \
        static float Value() noexcept { return Instance.GetValue(); } \
        static const char* String() noexcept { return Instance.GetString(); } \
        static NAME Instance        

#define MANUAL_UPDATE() public: void update(signed int new_value)


/**
 * Brake pedal position
 */
class BrakePedal : public Parameter
{
        PARAMETERS(BrakePedal, "Brake pedal position", "Brake", "BRK", "%", "%", 100.0f / 127, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 0);
};

class AcceleratorPosition : public Parameter
{
        PARAMETERS(AcceleratorPosition, "Accelerator pedal position", "Gas", "Gas", "?", "?", 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 0);
};

/**
 * Traction battery amperage, as periodically reported.
 */
class BatteryCurrent : public Parameter
{
        PARAMETERS(BatteryCurrent, "Traction battery current", "Batt. current", "B I", "A", "A", 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 1);
        MANUAL_UPDATE();
};

/**
 * Traction battery voltage, as periodically reported.
 */
class BatteryPotentional : public Parameter
{
        PARAMETERS(BatteryPotentional, "Traction battery potentional difference", "Batt. volts", "B V", "V", "V", 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 0);
};


/**
 * Battery charge state
 */
class BatterySoC : public Parameter
{
        PARAMETERS(BatterySoC, "Battery state of charge", "Batt. SoC", "SoC", "%", "%", 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 1);
};


/**
 * Battery delta SoC.
 * This is the computed difference in charge between the most-charged and
 * least-charged battery modules.
 */
class BatteryDeltaSoC : public Parameter
{
        PARAMETERS(BatteryDeltaSoC, "Battery state of charge delta", "Batt. dSoC", "dSoC", "%", "%", 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, false, 3, 1);
};


/**
 * Battery lowest temp
 */
class BatteryTempLowest : public Parameter
{
//                 NAME,              
        PARAMETERS(BatteryTempLowest, "Battery temperature lowest", "Batt. temp low", "B T low",
//      UNITS, UNITS, RAW_SCALE, RAW_OFFSET, METRIC_OFFSET, IMPERIAL_SCALE, IMPERIAL_OFFSET, IS_SIGNED, INTEGER_DIGITS, FRACTIONAL_DIGITS
        "C",   "F",   1.0f,      -273.15f,   273.15f,       5.0f / 9.0f,    459.67f,         true,      3,              0);
};


/**
 * Battery highest temp
 */
class BatteryTempHighest : public Parameter
{
//                 NAME,               
        PARAMETERS(BatteryTempHighest, "Battery temperature highest", "Batt. temp high", "B T high",
//      UNITS, UNITS, RAW_SCALE, RAW_OFFSET, METRIC_OFFSET, IMPERIAL_SCALE, IMPERIAL_OFFSET, IS_SIGNED, INTEGER_DIGITS, FRACTIONAL_DIGITS
        "C",   "F",   1.0f,      -273.15f,   273.15f,       5.0f / 9.0f,    459.67f,         true,      3,              0);
};


/**
 * Front left wheel speed (probably for ABS?)
 */
class WheelFrontLeft : public Parameter
{
        PARAMETERS(WheelFrontLeft, "Front left wheel speed", "Wheel F L", "F L", "kph", "mph", 0.01f, 0.0f, 0.0f, KPH_TO_MPH_FACTOR, 0.0f, true, 3, 2);
};

/**
 * Front right wheel speed (probably for ABS?)
 */
class WheelFrontRight : public Parameter
{
        PARAMETERS(WheelFrontRight, "Front right wheel speed", "Wheel F R", "F R", "kph", "mph", 0.01f, 0.0f, 0.0f, KPH_TO_MPH_FACTOR, 0.0f, true, 3, 2);
};

/**
 * Rear left wheel speed (probably for ABS?)
 */
class WheelRearLeft : public Parameter
{
        PARAMETERS(WheelRearLeft, "Rear left wheel speed", "Wheel R L", "R L", "kph", "mph", 0.01f, 0.0f, 0.0f, KPH_TO_MPH_FACTOR, 0.0f, true, 3, 2);
};

/**
 * Rear right wheel speed (probably for ABS?)
 */
class WheelRearRight : public Parameter
{
        PARAMETERS(WheelRearRight, "Rear right wheel speed", "Wheel R R", "R R", "kph", "mph", 0.01f, 0.0f, 0.0f, KPH_TO_MPH_FACTOR, 0.0f, true, 3, 2);
};


/**
 * Overall vehicle speed.
 */
class Speed : public Parameter
{
        PARAMETERS(Speed, "Vehicle speed", "Speed", "speed", "kph", "mph", 0.01f, 0.0f, 0.0f, KPH_TO_MPH_FACTOR, 0.0f, true, 3, 2);
};


class EngineCoolantTemp : public Parameter
{
//             NAME,              
        PARAMETERS(EngineCoolantTemp, "Engine coolant temperature", "ICE coolant", "ICE temp",
//      UNITS, UNITS, RAW_SCALE, RAW_OFFSET, METRIC_OFFSET, IMPERIAL_SCALE, IMPERIAL_OFFSET, IS_SIGNED, INTEGER_DIGITS, FRACTIONAL_DIGITS
        "C",   "F",   0.5f,      -273.15f,   273.15f,       5.0f / 9.0f,    459.67f,         true,      3,              0);
};


#undef PARAMETERS
#undef MANUAL_UPDATE

} /* namespace Vehicle */
#endif /* CAN_FRAME_TYPES_H */
