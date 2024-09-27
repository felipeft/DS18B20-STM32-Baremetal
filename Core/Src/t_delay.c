#include "stm32f103xb.h"
#include "t_delay.h"

//*************************** Funções de atraso utilizando Timer *************************************
/* Frequência do Timer (TF) = Clock do barramento (Fsysclk) / (Prescaler (TIM2PSC) + 1)
   - Clock do barramento = 32 MHz
   - TIM2PSC = 0, então TF = 32 MHz

   Período do Timer (TTP) = 1 / Frequência do Timer (TF) = 1 / 32 MHz = 0,03125 µs

   Configurando o Auto Reload Register (ARR) para 32, temos:
   - Atraso gerado pelo timer = Período do Timer (TTP) * Valor do ARR = 32 * 0,03125 µs = 1 µs
   Portanto:
   - TTP = 1 µs
   - TIM2ARR = 0xffff (máximo valor)
   - 1 µs = 1 contagem
   - 1 ms = 1000 µs
   - 1 s = 1000 ms
*/

// Função para inicializar o Timer 2
void timer_initialise()
{
    // Desliga o contador do Timer para garantir que ele não esteja ativo
    TIM2->CR1 &= ~(TIM_CR1_CEN);
    TIM2->SR &= ~(TIM_SR_UIF);

    // Reset do barramento do Timer 2
    RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
    RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);

    // Habilita o clock para o periférico Timer 2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Habilita a interrupção de atualização para evitar que os registros sejam atualizados
    TIM2->CR1 |= TIM_CR1_UDIS;

    // Define o valor do prescaler do Timer
    TIM2->PSC = 32;  // O prescaler divide a frequência de clock para ajustar o tempo do timer

    // Define o valor de auto reload (ARR) do Timer
    TIM2->ARR = 0xffff;  // Configura o valor máximo de ARR (65535)

    // Desabilita a interrupção de atualização para permitir que os valores sejam atualizados
    TIM2->CR1 &= ~TIM_CR1_UDIS;

    // Gera uma atualização para aplicar os valores dos registros de preload
    TIM2->EGR |= TIM_EGR_UG;

    // Limpa a flag de atualização (UIF) para garantir que não haja interrupções pendentes
    TIM2->SR &= ~(TIM_SR_UIF);
}

// Função para gerar um atraso em microssegundos
void delay_us(uint16_t us)
{
    TIM2->CR1 |= TIM_CR1_CEN;  // Habilita o contador do Timer
    TIM2->CNT = 0;  // Inicializa o contador do Timer com 0
    while (TIM2->CNT < us)  // Aguarda até que o valor do contador atinja o número de microssegundos desejado
    {
        // O loop é mantido até que o tempo desejado seja atingido
    }
}

// Função para gerar um atraso em milissegundos
void delay_ms(uint16_t ms)
{
    for (uint16_t i = 0; i < ms; i++)
    {
        delay_us(1000);  // Cada milissegundo é equivalente a 1000 microssegundos
    }
}

// Função para gerar um atraso em segundos
void delay_sec(uint16_t secs)
{
    for (uint16_t i = 0; i < secs; i++)
    {
        delay_ms(1000);  // Cada segundo é equivalente a 1000 milissegundos
    }
}

// Função para gerar um atraso indefinido com base na flag de atualização
void delay()
{
    TIM2->CR1 |= TIM_CR1_CEN;  // Habilita o contador do Timer
    while (!(TIM2->SR & TIM_SR_UIF));  // Aguarda até que a flag de atualização (UIF) seja setada
    TIM2->SR &= ~(TIM_SR_UIF);  // Limpa a flag de atualização
}

// Função alternativa para gerar um atraso em microssegundos
void delayus(uint16_t us)
{
    int i = 0;
    while (i++ < us)
    {
        TIM2->CR1 |= TIM_CR1_CEN;  // Habilita o contador do Timer
        while (!(TIM2->SR & TIM_SR_UIF));  // Aguarda a flag de atualização (UIF)
        TIM2->SR &= ~(TIM_SR_UIF);  // Limpa a flag de atualização
    }
}

// Função alternativa para gerar um atraso em milissegundos
void delayms(uint16_t ms)
{
    int i = 0;
    while (i++ < ms)
    {
        delayus(1000);  // Cada milissegundo é equivalente a 1000 microssegundos
    }
}
