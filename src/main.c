#include "stm32f10x.h"
#define USART1_Mantissa 39
#define USART1_Fraction 1
#define not_ready 0x100020 
#define ready 0x200010
#define LED_ALL_ON 255
#define LED_ALL_OFF 0


volatile uint8_t cnt = 0, tt = 255, flagu = 0, flage = 0;
volatile uint8_t data[2];

void send_led_status(){
    while(!(USART1 -> SR & (1 << 7)));
    USART1 -> DR = tt;
}

void send_error(){
    char error[] = "Du lieu khong hop le";
    for(uint8_t i = 0; i < sizeof(error) - 1; i++){
        while(!(USART1 -> SR & (1 << 7)));
        USART1 -> DR = error[i];
    }
}

void EXTI2_IRQHandler(){
    EXTI -> PR |= (1 << 2);
    flage = 1;
}

void USART1_IRQHandler(){
    if(USART1 -> SR & (1 << 5)){
        data[cnt++] = USART1 -> DR;
        if(cnt == 2){
            USART1 -> CR1 &= ~(1 << 5);
            flagu = 1;
        }
    }
}

void clear_dr(){
    uint32_t temp;
    if(USART1 -> SR & (1 << 3)){
        temp = USART1 -> SR;
        temp = USART1 -> DR;
    }
    else{
        temp = USART1 -> DR;
    }
}

void setup(){
    // Config HSE
        // RCC_CR reset value: 1000 0011
        RCC -> CR |= (1 << 16);
        while(!(RCC -> CR & (1 << 17)));
    // Config FLASH 
        // FLASH_ACR reset value: 0011 0000
        FLASH -> ACR |= (1 << 1);
    // Config BUS APB1
        RCC -> CFGR |= (1 << 10);
    // Config PLL
        RCC -> CFGR |= ((1 << 16) | (1 << 20) | (1 << 19) | (1 << 18));
        RCC -> CR |= (1 << 24);
        while(!(RCC -> CR & (1 << 25)));
        RCC -> CFGR |= (1 << 1);
        while(!(RCC -> CFGR & (1 << 3)));
    // Config APB2
        RCC -> APB2ENR |= ((1 << 14) | (1 << 3) | (1 << 2) | (1 << 0));
    // Config GPIOB
        GPIOB -> CRL &= ~((1 << 30) | (1 << 26) | (1 << 22));
        GPIOB -> CRL |= ((1 << 29) | (1 << 25) | (1 << 21));
        GPIOB -> CRH &= ~((1 << 2) | (1 << 6) | (1 << 22) | (1 << 26) | (1 << 30));
        GPIOB -> CRH |= ((1 << 1) | (1 << 5) | (1 << 21) | (1 << 25) | (1 << 29));
    // Config GPIOA
        GPIOA -> CRL &= ~((1 << 18) | (1 << 22));
        GPIOA -> CRL |= ((1 << 17) | (1 << 21));
        GPIOA -> CRH &= ~(1 << 6);
        GPIOA -> CRH |= ((1 << 4) | (1 << 5) | (1 << 7));
    // Config UART1
	    USART1 -> BRR = (USART1_Mantissa << 4) | USART1_Fraction;	
        USART1 -> CR1 |= ((1 << 13) | (1 << 5) | (1 << 3) | (1 << 2));
    // Config EXTI
        EXTI -> PR |= (1 << 2);
        EXTI -> IMR |= (1 << 2);
        EXTI -> FTSR |= (1 << 2);
    // Config Interrupt
		NVIC_ClearPendingIRQ(USART1_IRQn);
		NVIC_ClearPendingIRQ(EXTI2_IRQn);
		NVIC_EnableIRQ(USART1_IRQn);
		NVIC_EnableIRQ(EXTI2_IRQn);
}

int main(){
    setup();
    GPIOB -> BSRR = 0xE3E0;
    GPIOA -> BSRR = ready;
    uint8_t led_pin[8] = {5, 6, 7, 8, 9, 13, 14, 15};
    while(1){
        if(flagu == 1){
            flagu = 0;
            GPIOA -> BSRR = not_ready;
            if(data[0] >= '1' && data[0] <= '8'){
                if(data[1] == 'i') tt |= (1 << (data[0] - 49));
                else if(data[1] == 'o') tt &= ~(1 << (data[0] - 49));
                else send_error();
            }
            else if(data[0] == 'a'){
                if(data[1] == 'i') tt = LED_ALL_ON;
                else if(data[1] == 'o') tt = LED_ALL_OFF;
                else send_error();
            }
            else send_error();
            send_led_status();
            uint32_t bsrr = 0;
            for(uint8_t i = 0; i < 8; i++){
                if(tt & (1 << i)){
                    bsrr |= (1 << led_pin[i]);
                }
                else{
                    bsrr |= (1 << (led_pin[i] + 16));
                }
            }
            GPIOB -> BSRR = bsrr;
            clear_dr();
            cnt = 0;
            GPIOA -> BSRR = ready;
            USART1 -> CR1 |= (1 << 5);
        }
        if(flage == 1){
            flage = 0;
            send_led_status();
        }
    }
}
