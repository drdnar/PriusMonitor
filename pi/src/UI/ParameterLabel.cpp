#include "ParameterLabel.hpp"

using namespace UI;
using namespace Vehicle;
using namespace LVGL;




ParameterLabel::ParameterLabel(Vehicle::Parameter& parameter, Object& parent) : Label(parent), Parameter { parameter }
{
    OnEvent(LV_EVENT_REFRESH, &ParameterLabel::RefreshHandler);
    SetTextStatic("init");
}


ParameterLabel::ParameterLabel(Vehicle::Parameter& parameter) : ParameterLabel(parameter, Screen::Active())
{
    // nothing else to do
}


void ParameterLabel::RefreshHandler()
{
    //SetText("%f %s", Parameter.GetValue(), Parameter.GetUnits());
    SetTextStatic(Parameter.GetString());
}
