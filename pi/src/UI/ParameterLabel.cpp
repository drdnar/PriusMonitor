#include "ParameterLabel.hpp"

using namespace UI;
using namespace Vehicle;
using namespace LVGL;


ParameterLabel::ParameterLabel(Vehicle::Parameter& parameter, Object& parent, bool show_units, bool imperial)
    : Label(parent), Parameter { parameter }, show_units{show_units}, imperial{imperial}
{
    OnEvent(LV_EVENT_REFRESH, &ParameterLabel::RefreshHandler);
}



void ParameterLabel::RefreshHandler() noexcept
{
    if (show_units)
    {
        if (!imperial)
            sprintf(cached_string, "%s %s", Parameter.GetString(), Parameter.GetUnits());
        else
            sprintf(cached_string, "%s %s", Parameter.GetStringImperial(), Parameter.GetUnitsImperial());
        SetTextStatic(cached_string);
    }
    else
        SetTextStatic(Parameter.GetString());
}
