#include <iostream>
#include "Vehicle/CAN.hpp"
#include <stdio.h>
#include "Vehicle/ProcessFrame.hpp"
#include "Vehicle/Parameter.hpp"

#include <ncurses.h>

using namespace std;
using namespace Vehicle;

#define KPHTOMPH(x) (x * 0.621371)

static struct ncurses
{
	ncurses() { initscr(); cbreak(); noecho(); }
	~ncurses() { endwin(); }
} instance;


int main()
{
	CANBus can { };
	CANFrame frame;
	while (1)
	{
		if (can.Receive(frame))
		{
			ProcessFrame(frame);
			mvprintw(0, 0, "Speed: %7.2f %s\n", Speed::Value(), Speed::Units());
			printw("Wheels FL FR RL RR: %7.2f %7.2f %7.2f %7.2f %s\n",
				WheelFrontLeft::Value(), WheelFrontRight::Value(),
				WheelRearLeft::Value(), WheelRearRight::Value(),
				WheelFrontLeft::Units());
			printw("Battery V&A: %3.0f %s  %5.1f %s  %8.1f W\n",
				BatteryCurrent::Value(), BatteryCurrent::Units(),
				BatteryPotentional::Value(), BatteryPotentional::Units(),
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
		}
	}
	return 0;
}
