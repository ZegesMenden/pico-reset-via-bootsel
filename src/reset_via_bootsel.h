#pragma once

#include "pico/stdlib.h"

#ifndef RESET_BUTTON_POLL_HZ
#define RESET_BUTTON_POLL_HZ 30
#endif

#ifndef RESET_BUTTON_TIMEOUT_US
#define RESET_BUTTON_TIMEOUT_US 500000
#endif

#ifndef RESET_BUTTON_PIN
#define RESET_BUTTON_PIN 1
#endif

#ifndef RESET_BUTTON_PRESS_CNT
#define RESET_BUTTON_PRESS_CNT 2
#endif

#ifndef RESET_BUTTON_POLL_DELAY_DUR
#define RESET_BUTTON_POLL_DELAY_DUR 10
#endif

bool enable_reset_via_bootsel();