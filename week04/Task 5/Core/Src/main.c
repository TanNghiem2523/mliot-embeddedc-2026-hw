#include "stm32f1xx_hal.h" //Thư viện cung cấp các hàm cấu hình và điều khiển GPIO

/* Định nghĩa các chân Ma trận phím 2x2 */
#define ROW1_PORT   GPIOA
#define ROW1_PIN    GPIO_PIN_1
#define ROW2_PORT   GPIOA
#define ROW2_PIN    GPIO_PIN_2

#define COL1_PORT   GPIOA
#define COL1_PIN    GPIO_PIN_3
#define COL2_PORT   GPIOA
#define COL2_PIN    GPIO_PIN_4

#define LED_PORT    GPIOC
#define LED_PIN     GPIO_PIN_13

void SystemClock_Config(void);
void MX_GPIO_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    uint8_t key_pressed = 0;

    while (1) {
        key_pressed = 0;
        // Thuật toán: Quét từng ROW, đặt mức 0v ở từng ROW, kiểm tra có COL nào ở mức 0v không? ( được nhấn )
        // Quét Hàng 1
        HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_SET);
        
        HAL_Delay(1); 
        if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) {
            key_pressed = 1;
        } 
        else if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) {
            key_pressed = 2;
        }

        // Quét Hàng 2
        HAL_GPIO_WritePin(ROW1_PORT, ROW1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(ROW2_PORT, ROW2_PIN, GPIO_PIN_RESET);
        
        HAL_Delay(1); 
        if (HAL_GPIO_ReadPin(COL1_PORT, COL1_PIN) == GPIO_PIN_RESET) {
            key_pressed = 3;
        } 
        else if (HAL_GPIO_ReadPin(COL2_PORT, COL2_PIN) == GPIO_PIN_RESET) {
            key_pressed = 4;
        }

        if (key_pressed != 0) {
            // Lặp lại việc nháy LED đúng bằng số lần của phím được bấm
            for (uint8_t i = 0; i < key_pressed; i++) {
                HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET); // Bật LED (PC13 thường tích cực mức thấp)
                HAL_Delay(250);                                       // Chờ 250ms (LED sáng)
                
                HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);   // Tắt LED
                HAL_Delay(250);                                       // Chờ 250ms (LED tắt)
            }
            HAL_Delay(500); //chống dội buttons
        }
        
    }
}
void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();// đánh thức portA
    __HAL_RCC_GPIOC_CLK_ENABLE();// đánh thức portC

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET); 
    GPIO_InitStruct.Pin   = LED_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, ROW1_PIN | ROW2_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin   = ROW1_PIN | ROW2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = COL1_PIN | COL2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; 
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SysTick_Handler(void) {
    HAL_IncTick();
}


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

