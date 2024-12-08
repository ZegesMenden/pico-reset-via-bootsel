#include "reset_via_bootsel.h"

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#include "pico/bootrom.h"

// modified from pico-sdk
// this function should only take ~53 cycles to execute
// https://github.com/raspberrypi/pico-examples/blob/b6ac07f1946271de2817f94d8ffc0425ecb7c2a9/picoboard/button/button.c#L25
static bool __no_inline_not_in_flash_func(get_bootsel_button)() {

    // might be unecesarry, not 100% sure
    // removing this saves 5 cycles
    uint32_t tmp = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[RESET_BUTTON_PIN].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now, so we just busy wait
    // The pico SDK has this loop at 1000 iterations, but from my testing it seems like 1 is enough
    for (volatile int i = 0; i < RESET_BUTTON_POLL_DELAY_DUR; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
#if PICO_RP2040
    #define CS_BIT (1u << 1)
#else
    #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
#endif
    bool button_state = !(sio_hw->gpio_hi_in & CS_BIT);

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[RESET_BUTTON_PIN].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(tmp);

    return button_state;
}

static bool bootsel_timer_callback(repeating_timer_t *rt) {
    static bool last_button_state = false;
    static int button_press_count = 0;
    static uint64_t last_press_time = 0;

    bool button_state = get_bootsel_button();
    uint64_t t_cur = time_us_64();

    bool timeout_passed = t_cur - last_press_time > RESET_BUTTON_TIMEOUT_US;

    // Button press logic
    if (button_state != last_button_state) {
        button_press_count += button_state & !timeout_passed; 
        last_press_time = t_cur;
    }

    // Reset the button press count if the timeout has passed
    if ( timeout_passed) {
        button_press_count = 0;
    }

    // Reset if the button has been pressed for the required number of times
    if (button_press_count >= RESET_BUTTON_PRESS_CNT) {
        reset_usb_boot(0, 0);
    }

    last_button_state = button_state;

    return true;
}

bool enable_reset_via_bootsel() {

    static repeating_timer_t timer;
    return add_repeating_timer_us(-1000000 / RESET_BUTTON_POLL_HZ, bootsel_timer_callback, NULL, &timer);

}