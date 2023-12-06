#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <hal/nrf_gpio.h>

// Configuración del PWM 
//#define PWM_DEV_NAME DT_LABEL(DT_ALIAS(pwm0)) // P0.13
//#define PWM_CHANNEL 0

//#define PWM_DEV_NAME DT_LABEL(DT_ALIAS(pwm_led0))
#define PWM_NODE  DT_NODELABEL(pwm_led0)
#define PWM_CTRL  DT_PWMS_CTLR(PWM_NODE)
#define PWM_CHAN  DT_PWMS_CHANNEL(PWM_NODE)
#define PWM_FLAGS DT_PWMS_FLAGS(PWM_NODE)

// PWM Period, 50 Hz for example
#define LED_PWM_PERIOD_US (USEC_PER_SEC / 50U)

const struct device *pwm_dev = DEVICE_DT_GET(PWM_CTRL);

//
const uint8_t app_indication_pattern_glow[] = {255,100,50,225}

// Timers
K_TIMER_DEFINE(pwm_change_timer, pwm_change_fn, NULL);

// Start repeat timer
k_timer_start(&pwm_change_timer, K_MSEC(200), K_MSEC(200));

pulse = duty * PWM_PERIOD_US / 255U;
if (pwm_dev == NULL || !
device_is_ready(pwm_dev))
{
	LOG_ERR("Error");
}
err = pwm_pin_set_usec(pwm_dev, PWM_CHAN, PWM_PERIOD_US, pulse, PWM_FLAGS);
if (err)
{
	LOG_ERR("Pwm set fail. Err: %i",err);
	return err;
}    // 

// Timer function
void pwm_change_fn(struct k_timer *dummy){
    uint32_t pulse = PWM_PERIOD_US;
    switch (current_mode)
    {
    case app_indication_glow:
        pulse = app_indication_pattern_glow[pattern_index++ % sizeof(app_indication_pattern_glow)] * PWM_PERIOD_US / 255U;
    default:
        break;
    };
    int err = pwm_pin_set_usec(pwm_dev, PWM_CHAN, PWM_PERIOD_US, pulse, PWM_FLAGS);
    //if (err)
}

void Motor() 
{
    const struct device *pwm_dev;

    pwm_dev = device_get_binding(PWM_NODE);
    if (!pwm_dev) {
        return;
    }

    struct pwm_config pwm_cfg = {
        .frequency = 1000, 
        .period = USEC_PER_SEC / 1000,
        .pulse_width = 0, 
        .flags = 0,
    };

    int ret = pwm_pin_set_config(pwm_dev, PWM_CHANNEL, &pwm_cfg);
    if (ret < 0) {
        return;
    }

    while (1) {
        pwm_pin_set_usec(pwm_dev, PWM_CHANNEL, 1500, 1000);
        k_sleep(K_MSEC(1000)); 
        pwm_pin_set_usec(pwm_dev, PWM_CHANNEL, 2000, 1000);
        k_sleep(K_MSEC(1000));
    }
}
