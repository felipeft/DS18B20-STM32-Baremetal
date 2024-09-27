#include <i2c.h>
#include "stm32f103xb.h"

// PB6-SCL, PB7-SDA
void i2c2_pin_config()
{
    // Habilitar o clock para o GPIOB e I2C2
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;  // Habilita o clock para o GPIOB
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;  // Habilita o clock para o I2C2

    // Configurações para PB6 (SCL) como Função Alternativa Open Drain
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);  // Limpa as configurações atuais do pino PB6
    GPIOB->CRL |= GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1;  // Configura o pino como modo de saída a 50 MHz
    GPIOB->CRL |= GPIO_CRL_CNF6_1;  // Define como Função Alternativa Open Drain

    // Configurações para PB7 (SDA) como Função Alternativa Open Drain
    GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);  // Limpa as configurações atuais do pino PB7
    GPIOB->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;  // Configura o pino como modo de saída a 50 MHz
    GPIOB->CRL |= GPIO_CRL_CNF7_1;  // Define como Função Alternativa Open Drain
}

uint32_t read_reg = 0;

void i2c_master_init()
{
    i2c2_pin_config();  // Configura os pinos PB6 e PB7 para I2C

    // Configura o registro CR2 do I2C para o clock de entrada do periférico: FREQ[] bits
    I2C2->CR2 = 0x20;  // Configura a frequência do clock com base na frequência do core, que é 32MHz (0x20 = 32)

    // Configura o registro CCR do I2C para o controle do clock
    I2C2->CCR = 0x140;

    // Configura o registro TRISE do I2C
    I2C2->TRISE = 0x21;

    // Habilita o periférico no registro CR1
    I2C2->CR1 |= I2C_CR1_PE;

    // Verifica se o barramento I2C está ocupado (busy)
    if ((I2C2->SR2 & I2C_SR2_BUSY) == 1)
        i2c_master_stop_generation();  // Gera um sinal de parada se estiver ocupado
}

// Gera a condição de start para iniciar a comunicação no modo master
void i2c_start_condition_w()  // Evento EV5
{
    I2C2->CR1 |= I2C_CR1_START;  // Gera a condição de start
    while (!(I2C2->SR1 & I2C_SR1_SB));  // Aguarda até que o start bit seja transmitido e o modo master seja ativado
    read_reg = I2C2->SR1;  // Lê o registro SR1 para limpar o flag
}

void i2c_start_condition_r()  // Evento EV5
{
    I2C2->CR1 |= I2C_CR1_ACK;  // Habilita ACK para leitura
    I2C2->CR1 |= I2C_CR1_START;  // Gera a condição de start
    while (!(I2C2->SR1 & I2C_SR1_SB));  // Aguarda a transição para o modo master
    read_reg = I2C2->SR1;  // Lê o registro SR1 para limpar o flag
}

// Envia o endereço do escravo no modo de escrita
void i2c_address_send_w(uint8_t slave_address)  // Evento EV6
{
    I2C2->DR = slave_address;  // Escreve o endereço do escravo no barramento I2C
    while (!(I2C2->SR1 & I2C_SR1_TXE));  // Aguarda o envio do dado
    while (!(I2C2->SR1 & I2C_SR1_ADDR));  // Aguarda a transmissão do endereço do escravo (ADDR setado com sucesso)

    // Limpa o bit ADDR
    read_reg = I2C2->SR1;
    read_reg = I2C2->SR2;
}

// Envia um byte de dado no modo master
void i2c_master_send_byte(uint8_t data)  // Evento EV8
{
    I2C2->DR = data;  // Escreve o dado no registro de dados
    while (!(I2C2->SR1 & I2C_SR1_TXE));  // Aguarda até que o registro de dados esteja vazio após a transmissão
}

// Envia o endereço do escravo no modo de leitura
int i2c_address_send_r(uint8_t slave_address)  // Evento EV6
{
    I2C2->DR = slave_address;  // Escreve o endereço do escravo no barramento I2C
    while (!(I2C2->SR1 & I2C_SR1_ADDR))  // Aguarda a transmissão do endereço
    {
        if ((I2C2->SR1 & I2C_SR1_AF) == 1)
        {
            printf("Erro ao enviar o endereço de leitura do escravo\n");
            return 0;  // Retorna erro se o bit AF (Acknowledge Failure) for setado
        }
    }

    // Limpa o bit ADDR
    read_reg = I2C2->SR1;
    read_reg = I2C2->SR2;
}

// Recebe mais de 3 bytes
void datareive_string(char data[])
{
    I2C2->CR1 |= I2C_CR1_ACK;  // Habilita ACK
    while (!(I2C2->SR1 & I2C_SR1_BTF));  // Aguarda a conclusão da transmissão
    I2C2->CR1 &= ~I2C_CR1_ACK;  // Desabilita ACK
}

// Recebe 2 bytes
void i2c_master_receive_2_bytes(uint8_t data1, uint8_t data2)
{
    I2C2->CR1 |= I2C_CR1_POS;  // Configura o bit POS
    I2C2->CR1 &= ~I2C_CR1_ACK;  // Desabilita ACK
    while (!(I2C2->SR1 & I2C_SR1_BTF));  // Aguarda a transferência do buffer de dados
    I2C2->CR1 |= I2C_CR1_STOP;  // Gera um stop
    data1 = I2C2->DR;  // Lê o primeiro byte
    data2 = I2C2->DR;  // Lê o segundo byte
}

// Recebe 1 byte
uint8_t i2c_master_receive_byte()
{
    uint8_t data;
    I2C2->CR1 &= ~I2C_CR1_ACK;  // Desabilita ACK
    I2C2->CR1 |= I2C_CR1_STOP;  // Gera um stop
    while (!(I2C2->SR1 & I2C_SR1_RXNE));  // Aguarda o dado no registro de recepção
    data = I2C2->DR;  // Lê o dado recebido
    return data;
}

// Gera uma condição de parada
void i2c_master_stop_generation()
{
    I2C2->CR1 |= I2C_CR1_STOP;  // Gera o sinal de stop
}
