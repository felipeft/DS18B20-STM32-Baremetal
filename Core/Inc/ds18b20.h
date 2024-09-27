#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f103xb.h"
#include <stdbool.h>
#include <stdint.h>

#define Ds18b20_PinHigh     GPIOB->ODR |= GPIO_ODR_ODR0    // Define o pino como High
#define Ds18b20_PinLow      GPIOB->ODR &= ~GPIO_ODR_ODR0   // Define o pino como Low

// Funções públicas do DS18B20
bool Ds18b20_Init(void);                // Inicializa o sensor DS18B20
void Ds18b20Write(uint8_t Data);        // Faz a escrita do sensor DS18B20
uint8_t Ds18b20Read(void);              // Faz a leitura do sensor DS18B20
void Ds18b20_PinOut(void);              // Define o pino do sensor como saída
void Ds18b20_PinIn(void);               // Define o pino do sensor como entrada
uint16_t Ds18b20TempRead(void);         // Faz a leitura da temperatura

#endif // DS18B20_H
