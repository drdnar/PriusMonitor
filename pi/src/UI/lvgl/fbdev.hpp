#ifndef FBDEV_H
#define FBDEV_H

#include "lv_drv_conf.h"
#include "../../../libs/lvgl/lvgl.h"
#include <linux/fb.h>

#define FRAMEBUFFER_SUPPORT_16_BPP

namespace LVGL
{

class Framebuffer
{
    public:
        static Framebuffer& GetInstance() { return instance; }
        int GetWidth() const noexcept { return vinfo.width; }
        int GetHeight() const noexcept { return vinfo.height; }
    private:
        Framebuffer();
        ~Framebuffer();
        /**
         * Singleton to ensure initialization of the main framebuffer.
         */
        static Framebuffer instance;
        /**
         * Flush a buffer to the marked area
         * @param area an area where to copy `color_p`
         * @param color_p an array of pixel to copy to the `area` part of the screen
         */
        void flush(const lv_area_t * area, lv_color_t * color_p);
        /**
         * Thunks up the way to call into the actal Framebuffer object.
         */
        static void thunk(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);
        /**
         * Name of the framebuffer device.
         */
        const char* fbPath;
        /**
         * Original framebuffer settings.
         */
        struct fb_var_screeninfo orig_vinfo;
        /**
         * Cache of actual framebuffer settings.
         */
        struct fb_var_screeninfo vinfo;
        /**
         * Cache of actual framebuffer settings.
         */
        struct fb_fix_screeninfo finfo;
        /**
         * Pointer to framebuffer memory.
         */
        unsigned char* framebuffer;
        /**
         * Low-level handle for the framebuffer object.
         */
        int fb_handle = 0;
        /**
         * LVGL draw buffer 1.
         */
        lv_color_t* buffer1;
#ifdef FRAMEBUFFER_DOUBLE_BUFFER
        /**
         * LVGL draw buffer 2.
         */
        lv_color_t* buffer2;
#endif
        /**
         * LVGL draw buffer descriptor.
         */
        lv_disp_draw_buf_t draw_buffer;
        /**
         * LVGL display driver descriptor.
         */
        lv_disp_drv_t driver;
};

} /* namespace LVGL */

#endif /*FBDEV_H*/
