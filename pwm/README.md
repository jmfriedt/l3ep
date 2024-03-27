## PWM functions

* ``void pwm_configure()``: called once for initializing the timer period and pin output
* ``void pwmD_configure(short)``: called for updating the duty cycle on the PWM output with the value given as argument, between 0 and the period
