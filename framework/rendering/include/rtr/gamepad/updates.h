#pragma once

#include <rtr/gamepad/joy.h>
#include <rtr/object/object.hpp>
#include <iostream>
#include <climits>

void update_buttons(Joystick* joy, Object* obj);
void update_axes(Joystick* joy, Object* obj);

short bound_short(int num);
short map(long x, long in_min, long in_max, long out_min, long out_max);
short short_map(long x);
