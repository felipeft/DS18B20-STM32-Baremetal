#include "stm32f103xb.h"
#include "ds18b20.h"
#include "usart.h"
#include "lcd_i2c.h"
#include <stdbool.h>
#include <stdio.h>

#define Ds18b20_PinHigh         GPIOB->ODR |= GPIO_ODR_ODR0          // Seta este pino para High
#define Ds18b20_PinLow          GPIOB->ODR &= ~GPIO_ODR_ODR0         // Seta este pino para Low
#define LED_PIN                 GPIO_ODR_ODR15                       // Definição do pino do LED

uint8_t MAIN_SM = 0;
double Ds18b20_Temp;
char Txt[50] = {0};

volatile bool updateTemp = false;                                   // Flag para controle de leitura de temperatura

void Mcu_Config(void);                                              // Configura o microcontrolador
void GpioConfig(void);                                              // Configura as portas de GPIO
void SysTick_Handler(void);
void Delay_us(uint32_t Vezes);                                      // Espera um tempo em us
void Delay_ms(uint32_t Vezes);                                      // Espera um tempo em ms
void setupTimer(void);
void TIM2_IRQHandler(void);


// Função principal
int main()
{
    Mcu_Config();                                                   // Configura os periféricos do microcontrolador
    LCD_init();														//configura o LCD i2c
    setupTimer();
    lcd_clear();

    while(1){
		if (updateTemp) {
			updateTemp = false;
			Ds18b20_Temp = Ds18b20TempRead();                       // Lê a temperatura do DS18B20
			Ds18b20_Temp *= 0.0625;                                 // Faz um cálculo da conversão
			sprintf(Txt, "Temp: %.1fºC\r\n", Ds18b20_Temp);			// Converte a temperatura em string com 1 casa após a vírgula
			uart_write((uint8_t*)Txt);                                   // Escreve a string na serial

			// Converte a temperatura para exibição no LCD sem \r\n
			sprintf(Txt, "Temp: %.1f\xDF" "C", Ds18b20_Temp);           // \xDF é o código para o símbolo de grau em muitos conjuntos de caracteres do LCD

			lcd_setcursor(0, 0);
			LCD_string(Txt);
		}
	}
}


// Definição das funções
void Mcu_Config(void)
{
    GpioConfig();                                                   // Configura os GPIOs
    usart_init();	                                                // Configura a USART1


}

// Configura as portas de GPIO
void GpioConfig(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;        // Habilita o clock do GPIOB e GPIOC
    GPIOC->CRH &= ~(GPIO_CRH_CNF15_1 | GPIO_CRH_CNF15_0);           // Configura PC15 como saída push-pull
    GPIOC->CRH |= GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0;            // Configura para saída em 50Mhz
}

void SysTick_Handler(void)
{
	MAIN_SM = 1;
}

void setupTimer(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Habilita o clock do Timer 2
    TIM2->PSC = 8000 - 1;              // Prescaler para contar em milissegundos
    TIM2->ARR = 1000 - 1;              // Auto-reload para contar até 1000 ms (1 segundo)
    TIM2->DIER |= TIM_DIER_UIE;        // Habilita a interrupção de update
    TIM2->CR1 |= TIM_CR1_CEN;          // Habilita o contador do Timer 2
    NVIC_EnableIRQ(TIM2_IRQn);         // Habilita a interrupção do Timer 2 no NVIC
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        updateTemp = true;

        if (Ds18b20_Temp >= 25.0) {
            GPIOC->ODR |= LED_PIN;
        } else {
            GPIOC->ODR &= ~LED_PIN;
        }
    }
}




// Rotina de delay com base em us
void Delay_us(uint32_t Vezes)
{
	SysTick->LOAD = Vezes;											// Carrega o valor de ticks
	SysTick->VAL = 0;												// Zera o contador de systick
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;						// Habilita o systick

	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));			// Espera dar o tempo
	SysTick->CTRL = 0;												// Limpa o registrador inteiro
}

// Rotina de delay com base em ms
void Delay_ms(uint32_t Vezes)
{
	uint32_t Ticks = Vezes;

	Ticks *= 1000;													// Calcula o tempo com base em ms
	SysTick->LOAD = Ticks;											// Carrega o valor de ticks
	SysTick->VAL = 0;												// Zera o contador de systick
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;						// Habilita o systick

	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));			// Espera dar o tempo
	SysTick->CTRL = 0;												// Limpa o registrador inteiro
}


