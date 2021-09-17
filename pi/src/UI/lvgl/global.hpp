#ifndef LVGL_CPP_H
#define LVGL_CPP_H

#include "lv_drv_conf.h"
#include "../../../libs/lvgl/lvgl.h"

namespace LVGL
{

/**
 * Handles global state of LVGL, namely initialization.
 */
class Global
{
    public:
        static void EnsureInitialized() { GetInstance(); }
        static Global& GetInstance();
    private:
        Global();
        ~Global();
        static Global* instance;
};

} /* namespace LVGL */

#endif /* LVGL_CPP_H */
