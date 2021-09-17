#include "fbdev.hpp"
#include "global.hpp"

#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sysexits.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>

using namespace LVGL;

Framebuffer Framebuffer::instance { };

Framebuffer::Framebuffer()
{
    // Select framebuffer device to open
    fbPath = getenv("FRAMEBUFFER");
	if (!fbPath) fbPath = "/dev/fb0";

    // Open a handle
    fb_handle = open(fbPath, O_RDWR);
    if (fb_handle < 0)
        throw std::runtime_error("Unable to open a framebuffer.");
    
    // Get info
    int error = ioctl(fb_handle, FBIOGET_VSCREENINFO, &vinfo);
	if (error)
    {
        close(fb_handle);
        throw std::runtime_error("ioctl: failed to get FBIOGET_VSCREENINFO");
    }
    orig_vinfo = vinfo;

    // RPi4 likes to force this to 16 so we'll stick with that
    if (vinfo.bits_per_pixel != 16)
    {
        vinfo.bits_per_pixel = 16;
        //std::cout << "Test: " << vinfo.xres << " " << vinfo.yres << " " << vinfo.bits_per_pixel << "\n";
        error = ioctl(fb_handle, FBIOPUT_VSCREENINFO, &vinfo);
        if (error)
        {
            close(fb_handle);
            throw std::runtime_error("ioctl: failed to set 16 bpp mode");
        }
    }
	
    // But now, like, get /more/ info
    error = ioctl(fb_handle, FBIOGET_FSCREENINFO, &finfo);
	if (error)
    {
        close(fb_handle);
        throw std::runtime_error("ioctl: failed to get FBIOGET_FSCREENINFO");
    }
    
    // Map buffer into address space
    framebuffer = (unsigned char*)mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_handle, 0);
	if (framebuffer == MAP_FAILED)
    {
        close(fb_handle);
        throw std::runtime_error("mmap: failed to map framebuffer into address space.");
    }

    // Erase screen
    memset(framebuffer, 0, finfo.smem_len);

    // Register with LVGL
    Global::EnsureInitialized();
    int pixels = vinfo.xres * vinfo.yres;
    buffer1 = new lv_color_t[pixels];
#ifdef FRAMEBUFFER_DOUBLE_BUFFER
    buffer2 = new lv_color_t[pixels];
    lv_disp_draw_buf_init(&draw_buffer, buffer1, buffer2, pixels);
#else
    lv_disp_draw_buf_init(&draw_buffer, buffer1, nullptr, pixels);
#endif
    lv_disp_drv_init(&driver);
    driver.user_data = this;
    driver.draw_buf = &draw_buffer;
    driver.hor_res = vinfo.xres;
    driver.ver_res = vinfo.yres;
    driver.flush_cb = &thunk;
    lv_disp_drv_register(&driver);
}


void Framebuffer::thunk(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    ((Framebuffer*)drv->user_data)->flush(area, color_p);
}


void Framebuffer::flush(const lv_area_t * area, lv_color_t * color_p)
{
    // Generally adapted from https://github.com/lvgl/lv_port_linux_frame_buffer
    if (framebuffer == nullptr ||
            area->x2 < 0 ||
            area->y2 < 0 ||
            area->x1 > (int32_t)vinfo.xres - 1 ||
            area->y1 > (int32_t)vinfo.yres - 1) {
        lv_disp_flush_ready(&driver);
        return;
    }

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > (int32_t)vinfo.xres - 1 ? (int32_t)vinfo.xres - 1 : area->x2;
    int32_t act_y2 = area->y2 > (int32_t)vinfo.yres - 1 ? (int32_t)vinfo.yres - 1 : area->y2;

    lv_coord_t w = (act_x2 - act_x1 + 1);
    long int location = 0;
    long int byte_location = 0;
    unsigned char bit_location = 0;

    switch (vinfo.bits_per_pixel)
    {
#ifdef FRAMEBUFFER_SUPPORT_24_BPP
        case 32: /*32 or 24 bit per pixel*/
        case 24:
            uint32_t * fbp32 = (uint32_t *)framebuffer;
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) {
                location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 4;
                memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
                color_p += w;
            }
            break;
#endif
#ifdef FRAMEBUFFER_SUPPORT_16_BPP
        case 16: /*16 bit per pixel*/
            uint16_t * fbp16 = (uint16_t *)framebuffer;
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) {
                location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length / 2;
                memcpy(&fbp16[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 2);
                color_p += w;
            }
            break;
#endif
#ifdef FRAMEBUFFER_SUPPORT_8_BPP
        case 8: /*8 bit per pixel*/
            uint8_t * fbp8 = (uint8_t *)framebuffer;
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) {
                location = (act_x1 + vinfo.xoffset) + (y + vinfo.yoffset) * finfo.line_length;
                memcpy(&fbp8[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1));
                color_p += w;
            }
            break;
#endif
#ifdef FRAMEBUFFER_SUPPORT_1_BPP
        case 1: /*1 bit per pixel*/
            uint8_t * fbp8 = (uint8_t *)framebuffer;
            int32_t x;
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) {
                for(x = act_x1; x <= act_x2; x++) {
                    location = (x + vinfo.xoffset) + (y + vinfo.yoffset) * vinfo.xres;
                    byte_location = location / 8; /* find the byte we need to change */
                    bit_location = location % 8; /* inside the byte found, find the bit we need to change */
                    fbp8[byte_location] &= ~(((uint8_t)(1)) << bit_location);
                    fbp8[byte_location] |= ((uint8_t)(color_p->full)) << bit_location;
                    color_p++;
                }

                color_p += area->x2 - act_x2;
            }
            break;
#endif
    }

    lv_disp_flush_ready(&driver);
}


Framebuffer::~Framebuffer()
{
    munmap(framebuffer, finfo.smem_len);
    // I guess this could fail, but this will only run during shutdown, so it's
    // not like there's any point in throwing an error.
    ioctl(fb_handle, FBIOPUT_VSCREENINFO, &orig_vinfo);
    close(fb_handle);
}
