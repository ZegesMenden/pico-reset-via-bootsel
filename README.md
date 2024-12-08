# pico-reset-via-bootsel
## Library for the pico sdk that allows resetting via a double-tap of the boot select pin

## Implementation notes:
* Enabling resets within ~50ms of boot seems to automatically trigger a reset, not sure why.
* The pico-sdk implementation of reading the bootsel button sets RESET_BUTTON_POLL_DELAY_DUR to 1000, but i have found that 10 works well. Keep in mind that increasing this value massively impacts the runtime of the irq handler. With default settings the irq only takes ~53 cycles to run.