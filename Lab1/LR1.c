/**
 * Copyright (C) 2025 Kravchuk Roman <darkkitsunezx128k2309@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT License.
 * Lab work 1 --
 */

#if defined (__TARGET_STM32F401CEU6__)
#include <stm32f4xx.h>
#elif defined (__TARGET_STM32F103C8T6__)
#include <stm32f1xx.h>
#endif
#include <shared_functions.h>

/*int main(void)
{
#if defined (__TARGET_STM32F401CEU6__)
    // Enable clock at GPIO port C
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk;
    //enable TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk;
    // Execute dsb instruction after enabling the peripheral clock, as per the errata
    //https://www.st.com/resource/en/errata_sheet/es0222-stm32f401xb-and-stm32f401xc-device-errata-stmicroelectronics.pdf
    _set_dsb();

    // Set pin 13 connected to blue led of port C as output
    //GPIOC->MODER |= (1 << GPIO_MODER_MODER13_Pos);

    GPIOC->MODER |= GPIO_MODER_MODER13_Pos;
#else
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    MODIFY_REG(GPIOC->CRH, 0 << 20, 3 << 20);    //Конфигурируем порт PC13 в режим push-pull с макс. частотой 50МГц

    RCC->APB2ENR |= 1<<14; // включаем тактирование UART1
	RCC->APB2ENR |= 1<<2; // включаем тактирование порта A
	GPIOA->CRH |= 0b1011<<4; // PA9 - выход 50МГц, push-pull,
	GPIOA->CRH &= 0b1011<<4; // альтернативная функция
	USART1->BRR = 7500; // 72000000 Гц / 9600 бод = 7500
	USART1->CR1 |= (1<<3) | (1<<13);//разблокировать UART1 и передачу по нему
	char string[] = "Hello World!\n"; // массив строки для передачи
	int counter = 0; // счетчик массива строки

#endif

    for(;;)
    {
        while ((USART1->SR & 0b10000000) == 0) {}//ждем опустошения USART1->DR
		USART1->DR = string[counter]; // отправить символ
		counter++;	// перейти на следующий символ
		if(string[counter]=='\0')counter = 0;//обнулить если конец строки
		for(long i=0;i<1000000;i++){
            GPIOC->BSRR |= 1 << 13;
        } // задержка
        GPIOC->BSRR |= 1 << 13;
    }

    return 0;
}*/

/*#define SYSCLOCK 72000000U
volatile uint32_t SysTick_CNT = 0;

void delay_ms(uint32_t ms)
{
  MODIFY_REG(SysTick->VAL,SysTick_VAL_CURRENT_Msk,SYSCLOCK / 1000 - 1);
  SysTick_CNT = ms;
  while(SysTick_CNT) {}
}

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    //MODIFY_REG(GPIOC->CRH, 0 << 20, 3 << 20);    //Конфигурируем порт PC13 в режим push-pull с макс. частотой 50МГц
    //GPIOC->CRH |= ;
    GPIOA->CRH |= GPIO_CRH_MODE13_Pos;
    GPIOA->CRH |= GPIO_CRH_CNF13_Pos;

	RCC->APB2ENR |= 1<<14; // включаем тактирование UART1
	RCC->APB2ENR |= 1<<2; // включаем тактирование порта A
	GPIOA->CRH |= 0b1011<<4; // PA9 - выход 50МГц, push-pull,
	GPIOA->CRH &= 0b1011<<4; // альтернативная функция
	USART1->BRR = 7500; // 72000000 Гц / 9600 бод = 7500
	USART1->CR1 |= (1<<3) | (1<<13);//разблокировать UART1 и передачу по нему
	char string[] = "Hello World!\n"; // массив строки для передачи
	int counter = 0; // счетчик массива строки
	while(1)
	{
		while ((USART1->SR & 0b10000000) == 0) {}//ждем опустошения USART1->DR
		USART1->DR = string[counter]; // отправить символ
		counter++;	// перейти на следующий символ
		if(string[counter]=='\0')counter = 0;//обнулить если конец строки
		GPIOC->BSRR |= 1 << 13;
		for(long i=0;i<1000000;i++){} // задержка
		GPIOC->BSRR |= 1 << 13;
	}
}*/
/**
 * STM32F103C8T6 UART TX with LED blink using CMSIS only
 * - Configures USART1 for 9600 baud transmission
 * - Sends text via UART1
 * - Blinks LED on PC13 after transmission
 */


/* Function prototypes */
/*void SystemClock_Config(void);
void UART1_Init(void);
void GPIO_Init(void);
void UART1_SendString(const char* str);
void LED_Toggle(void);
void Delay(uint32_t time);*/

/*int main(void)
{
    // Configure system clock
    SystemClock_Config();

    // Initialize GPIO
    GPIO_Init();

    // Initialize UART1
    UART1_Init();

    const char* message = "Hello from STM32F103 UART!\r\n";

    while(1)
    {
        // Send text via UART1
        UART1_SendString(message);

        // Toggle LED on PC13
        LED_Toggle();

        // Delay
        Delay(1000000);
    }
}*/

/**
 * @brief Configure system clock
 * - Set up 72MHz system clock from 8MHz HSI
 */
/*
void SystemClock_Config(void)
{
    // Enable HSI and wait until it's ready
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    // Configure PLL (8MHz HSI / 2 * 9 = 36MHz)
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;         // HSI oscillator clock / 2 selected as PLL input
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;        // Clear PLLMULL bits
    RCC->CFGR |= RCC_CFGR_PLLMULL9;        // PLL input clock * 9

    // Enable PLL and wait until it's ready
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // Configure flash latency
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_1;

    // Select PLL as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait until PLL is used as system clock source
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
*/

/**
 * @brief Initialize GPIO pins
 * - PC13 as output for LED
 * - PA9 as alternate function for UART1 TX
 */
/*
void GPIO_Init(void)
{
    // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Configure PC13 as output push-pull
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;       // Output mode, max speed 10 MHz

    // Set PC13 high (LED off for Blue Pill)
    GPIOC->BSRR = GPIO_BSRR_BS13;

    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA9 (USART1 TX) as alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;  //Output mode, max speed 50 MHz
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                     //Alternate function push-pull
}
*/

/**
 * @brief Initialize UART1
 * - 9600 baud rate
 * - 8 data bits
 * - 1 stop bit
 * - No parity
 * - No flow control
 */
/*void UART1_Init(void)
{
    // Enable USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Reset USART1 configuration
    USART1->CR1 = 0x00;
    USART1->CR2 = 0x00;
    USART1->CR3 = 0x00;

    // Configure baud rate: 36MHz/9600
    USART1->BRR = 0xEA6;     // 36000000 / 9600 = 3750 (0xEA6)

    // Enable transmitter
    USART1->CR1 |= USART_CR1_TE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;
}*/

/**
 * @brief Send a string via UART1
 * @param str: string to send
 */
/*void UART1_SendString(const char* str)
{
    while(*str)
    {
        // Wait until transmit data register is empty
        while(!(USART1->SR & USART_SR_TXE));

        // Send character
        USART1->DR = (*str & 0xFF);

        // Next character
        str++;
    }

    // Wait until transmission is complete
    while(!(USART1->SR & USART_SR_TC));
}*/

/**
 * @brief Toggle LED on PC13
 */
/*void LED_Toggle(void)
{
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}*/

/**
 * @brief Simple delay function
 * @param time: number of cycles to delay
 */
//void Delay(uint32_t time)
//{
//    for(uint32_t i = 0; i < time; i++)
//    {
//        __NOP();  /* No operation, prevents loop from being optimized away */
//    }
//}


/*#define LED_PIN 13
#define LED_PIN2 15

int main(void)
{
    #if defined (__TARGET_STM32F401CEU6__)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN_Msk; // enable clock at port C
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN_Msk; //enable TIM2
    _set_dsb(); // set dsb as errata solution
    GPIOC->MODER |= (1 << GPIO_MODER_MODER13_Pos);
    #endif
    return 0;
}*/

#if defined (__TARGET_STM32F103C8T6__)

/**
 * STM32F103C8T6 UART1 TX with LED blink using CMSIS only
 * - Configures UART1 for 9600 baud transmission
 * - Sends text via UART1
 * - Blinks LED on PC13 after transmission
 */

/* Function prototypes */
/*
void SystemClock_Config(void);
void UART1_Init(void);
void GPIO_Init(void);
void UART1_SendString(const char* str);
void LED_Toggle(void);
void Delay(uint32_t time);*/

/*int main(void)
{
    // Configure system clock
    SystemClock_Config();

    // Initialize GPIO
    GPIO_Init();

    // Initialize UART1
    UART1_Init();

    const char* message = "Hello from STM32F103 UART1!\n";

    while(1)
    {
        // Send text via UART1
        UART1_SendString(message);

        // Toggle LED on PC13
        LED_Toggle();

        // Delay
        Delay(1000000);
    }
}*/

/**
 * @brief Configure system clock
 * - Set up 72MHz system clock from 8MHz HSI
 */
/*
void SystemClock_Config(void)
{
    // Enable HSI and wait until it's ready
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    //Configure PLL (8MHz HSI / 2 * 9 = 36MHz)
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;         // HSI oscillator clock / 2 selected as PLL input
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;        // Clear PLLMULL bits
    RCC->CFGR |= RCC_CFGR_PLLMULL9;        // PLL input clock * 9

    // Enable PLL and wait until it's ready
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // Configure flash latency
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_1;

    // Select PLL as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait until PLL is used as system clock source
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}*/

/**
 * @brief Initialize GPIO pins
 * - PC13 as output for LED
 * - PA9 as alternate function for UART1 TX
 */
/*
void GPIO_Init(void)
{
    // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Configure PC13 as output push-pull
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;       // Output mode, max speed 10 MHz

    // Set PC13 high (LED off for Blue Pill)
    GPIOC->BSRR = GPIO_BSRR_BS13;

    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA9 (USART1 TX) as alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;  // Output mode, max speed 50 MHz
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                     // Alternate function push-pull
}*/

/**
 * @brief Initialize UART1
 * - 9600 baud rate
 * - 8 data bits
 * - 1 stop bit
 * - No parity
 * - No flow control
 */
/*void UART1_Init(void)
{
    // Enable USART1 clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Reset USART1 configuration
    USART1->CR1 = 0x00;
    USART1->CR2 = 0x00;
    USART1->CR3 = 0x00;

    // Configure baud rate: 36MHz/9600
    USART1->BRR = 0xEA6;     // 36000000 / 9600 = 3750 (0xEA6)

    // Enable transmitter
    USART1->CR1 |= USART_CR1_TE;

    // Enable USART1
    USART1->CR1 |= USART_CR1_UE;
}*/

/**
 * @brief Send a string via UART1
 * @param str: string to send
 */
/*void UART1_SendString(const char* str)
{
    while(*str)
    {
        // Wait until transmit data register is empty
        while(!(USART1->SR & USART_SR_TXE));

        // Send character
        USART1->DR = (*str & 0xFF);

        // Next character
        str++;
    }

    // Wait until transmission is complete
    while(!(USART1->SR & USART_SR_TC));
}*/

/**
 * @brief Toggle LED on PC13
 */
/*void LED_Toggle(void)
{
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}*/

/**
 * @brief Simple delay function
 * @param time: number of cycles to delay
 */
/*void Delay(uint32_t time)
{
    for(uint32_t i = 0; i < time; i++)
    {
        __NOP();  // No operation, prevents loop from being optimized away
    }
}*/

void _pc13_setup(void)
{
    // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // Configure PC13 as output push-pull
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_0;       // Output mode, max speed 10 MHz

    // Set PC13 high (LED off for Blue Pill)
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void _blink_led(void)
{
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void _gpio_init(void)
{

    _pc13_setup();

    // Enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA9 (USART1 TX) as alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;  // Output mode, max speed 50 MHz
    GPIOA->CRH |= GPIO_CRH_CNF9_1;                     // Alternate function push-pull
}

int main(void)
{
    // Configure system clock
    //SystemClock_Config();

    for(;;)
    {

    }
}

#endif
