#include <iostream>
#include "Vehicle/CANBus.hpp"
#include <stdio.h>
#include "Vehicle/ProcessFrame.hpp"
#include "Vehicle/Parameter.hpp"
#include "Vehicle/CANTrafficProcessor.hpp"

#include <time.h>

#include <ncurses.h>

#include "../libs/lv_conf.h"
#include "../libs/lvgl/lvgl.h"

#include "UI/lvgl/fbdev.hpp"
#include "UI/lvgl/keyboard.hpp"
#include "UI/lvgl/core/object.hpp"

#include "UI/ParameterDisplay.hpp"

using namespace std;
using namespace Vehicle;

#define KPHTOMPH(x) (x * 0.621371)

/**
 * Honestly a horrible syntax hack.
 */
#define ms * 1000000

int main()
{
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Terrible test hack 2.");
    //lv_obj_add_style(label, &style1, 0);
	lv_label_set_text_fmt(label, "Test %p", lv_scr_act());
	lv_timer_handler();
	

	CANTrafficProcessor processor { *(new CANBus()) };
	processor.Begin();
	struct timespec update_period = { .tv_sec = 0, .tv_nsec = 20 ms };
	while (1)
	{
		mvprintw(0, 0, "Speed: %7.2f %s\n", Speed::Value(), Speed::Units());
		printw("Wheels FL FR RL RR: %7.2f %7.2f %7.2f %7.2f %s\n",
			WheelFrontLeft::Value(), WheelFrontRight::Value(),
			WheelRearLeft::Value(), WheelRearRight::Value(),
			WheelFrontLeft::Units());
		printw("Battery V&A: %3.0f %s  %5.1f %s  %8.1f W\n",
			BatteryPotentional::Value(), BatteryPotentional::Units(),
			BatteryCurrent::Value(), BatteryCurrent::Units(),
			BatteryCurrent::Value() * BatteryPotentional::Value());
		printw("Battery health: SoC %5.1f %s   dSoC %5.1f %s  min %2.0f %s  max %2.0f %s\n",
			BatterySoC::Value(), BatterySoC::Units(), 
			BatteryDeltaSoC::Value(), BatteryDeltaSoC::Units(), 
			BatteryTempLowest::Value(), BatteryTempLowest::Units(),
			BatteryTempHighest::Value(), BatteryTempHighest::Units());
		printw("Brake pedal position: %3.0f %s\n", BrakePedal::Value(), BrakePedal::Units());
		printw("Engine coolant: %5.1f %s\n",
			EngineCoolantTemp::Value(), EngineCoolantTemp::Units());
		
		refresh();
		lv_timer_handler();
		nanosleep(&update_period, nullptr);
	}
	return 0;
}
