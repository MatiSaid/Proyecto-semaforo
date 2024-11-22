/* Includes ------------------------------------------------------------------*/
#include "main.h"

typedef enum {
  LUZ_VERDE,
  LUZ_VERDE_INTERMITENTE,
  LUZ_ROJA,
  LUZ_ROJA_INTERMITENTE
} EstadosSemaforo;

EstadosSemaforo estadoSemaforo = LUZ_VERDE;
int botonActivado = 0;  // Indicador del botón

void ConfiguracionReloj(void);
static void InicializarGPIO(void);
void cambiarLuz(EstadosSemaforo nuevoEstado);
void parpadearLED(GPIO_TypeDef *puerto, uint16_t pin, int retardo, int repeticiones);
void procesarTransicion(void);

int main(void)
{
  HAL_Init();
  ConfiguracionReloj();
  InicializarGPIO();

  // Comienza en el estado LUZ_VERDE
  cambiarLuz(LUZ_VERDE);

  while (1)
  {
    // Detecta si el botón está activado
    if (HAL_GPIO_ReadPin(GPIOB, BOTON) == GPIO_PIN_SET && botonActivado == 0) {
      botonActivado = 1;  // Marca la pulsación
      procesarTransicion();  // Ejecuta la secuencia del semáforo
    }

    // Restablece la bandera cuando el botón se suelta
    if (HAL_GPIO_ReadPin(GPIOB, BOTON) == GPIO_PIN_RESET) {
      botonActivado = 0;
    }
  }
}

/* Procesa la transición de estados del semáforo */
void procesarTransicion(void) {
  cambiarLuz(LUZ_VERDE_INTERMITENTE);  // Cambia a LUZ_VERDE_INTERMITENTE

  cambiarLuz(LUZ_ROJA);                // Cambia a LUZ_ROJA
  HAL_Delay(3000);                     // Pausa 3 segundos

  cambiarLuz(LUZ_ROJA_INTERMITENTE);   // Cambia a LUZ_ROJA_INTERMITENTE

  cambiarLuz(LUZ_VERDE);               // Retorna al estado LUZ_VERDE
}

/* Cambia el estado de los LEDs según el semáforo */
void cambiarLuz(EstadosSemaforo nuevoEstado) {
  estadoSemaforo = nuevoEstado;

  switch (estadoSemaforo) {
    case LUZ_VERDE:
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_RESET);  // Activa LED verde
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);     // Desactiva LED rojo
      break;

    case LUZ_VERDE_INTERMITENTE:
      parpadearLED(GPIOB, LED_VERDE, 200, 5);               // Parpadea el verde
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);    // Apaga el verde
      break;

    case LUZ_ROJA:
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_RESET);   // Activa LED rojo
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);    // Desactiva LED verde
      break;

    case LUZ_ROJA_INTERMITENTE:
      parpadearLED(GPIOC, LED_ROJO, 200, 5);                // Parpadea el rojo
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);     // Apaga el rojo
      break;
  }
}

/* Función para parpadear un LED */
void parpadearLED(GPIO_TypeDef *puerto, uint16_t pin, int retardo, int repeticiones) {
  for (int i = 0; i < repeticiones; i++) {
    HAL_GPIO_TogglePin(puerto, pin);
    HAL_Delay(retardo);
  }
}

/* Configuración del reloj del sistema */
void ConfiguracionReloj(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/* Inicialización de GPIO */
static void InicializarGPIO(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);

  /* Configuración del botón */
  GPIO_InitStruct.Pin = BOTON;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configuración de los LEDs */
  GPIO_InitStruct.Pin = LED_VERDE;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_ROJO;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* Manejo de errores */
void Error_Handler(void) {
  while (1) {}
}
