#ifndef LVGL_HPP
#define LVGL_HPP

#include "../../../../libs/lvgl/lvgl.h"

/* Configuration options:
 * LVGL_GET_WRAPPER_NOT_PARANOID
 *     Specifies that Object::GetWrapper shouldn't verify that user_data is a
 *     valid Object.  This detection isn't 100 % guaranteed to detect this
 *     mistake.
 * LVGL_IGNORE_DELAYED_DOUBLE_DELETE_ERROR
 *     DeleteDelayed and DeleteAsync signal an intention to delete an object
 *     later, and necessarily imply dynamic allocation.
 */

namespace LVGL
{

/**
 * Handles global state of LVGL, namely initialization.
 */
class Global
{
    public:
        /**
         * Ensure LVGL is initialized.
         */
        static void EnsureInitialized() { GetInstance(); }
        /**
         * @todo This is probably pointless?
         */
        static Global& GetInstance();
    private:
        Global();
        ~Global();
        static Global* instance;
};


}

#endif /* LVGL_HPP */
