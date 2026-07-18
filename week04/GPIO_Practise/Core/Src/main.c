/**
 * TEST TINH - PC13 + phim SW65 (day "1" = Row1/PA0, day "A" = Col1/PA2)
 * Row1 giu LOW co dinh (khong toggle). Bam SW65 -> Col1 tut xuong 0 -> LED PC13 sang.
 */
#include "main.h"
#include "stm32f1xx_hal.h"

#define ROW1_PIN   GPIO_PIN_0
#define ROW1_PORT  GPIOA
#define COL1_PIN   GPIO_PIN_2
#define COL1_PORT  GPIOA
#define LED_PIN    GPIO_PIN_13
#define LED_PORT   GPIOC

void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};

    // Row1 (day "1"): output, giu LOW mai mai
    gpio.Pin   = ROW1_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ROW1_PORT, &gpio);
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);

    // Col1 (day "A"): input pull-up
    gpio.Pin  = COL1_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(COL1_PORT, &gpio);

    // LED PC13
    gpio.Pin   = LED_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &gpio);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);   // active-low -> tat luc dau

    // Bao hieu da chay toi day: nhay 2 lan
    for (int i = 0; i < 2; i++)
    {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        HAL_Delay(150);
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        HAL_Delay(150);
    }

    while (1)
    {
        if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);   // bam SW65 -> sang
        else
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);     // tha -> tat
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_PCLK1   | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file; (void)line;
    while (1) { }
}
#endif