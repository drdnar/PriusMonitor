#ifndef NCURSES_KEYBOARD_H
#define NCURSES_KEYBOARD_H

#include "lv_drv_conf.h"
#include "../../../libs/lvgl/lvgl.h"
#include "../../../libs/lvgl/src/hal/lv_hal_indev.h"

namespace LVGL
{

/**
 * Class to provide access to ncurses character input.
 */
class ncursesKeyboard
{
    public:
        /**
         * Get an instance, in case there's ever methods for this.
         */
        static ncursesKeyboard& GetInstance() { return instance; }
        operator lv_indev_t*() const { return device_instance; }
    private:
        ncursesKeyboard();
        ~ncursesKeyboard();
        static ncursesKeyboard instance;
        /**
         * LVGL input driver descriptor.
         */
        lv_indev_drv_t indev_drv;
        /**
         * Device instance, for associating with LVGL objects.
         */
        lv_indev_t* device_instance = nullptr;
        /**
         * Get the last pressed or released character from the terminal's keyboard
         * @param indev_drv pointer to the related input device driver
         * @param data store the read data here
         */
        static void read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);
};

} /* namespace LVGL */

#endif /* NCURSES_KEYBOARD_H */
