#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void Error_Handler(void);

#define LED_ROJO GPIO_PIN_13
#define LED_ROJO_GPIO_Port GPIOC
#define LED_VERDE GPIO_PIN_12
#define LED_VERDE_GPIO_Port GPIOB
#define BOTON GPIO_PIN_5
#define BOTON_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
