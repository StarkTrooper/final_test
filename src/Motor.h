// Configuración del PWM 

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>

// Velocidades del motor
#define SPEED_STOP  0
#define SPEED_LOW   1000
#define SPEED_HIGH  2000

// Duraciones de operación del motor en milisegundos
#define DURATION_STOP    0
#define DURATION_SHORT   1000
#define DURATION_LONG    2000

// Constantes de ejemplo, ajusta según tus necesidades
#define PWM_PERIOD_US 20000
#define PWM_CHANNEL 0

// Declaración de la estructura pwm_config
struct pwm_config {
    uint32_t frequency;
    uint32_t period;
    uint32_t pulse_width;
    uint8_t flags;
};

// Variable global para almacenar la velocidad actual
uint32_t current_speed = SPEED_STOP;
uint32_t current_duration = DURATION_STOP;

// Declaración de la función para evitar el error
void pwm_change_fn(struct k_timer *dummy);

// Timers
K_TIMER_DEFINE(pwm_change_timer, pwm_change_fn, NULL);

// Función para cambiar la velocidad y duración del motor
void change_motor_speed_and_duration(int val)
{
    switch (val)
    {
    case 1:
        //current_speed = SPEED_LOW;
        current_duration = DURATION_SHORT;
        break;
    case 2:
        //current_speed = SPEED_HIGH;
        current_duration = DURATION_LONG;
        break;
    case 0:
        //current_speed = SPEED_STOP;
        current_duration = DURATION_STOP;
        break;
    default:
        //current_speed = SPEED_STOP;
        current_duration = DURATION_STOP;
        break;
    }
}

// Función para cambiar el valor del PWM
void pwm_change_fn(struct k_timer *dummy)
{
    int val = 0;
    int err = pwm_pin_set_usec(pwm_dev, PWM_CHANNEL, PWM_PERIOD_US, current_speed, PWM_FLAGS);
    if (err < 0) {
        printk("Error al cambiar el valor del PWM. Código de error: %i\n", err);
    }

    // Cambiar la velocidad y duración del motor en cada intervalo de temporizador
    change_motor_speed_and_duration(val);
}

// Función para configurar y aplicar la configuración inicial del PWM
void setup_pwm(const struct device *pwm_dev)
{
    struct pwm_config pwm_cfg = {
        .frequency = 1000,
        .period = PWM_PERIOD_US,
        .pulse_width = 0,
        .flags = 0,
    };

    int ret = pwm_pin_set_config(pwm_dev, PWM_CHANNEL, &pwm_cfg);
    if (ret < 0) {
        printk("Error al configurar el PWM. Código de error: %i\n", ret);
        return;
    }
}

void Motor(int val)
{
    const struct device *pwm_dev;

    change_motor_speed_and_duration(val)

    pwm_dev = device_get_binding(PWM_CTRL);
    if (!pwm_dev) {
        printk("Error al obtener el controlador PWM\n");
        return;
    }

    // Configurar y aplicar la configuración inicial del PWM
    setup_pwm(pwm_dev);

    // Iniciar el temporizador de cambio de PWM
    k_timer_start(&pwm_change_timer, K_MSEC(200), K_MSEC(200));

    while (1) {
        k_sleep(K_MSEC(current_duration));
    }
}
