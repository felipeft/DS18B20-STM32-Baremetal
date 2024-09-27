#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <i2c.h>
#include <t_delay.h>
#include "stm32f103xb.h"

// Definições para o conjunto de funções do LCD
// Rs RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
// 0  0  0   0   1   DL  N   F   -   -
// DL (8bit ou 4bit), N (1 linha ou 2 linhas)
#define lcd_8bit_1line 0b00110000  // Modo 8 bits, 1 linha
#define lcd_8bit_2line 0b00111000  // Modo 8 bits, 2 linhas
#define lcd_4bit_1line 0b00100000  // Modo 4 bits, 1 linha
#define lcd_4bit_2line 0b00101000  // Modo 4 bits, 2 linhas

// Controle de Display ON/OFF
// Rs RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
// 0  0  0   0   0   0   1   D   C   B
// D: Display ligado (1) ou desligado (0), C: Cursor ligado (1) ou desligado (0), B: Blink ligado (1) ou desligado (0)
#define lcd_DN_CN_BN 0b00001111  // Display ON, Cursor ON, Blink ON
#define lcd_DN_CF_BF 0b00001100  // Display ON, Cursor OFF, Blink OFF
#define lcd_DN_CN_BF 0b00001110  // Display ON, Cursor ON, Blink OFF
#define lcd_DF_CF_BF 0b00001000  // Display OFF, Cursor OFF, Blink OFF
#define lcd_DF_CN_BN 0b00001011  // Display OFF, Cursor ON, Blink ON

// Definições de modo de entrada
// Rs RW DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
// 0  0  0   0   0   0   0   1  I/D  S/H
// I/D: Move o cursor, S/H: Move o display
#define lcd_CMR_DMF 0b00000110  // Cursor move para a direita, sem mover o display
#define lcd_CML_DMF 0b00000100  // Cursor move para a esquerda, sem mover o display
#define lcd_CMR_DMR 0b00000111  // Cursor move para a direita, display move para a direita
#define lcd_CML_DML 0b00000101  // Cursor move para a esquerda, display move para a esquerda

// Comandos para limpar o display e retornar o cursor para o início
#define lcd_clear_all 0b00000001  // Limpa o display
#define lcd_rhome     0b00000010  // Retorna o cursor ao início
#define lcd_SETDDRAMADDR 0x80     // Endereço para definir DDRAM

// Endereços de leitura e escrita do PCF8574 (expansor de I/O para I2C)
#define pcf8574_WADDR 0b01111110  // Endereço de escrita no PCF8574
#define pcf8574_RADDR 0b01111111  // Endereço de leitura no PCF8574
#define LCD_BACKLIGHT         0x08  // Backlight ligado
#define LCD_NOBACKLIGHT       0x00  // Backlight desligado
#define display_rate 50             // Taxa de atualização do display

// Definições para caracteres personalizados no LCD
#define lcd_SETDDRAMADDR 0x80  // Endereço da DDRAM (para dados do display)
#define lcd_SETCGRAMADDR 0x40  // Endereço da CGRAM (para caracteres personalizados)

/*
 Pinos de controle do display:
 P7-D7, P6-D6, P5-D5, P4-D4
 P3 - sem uso
 P2 - Enable (E)
 P1 - RW (Read/Write)
 P0 - RS (Register Select)

 D7-D4 são os pinos de dados
 E: pino Enable (deve ser alternado entre Alto e Baixo após o envio de dados)
 RW: pino de leitura(1)/escrita(0)
 RS: pino de controle de comando(0)/dados(1)
*/

// Função para enviar dados ao expansor I/O via I2C
void lcd_ioexander(unsigned char Data);

// Gera um pulso no pino Enable para o envio de dados
void pulse_enable(uint8_t data);

// Escreve 4 bits no LCD (modo 4 bits)
void lcd_write4bit(unsigned char nibble);

// Envia um comando para o LCD (RS=0, RW=0)
void LCD_command(unsigned char command);

// Define a posição do cursor no display
void lcd_setcursor(uint8_t col, uint8_t row);

// Limpa o display
void lcd_clear();

// Retorna o cursor para o início
void lcd_Rethome();

// Envia um dado para o display (RS=1, RW=0)
void LCD_data(unsigned char data);

// Função para imprimir inteiros com formato fixo (ex: 001, 023, 015)
void lcd_printint(int value);

// Função para imprimir inteiros de forma simples (ex: 1, 23, 6)
void lcd_printint_num(int value);

// Função para enviar uma string ao display
void LCD_string(const char* str);

// Funções para imprimir uma linha no display
void lcd_print1line(const char *buff);
void lcd_print2line(const char *buff);

// Função para trabalhar com bits do LCD
void lcd_bit(unsigned char val);

// Inicializa o LCD
void LCD_init();

// Funções para trabalhar com caracteres personalizados no LCD
void setramaddr(uint8_t address);  // Define o endereço da RAM para caracteres
void LCD_sendpattern(uint8_t loc, uint8_t pattern[]);  // Envia um padrão de caractere personalizado para o LCD
void LCD_printpattern(uint8_t loc);  // Imprime um caractere personalizado no display

#endif /* LCD_I2C_H_ */
