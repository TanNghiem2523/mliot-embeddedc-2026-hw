/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Khai báo các hàm nguyên mẫu */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
    /* Khởi tạo thư viện HAL */
    HAL_Init();

    /* Cấu hình xung nhịp hệ thống (Sử dụng HSI mặc định hoặc tùy chỉnh theo CMake của bạn) */
    SystemClock_Config();

    /* Khởi tạo các chân GPIO */
    MX_GPIO_Init();

    uint8_t current_key = 0; // 0: Không có phím nào được nhấn

    /* Vòng lặp vô hạn */
    while (1)
    {
        current_key = 0; // Reset trạng thái mỗi lần quét

        // ==========================================
        // QUÉT HÀNG 1 (PA0)
        // ==========================================
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Kéo Hàng 1 xuống LOW
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);   // Giữ Hàng 2 ở mức HIGH
        
        HAL_Delay(5); // Delay chống nhiễu và chờ tín hiệu ổn định

        // Đọc Cột 1 (PA2)
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET) {
            current_key = 1; 
        }
        // Đọc Cột 2 (PA3)
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET) {
            current_key = 2; 
        }

        // ==========================================
        // QUÉT HÀNG 2 (PA1)
        // ==========================================
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // Giữ Hàng 1 ở mức HIGH
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // Kéo Hàng 2 xuống LOW
        
        HAL_Delay(5); 

        // Đọc Cột 1 (PA2)
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET) {
            current_key = 3; 
        }
        // Đọc Cột 2 (PA3)
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET) {
            current_key = 4; 
        }

        // ==========================================
        // XỬ LÝ LED PHẢN HỒI (PA4)
        // ==========================================
        if (current_key != 0) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // Bật LED
        } else {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // Tắt LED
        }
    }
}

/**
  * @brief Cấu hình khởi tạo GPIO thủ công
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Bật clock cho bến GPIOA */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Mặc định xuất mức HIGH cho 2 Hàng (PA0, PA1) trước khi vào vòng lặp */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_SET);
    
    /* Mặc định xuất mức LOW cho LED (PA4) để tắt đèn ban đầu */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    /* Cấu hình PA0, PA1 (Hàng) và PA4 (LED) là Digital Output Push-Pull */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Cấu hình PA2, PA3 (Cột) là Digital Input với trở treo Pull-up */
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief Cấu hình Clock Hệ Thống cơ bản 
  * (Bạn có thể thay thế bằng hàm clock riêng trong project của bạn nếu có dùng thạch anh ngoài)
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Sử dụng bộ dao động nội HSI (8MHz) cơ bản */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1); // Lỗi khởi tạo
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while(1); // Lỗi khởi tạo
    }
}