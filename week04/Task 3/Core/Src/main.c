#include "stm32f1xx.h"   // hoặc "stm32f1xx.h" tùy thư viện CMSIS đang dùng

#define SYSTEM_CORE_CLOCK 8000000 

// Hàm trễ chuẩn xác sử dụng SysTick Timer
void delay_ms(uint32_t ms) {
    // 1. Nạp giá trị đếm để tạo ra 1ms 
    // (Lấy tần số Clock chia 1000 sẽ ra số chu kỳ máy trong 1ms)
    SysTick->LOAD = (SYSTEM_CORE_CLOCK / 1000) - 1; 
    
    // 2. Xóa thanh ghi đếm hiện tại và cờ báo
    SysTick->VAL = 0; 
    
    // 3. Bật SysTick: Dùng clock của CPU (bit 2 = 1) và Kích hoạt bộ đếm (bit 0 = 1)
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; 

    // 4. Lặp đủ số millisecond yêu cầu
    for (uint32_t i = 0; i < ms; i++) {
        // Chờ cờ COUNTFLAG (bit 16) bật lên, báo hiệu đã đếm xong 1ms
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) {
            // Không làm gì, chỉ chờ
        }
    }
    
    // 5. Tắt SysTick sau khi hoàn thành
    SysTick->CTRL = 0; 
}

int main(void) {
    // 1. Bật APB2 Clock cho IOPC (Port C)
    RCC->APB2ENR |= (1 << 4);

    // 2. Cấu hình chân PC13 làm Digital Output Push-Pull
    GPIOC->CRH &= ~(0xF << 20); // Xóa cấu hình cũ
    GPIOC->CRH |= (0x2 << 20);  // Output mode, max speed 2 MHz, Push-Pull

    // 3. Vòng lặp điều khiển chính
    while (1) {
        // Đảo trạng thái chân PC13 (Toggle LED)
        GPIOC->ODR ^= (1 << 13);
        
        // Trễ chính xác 500ms bằng phần cứng
        delay_ms(500); 
    }
}