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
        ParameterLabel(Vehicle::Parameter& parameter, Object& parent);
        ParameterLabel(Vehicle::Parameter& parameter);
        Vehicle::Parameter& Parameter;
    private:
        void RefreshHandler();
};

}

#endif /* PARAMETER_LABEL_H */
