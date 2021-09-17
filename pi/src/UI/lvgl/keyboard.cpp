#include "keyboard.h"
#include <ncurses.h>

void keyboard_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    switch(getch())
    {
        case KEY_PPAGE:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_PREV;
            break;
        case KEY_NPAGE:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_NEXT;
            break;
        case KEY_UP:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_UP;
            break;
        case KEY_DOWN:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_DOWN;
            break;
        case KEY_LEFT:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_LEFT;
            break;
        case KEY_RIGHT:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_RIGHT;
            break;
        case KEY_ENTER:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_ENTER;
            break;
        case KEY_EXIT:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_ESC;
            break;
        case KEY_HOME:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_HOME;
            break;
        case KEY_END:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_END;
            break;
        case KEY_BACKSPACE:
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = LV_KEY_BACKSPACE;
            break;
        default:
            data->state = LV_INDEV_STATE_RELEASED;
            break;
    }
}
