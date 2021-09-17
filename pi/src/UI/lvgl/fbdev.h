/**
 * @file fbdev.h
 *
 */

#ifndef FBDEV_H
#define FBDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_drv_conf.h"
#include "../../../libs/lvgl/lvgl.h"

void fbdev_init(void);
void fbdev_exit(void);
void fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
void fbdev_get_sizes(uint32_t *width, uint32_t *height);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*FBDEV_H*/
