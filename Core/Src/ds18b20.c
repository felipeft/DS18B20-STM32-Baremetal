#include "ds18b20.h"
#include "stm32f103xb.h"
#include <stdbool.h>
#include <stdint.h>

// Inicializa o sensor DS18B20
bool Ds18b20_Init(void)
{
    uint16_t t = 0;
    Ds18b20_PinOut();          // Configura o pino como saída
    Ds18b20_PinHigh;           // Coloca o pino em nível alto
    Delay_us(5);               // Espera 5 microsegundos
    Ds18b20_PinLow;            // Coloca o pino em nível baixo para resetar o DS18B20
    Delay_us(750);             // Mantém o pino em nível baixo por 750 microsegundos
    Ds18b20_PinHigh;           // Coloca o pino novamente em nível alto
    Ds18b20_PinIn();           // Configura o pino como entrada para esperar a resposta do sensor

    // Aguarda o pulso de presença do DS18B20
    while(GPIOB->IDR & GPIO_IDR_IDR0)
    {
        t++;                    // Incrementa o contador
        if(t > 60) return false; // Se ultrapassar 60 ciclos, retorna erro
        Delay_us(1);            // Aguarda 1 microsegundo entre as leituras
    }
    t = 480 - t;                // Calcula o tempo restante
    Ds18b20_PinOut();           // Configura o pino como saída
    Delay_us(t);                // Espera o tempo restante
    Ds18b20_PinHigh;            // Coloca o pino em nível alto
    return true;                // Retorna sucesso
}

// Faz a escrita de um byte para o sensor DS18B20
void Ds18b20Write(uint8_t Data)
{
    Ds18b20_PinOut();           // Configura o pino como saída

    for(uint8_t i = 0; i < 8; i++) // Escreve cada bit, um por vez
    {
        Ds18b20_PinLow;         // Coloca o pino em nível baixo
        Delay_us(10);           // Aguarda 10 microsegundos

        if(Data & 0x01)         // Verifica o bit menos significativo
            Ds18b20_PinHigh;    // Se for 1, coloca o pino em nível alto
        else
            Ds18b20_PinLow;     // Se for 0, mantém em nível baixo

        Data >>= 1;             // Desloca o dado para a direita
        Delay_us(50);           // Aguarda 50 microsegundos
        Ds18b20_PinHigh;        // Coloca o pino em nível alto
    }
}

// Faz a leitura de um byte do sensor DS18B20
uint8_t Ds18b20Read(void)
{
    uint8_t Data = 0;

    Ds18b20_PinOut();           // Configura o pino como saída
    Ds18b20_PinHigh;            // Coloca o pino em nível alto
    Delay_us(2);                // Aguarda 2 microsegundos

    for(uint8_t i = 0; i < 8; i++) // Lê cada bit, um por vez
    {
        Ds18b20_PinLow;         // Coloca o pino em nível baixo
        Delay_us(1);            // Aguarda 1 microsegundo
        Ds18b20_PinHigh;        // Coloca o pino em nível alto
        Ds18b20_PinIn();        // Configura o pino como entrada
        Delay_us(5);            // Aguarda 5 microsegundos
        Data >>= 1;             // Desloca o dado para a direita

        if(GPIOB->IDR & GPIO_IDR_IDR0)  // Se o pino estiver em nível alto, escreve 1 no bit mais significativo
            Data |= 0x80;       // Insere o bit 1 no dado

        Delay_us(55);           // Aguarda 55 microsegundos
        Ds18b20_PinOut();       // Configura o pino como saída
        Ds18b20_PinHigh;        // Coloca o pino em nível alto
    }
    return Data;                // Retorna o byte lido
}

// Define o pino do sensor DS18B20 como saída
void Ds18b20_PinOut(void)
{
    GPIOB->CRL |= GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0;   // Configura o pino como saída de alta velocidade
    GPIOB->CRL &= ~(GPIO_CRL_CNF0_1 | GPIO_CRL_CNF0_0);  // Configura o modo push-pull
}

// Define o pino do sensor DS18B20 como entrada
void Ds18b20_PinIn(void)
{
    GPIOB->CRL &= ~(GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0);  // Desabilita o modo de saída
    GPIOB->CRL |= GPIO_CRL_CNF0_0;                        // Configura o pino como entrada com flutuação
}

// Faz a leitura da temperatura
uint16_t Ds18b20TempRead(void)
{
    uint16_t Temp = 0;

    if(!Ds18b20_Init())          // Inicializa o sensor, se falhar retorna 0
        return 0;

    Ds18b20Write(0xCC);          // Envia o comando Skip ROM (0xCC)
    Ds18b20Write(0x44);          // Inicia a conversão de temperatura (0x44)

    if(!Ds18b20_Init())          // Inicializa o sensor novamente, se falhar retorna 0
        return 0;

    Ds18b20Write(0xCC);          // Envia o comando Skip ROM (0xCC)
    Ds18b20Write(0xBE);          // Envia o comando Read Scratchpad (0xBE)

    Temp = Ds18b20Read();        // Lê o byte menos significativo da temperatura
    Temp |= Ds18b20Read() << 8;  // Lê o byte mais significativo e o desloca

    return Temp;                 // Retorna a temperatura lida (valor bruto)
}
