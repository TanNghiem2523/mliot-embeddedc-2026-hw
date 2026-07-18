/**
 * FIRMWARE TEST TĨNH - chỉ để đo bằng đồng hồ vạn năng / debug LED
 * Row1 (PA0) được giữ LOW CỐ ĐỊNH (không toggle), rồi liên tục đọc Col1 (PA2).
 * - Không bấm  -> Col1 phải đo được ~3.3V, LED tắt
 * - Bấm Row1+Col1 -> Col1 phải tụt xuống ~0V, LED sáng NGAY LẬP TỨC
 *
 * Nếu bấm mà điện áp Col1 vẫn không đổi -> lỗi thật sự nằm ở dây/nút bấm.
 */
#include "main.h"
#include "stm32f1xx_hal.h"

#define ROW1_PIN   GPIO_PIN_0
#define ROW1_PORT  GPIOA
#define COL1_PIN   GPIO_PIN_2
#define COL1_PORT  GPIOA
#define LED_PIN    GPIO_PIN_4
#define LED_PORT   GPIOA

void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};

    // Row1: output, kéo xuống LOW và giữ nguyên
    gpio.Pin   = ROW1_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ROW1_PORT, &gpio);
    HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);   // giữ LOW mãi mãi

    // Col1: input pull-up
    gpio.Pin  = COL1_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(COL1_PORT, &gpio);

    // LED: output
    gpio.Pin   = LED_PIN;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &gpio);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);

    while (1)
    {
        // Đọc liên tục, không delay -> phản ứng LED gần như tức thời
        if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET)
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);    // bấm -> sáng
        else
            HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);  // thả -> tắt
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
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;S
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