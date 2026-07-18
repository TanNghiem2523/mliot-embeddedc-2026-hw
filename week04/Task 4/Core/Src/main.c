#include "stm32f1xx.h"

#define SYSTEM_CORE_CLOCK 8000000 

// Hàm trễ chuẩn xác sử dụng SysTick Timer (Đã viết ở Task trước)
void delay_ms(uint32_t ms) {
    SysTick->LOAD = (SYSTEM_CORE_CLOCK / 1000) - 1; 
    SysTick->VAL = 0; 
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; 
    for (uint32_t i = 0; i < ms; i++) {
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    }
    SysTick->CTRL = 0; 
}

int main(void) {
    // 1. BẬT CLOCK CHO PORT A VÀ PORT C
    // Bit 2: IOPAEN (Port A), Bit 4: IOPCEN (Port C)
    RCC->APB2ENR |= (1 << 2) | (1 << 4);

    // 2. CẤU HÌNH GPIO CHO LED (PC13 - Digital Output Push-Pull)
    GPIOC->CRH &= ~(0xF << 20); // Xóa cấu hình cũ của PC13
    GPIOC->CRH |= (0x2 << 20);  // Output mode 2MHz, Push-Pull

    // 3. CẤU HÌNH GPIO CHO NÚT NHẤN (PA0 - Input Pull-up)
    // Chân PA0 thuộc nửa thấp của Port A nên dùng thanh ghi CRL (Bit 0 đến 3)
    GPIOA->CRL &= ~(0xF << 0);  // Xóa cấu hình cũ của PA0
    
    // Cấu hình Input mode (MODE=00) và Input Pull-up/Pull-down (CNF=10) => Nhị phân: 1000 (0x8)
    GPIOA->CRL |= (0x8 << 0);
    
    // ĐỂ XÁC ĐỊNH LÀ PULL-UP (kéo lên VCC): Phải ghi mức 1 vào thanh ghi ODR tương ứng
    // (Nếu ghi mức 0 thì sẽ là Pull-down)
    GPIOA->ODR |= (1 << 0);

    // Tắt LED ban đầu (PC13 cấu hình Active-Low trên Bluepill, ODR = 1 là tắt)
    GPIOC->ODR |= (1 << 13);

    // 4. VÒNG LẶP ĐIỀU KHIỂN CHÍNH
    while (1) {
        // Kiểm tra xem nút có được nhấn không? 
        // Đọc thanh ghi Input Data Register (IDR) tại bit 0. Nhấn = LOW (0).
        if ((GPIOA->IDR & (1 << 0)) == 0) {
            
            // a. CHỐNG DỘI PHÍM (Debounce): Chờ 20ms để triệt tiêu nhiễu cơ khí
            delay_ms(20); 
            
            // Kiểm tra lại lần 2 để chắc chắn nút thực sự được nhấn chứ không phải nhiễu
            if ((GPIOA->IDR & (1 << 0)) == 0) {
                
                // b. CHỐNG GIỮ PHÍM: Chờ cho đến khi người dùng nhả nút ra
                // Vòng lặp while này sẽ giữ chương trình đứng lại chừng nào PA0 vẫn bằng 0
                while ((GPIOA->IDR & (1 << 0)) == 0) {
                    // Trống (Chỉ đợi)
                }
                
                // Tùy chọn: Chống dội phím lúc nhả nút (cơ khí lúc nảy lên cũng tạo nhiễu)
                delay_ms(20); 
                
                // c. ĐẢO TRẠNG THÁI LED SAU KHI ĐÃ NHẤN VÀ NHẢ THÀNH CÔNG
                GPIOC->ODR ^= (1 << 13);
            }
        }
    }
}