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
}


Global::~Global( )
{
    lv_deinit();
    delete instance;
}
