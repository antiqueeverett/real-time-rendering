#include <rtr/gamepad/updates.h>

glm::vec3 rot_axis_ws_{1.0, 0.0, 0.0};
glm::vec3 rot_axis_ad_{0.0, 1.0, 0.0};
glm::vec3 rot_axis_qe_{0.0, 0.0, 1.0};

// This bounds any input to within the range of a short
short bound_short(int num)
{
    if (num > SHRT_MAX)
        return SHRT_MAX;
    else if (num < SHRT_MIN)
        return SHRT_MIN;
    else
        return num;
}

// Maps a value on an input range to an output range. Looks similar to the Arduino code, yeah?
short map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Specifically maps to the range of a short, like above. Not *really* necessary but nice to have.
short short_map(long x)
{
    return (x - SHRT_MIN) * (2*SHRT_MAX - 2*SHRT_MIN) / (SHRT_MAX - SHRT_MIN) + 2*SHRT_MIN;
}

void update_buttons(Joystick* joy, std::map<unsigned char, float>* map)
{
    ButtonId update = joy->getUpdatedButton();
    unsigned int value = joy->getButtonState(update);

    switch (update)
    {
    case BUTTON_SELECT:
    case BUTTON_LEFT_JOYSTICK:
    case BUTTON_RIGHT_JOYSTICK:
    case BUTTON_START:
    case BUTTON_DPAD_UP:
    case BUTTON_DPAD_RIGHT:
    case BUTTON_DPAD_DOWN:
    case BUTTON_DPAD_LEFT:
    case BUTTON_LEFT_TRIGGER:
    case BUTTON_RIGHT_TRIGGER:
    case BUTTON_LEFT_BUMPER:
    case BUTTON_RIGHT_BUMPER:
    case BUTTON_TRIANGLE:
    case BUTTON_CIRCLE:
    case BUTTON_X:
    case BUTTON_SQUARE:
    case BUTTON_PS3:
    default:
        std::cout << "button: " << update << " value: " << value <<std::endl;
        break;
    }
}

void update_axes(Joystick* joy, std::map<unsigned char, float>* map)
{
    AxisId update = joy->getUpdatedAxis();
    int16_t value = joy->getAxisState(update);

    switch (update)
    {
    case AXIS_ACCEL_X:
        break;
    case AXIS_ACCEL_Y:
        break;
    case AXIS_ACCEL_Z:
        break;
    // As of yet I do not know what the AXIS_DPAD_LEFT value should be - or if it even exists
    case NA4:
    case NA5:
    case NA6:
    case NA7:
    case NA20:
    case NA21:
    case NA22:
    case AXIS_DPAD_LEFT:
        std::cout << "\tDPAD LEFT" << std::endl;
    case AXIS_LEFT_STICK_HORIZONTAL:
        if(value == 0){
                map->at('a') = 0.0f;
                map->at('d') = 0.0f;
            } else if(value < 0 && value > -32766){
                map->at('a') = -value / 32767.f;
                map->at('d') = 0.0f;
            } else if (value < 32766) {
                map->at('a') = 0.0f;
                map->at('d') = value / 32767.f;
        }
        break;
    case AXIS_LEFT_STICK_VERTICAL:
        if(value == 0){
            map->at('w') = 0.0f;
            map->at('s') = 0.0f;
        } else if(value < 0 && value > -32766){
            map->at('w') = -value / 32767.f;
            map->at('s') = 0.0f;
        } else if (value < 32766) {
            map->at('w') = 0.0f;
            map->at('s') = value / 32767.f;
        }
        break;
    case AXIS_RIGHT_STICK_HORIZONTAL:
    case AXIS_RIGHT_STICK_VERTICAL:
    case AXIS_DPAD_UP:
    case AXIS_DPAD_RIGHT:
    case AXIS_DPAD_DOWN:
    case AXIS_LEFT_TRIGGER:
    case AXIS_RIGHT_TRIGGER:
    case AXIS_LEFT_BUMPER:
    case AXIS_RIGHT_BUMPER:
    case AXIS_TRIANGLE:
    case AXIS_CIRCLE:
    case AXIS_X:
    case AXIS_SQUARE:
    default:
        std::cout << "axis: " << update << " value: " << value << std::endl;
        break;
    }
}
