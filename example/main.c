#include <stdio.h>
#include "pico/stdlib.h"
#include "reset_via_bootsel.h"

int main()
{
    stdio_init_all();

    getchar(); // Wait for serial terminal to open
    sleep_ms(50);

    if ( !enable_reset_via_bootsel() ) {
        printf("Failed to enable reset via bootsel\n");
        while(1);
    } else {
        printf("Reset via bootsel enabled\n");
    }

    while (1) {
        printf("Hello, world!\n");
        sleep_ms(500);
    }

}
