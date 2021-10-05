#include "lvgl.hpp"
#include "object.hpp"

using namespace LVGL;

Global* Global::instance = nullptr;


Global& Global::GetInstance()
{
    if (instance)
        return *instance;
    else
        return *(instance = new Global());
}


Global::Global()
{
    lv_init();
    // TODO: This should really be handled by the display system.
    new Screen(lv_scr_act());
}


Global::~Global( )
{
    lv_deinit();
    delete instance;
}
