#include "Arduino.h"
#include "common.h"
#include "csi_gpio_pin.h"
#include "csi_pin.h"
#include "csi_pwm.h"
#include "wiring_analog.h"
#include "ControlPWM.h"

int channel = -1;
static csi_pwm_t active_pwm_servo;
extern dev_pin_map_t pwm_map[];

void setPWM(uint8_t _pin, unsigned int _pulse, unsigned int _period)
{
    const dev_pin_map_t* pwm_pin = target_pin_number_to_dev(_pin, pwm_map, 0xFF);
    if (pwm_pin == NULL) {
        pr_err("pin GPIO %d is not used as PWM func\n", _pin);
        return;
    }

    uint8_t pwm_idx = pwm_pin->idx >> 2;
    uint8_t pwm_channel = pwm_pin->idx & 0x3;

    if (csi_pin_set_mux(pwm_pin->name, pwm_pin->func)) {
        pr_err("pin GPIO %d fails to config as PWM func\n", _pin);
        return;
    };

    csi_error_t ret_status = csi_pwm_init(&active_pwm_servo, pwm_idx);
    if (ret_status != CSI_OK) {
        pr_err("GPIO pin %d init failed\n", _pin);
        return;
    }
    channel = pwm_channel;

    csi_pwm_out_stop(&active_pwm_servo, pwm_channel);
    csi_pwm_out_config_continuous(&active_pwm_servo, pwm_channel, _period,
                                   0, PWM_POLARITY_HIGH);
    csi_pwm_out_start(&active_pwm_servo, pwm_channel);

    csi_pwm_out_stop(&active_pwm_servo, pwm_channel);
    csi_pwm_out_config_continuous(&active_pwm_servo, pwm_channel, _period,
                                   _pulse, PWM_POLARITY_HIGH);
    csi_pwm_out_start(&active_pwm_servo, pwm_channel);
}
