#ifndef PARAMETER_LABEL_H
#define PARAMETER_LABEL_H
#include "lvgl/core/lvgl.hpp"
#include "lvgl/core/object.hpp"
#include "lvgl/core/label.hpp"
#include "../Vehicle/Parameter.hpp"

namespace UI
{

/**
 * Displays continously updating data from a vehicle data parameter.
 */
class ParameterLabel : public LVGL::Label
{
    public:
    // maybe use a timer to trigger updates?
    // probably better to have each screen force things to update
        ParameterLabel(Vehicle::Parameter& parameter, Object& parent, bool show_units = true, bool imperial = false);
        ParameterLabel(Vehicle::Parameter& parameter) : ParameterLabel(parameter, LVGL::Screen::Active()) { }
        /**
         * Reference to actual Parameter being monitored.
         */
        Vehicle::Parameter& Parameter;
        /**
         * Sets units to be shown.
         */
        ParameterLabel& ShowUnits() noexcept
        {
            if (show_units)
                return *this;
            show_units = true;
            RefreshHandler();
            return *this;
        }
        /**
         * Sets units to be shown or not.
         * @param value true to show, false to hide
         */
        ParameterLabel& ShowUnits(bool value) noexcept
        {
            if (show_units ^ value)
            {
                show_units = value;
                RefreshHandler();
            }
            return *this;
        }
        /**
         * Sets units not to be shown.
         */
        ParameterLabel& HideUnits() noexcept
        {
            if (!show_units)
                return *this;
            show_units = false;
            RefreshHandler();
            return *this;
        }
        /**
         * Check if units are configured to show.
         */
        bool ShowingUnits() const noexcept { return show_units; }
        /**
         * Set imperial mode.
         */
        ParameterLabel& SetImperial() noexcept
        {
            if (imperial)
                return *this;
            imperial = true;
            RefreshHandler();
            return *this;
        }
        /**
         * Set metric mode.
         */
        ParameterLabel& SetMetric() noexcept
        {
            if (!imperial)
                return *this;
            imperial = false;
            RefreshHandler();
            return *this;
        }
        /**
         * Set whether imperial display mode is active.
         * @param value true for imperial, false for metric.
         */
        ParameterLabel& SetImperial(bool value) noexcept
        {
            if (imperial ^ value)
            {
                imperial = value;
                RefreshHandler();
            }
            return *this;
        }
        /**
         * Check if imperial display mode is set.
         */
        bool IsImperial() const noexcept { return imperial; }
        /**
         * Maximum length of a string value.
         */
        static const std::size_t MaxStringLength = 31;
    private:
        char cached_string[MaxStringLength + 1] = 
        {
            '(', 'n', 'o', 'v', 'a', 'l', ')', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', 
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' 
        };
        void RefreshHandler() noexcept;
        bool show_units = true;
        bool imperial = false;
};

}

#endif /* PARAMETER_LABEL_H */
