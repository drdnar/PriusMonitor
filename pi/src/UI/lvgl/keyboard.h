#ifndef NCURSES_KEYBOARD_H
#define NCURSES_KEYBOARD_H

#include "lv_drv_conf.h"
#include "../../../libs/lvgl/lvgl.h"
#include "../../../libs/lvgl/src/hal/lv_hal_indev.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Get the last pressed or released character from the PC's keyboard
 * @param indev_drv pointer to the related input device driver
 * @param data store the read data here
 */
void keyboard_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NCURSES_KEYBOARD_H */
