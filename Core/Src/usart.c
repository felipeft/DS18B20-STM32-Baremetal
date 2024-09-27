#include "usart.h"
#include "stm32f103xb.h"

#define CPU_CLK 	8000000
#define BaudRate	115200

/*
 * Initialize UART (USART1) peripheral
 */
void usart_init(void)
{
	/* UART1 Pin configuration */
	// Enable clock access to GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	/* Configure PA9(TX) as output with maximum speed of 50MHz
	 * and alternate output push-pull mode for USART1 */
	GPIOA->CRH &= 0xFFFFFF0F;
	GPIOA->CRH |= 0x000000B0;

	/* GPIO PB8 configuration for DEBUG */
	// Enable clock access to GPIOB
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000002;

	GPIOB->ODR &= 0x00000000;

	/* USART1 configuration */
	// Enable clock access to USART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Transmit Enable
	USART1->CR1 |= USART_CR1_TE;

	// Set Baud Rate
	USART1->BRR = ((CPU_CLK + (BaudRate / 2U)) / BaudRate);

	// Enable UART
	USART1->CR1 |= USART_CR1_UE;
}

/*
 * UART Write Function
 * Transmit a string of characters via UART
 */
void uart_write(uint8_t *ch)
{
	GPIOB->ODR |= 0x00000100; // Debugging signal (set PB8 high)

	while (*ch)
	{
		// Wait until transmit data register is empty
		while (!(USART1->SR & USART_SR_TXE)) {}

		// Write data to transmit data register
		USART1->DR = (*ch & 0xFF);
		ch++;
	}

	GPIOB->ODR &= ~0x00000100; // Debugging signal (set PB8 low)
}
