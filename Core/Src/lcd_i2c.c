#include <lcd_i2c.h>
#include "stm32f103xb.h"

// Definições para os pinos de controle do LCD via I2C
uint8_t LCD_En = 0x04;  // Enable bit (00000100)
uint8_t LCD_Rw = 0x00;  // Read/Write bit (00000000) - sempre no modo de escrita
uint8_t LCD_Rs = 0x01;  // Register Select bit (00000001)

// Função para enviar dados ao expansor de I/O via I2C (PCF8574)
void lcd_ioexander(unsigned char Data)
{
    i2c_start_condition_w();  // Gera uma condição de start
    i2c_address_send_w(pcf8574_WADDR);  // Envia o endereço do expansor de I/O (escrita)
    i2c_master_send_byte(Data | LCD_BACKLIGHT);  // Envia os dados com o backlight ligado
    i2c_master_stop_generation();  // Gera uma condição de stop
}

// Função para gerar o pulso de enable do LCD
void pulse_enable(uint8_t data)
{
    lcd_ioexander(data | LCD_En);  // Seta o bit de enable (pulso alto)
    delay_us(1);  // Pequeno atraso
    lcd_ioexander(data & ~LCD_En);  // Limpa o bit de enable (pulso baixo)
    delay_us(50);  // Atraso para estabilizar
}

// Função para escrever 4 bits no LCD (modo 4 bits)
void lcd_write4bit(unsigned char nibble)
{
    nibble |= (LCD_Rs | LCD_Rw);  // Adiciona os bits de RS e RW
    lcd_ioexander(nibble | 0x04);  // Envia os dados com enable alto
    lcd_ioexander(nibble & 0xFB);  // Envia os dados com enable baixo
    delay_ms(display_rate);  // Atraso para estabilização
}

// Função para enviar um comando ao LCD (RS=0, RW=0)
void LCD_command(unsigned char command)
{
    LCD_Rs = 0b00000000;  // RS = 0 (modo comando)
    LCD_Rw = 0b00000000;  // RW = 0 (modo escrita)
    uint8_t highnib = command & 0xf0;  // Extrai os 4 bits mais significativos
    uint8_t lownib = (command << 4) & 0xf0;  // Extrai os 4 bits menos significativos
    lcd_write4bit(highnib);  // Envia os 4 bits mais altos
    lcd_write4bit(lownib);  // Envia os 4 bits mais baixos
}

// Função para definir a posição do cursor
void lcd_setcursor(uint8_t col, uint8_t row)
{
    static uint8_t offsets[] = {0x00, 0x40, 0x14, 0x54};  // Offsets para diferentes linhas no LCD
    LCD_command((0x80 | (row << 6)) + col);  // Envia o comando para mover o cursor
    delay_us(40);  // Atraso curto
}

// Função para limpar o display do LCD
void lcd_clear()
{
    LCD_command(lcd_clear_all);  // Envia o comando para limpar o display
    delay_ms(20);  // Atraso para garantir a limpeza
}

// Função para retornar o cursor para o início
void lcd_Rethome()
{
    LCD_command(lcd_rhome);  // Envia o comando para retornar ao início
    delay_ms(2);  // Pequeno atraso
}

// Função para enviar dados ao LCD (RS=1, RW=0)
void LCD_data(unsigned char data)
{
    LCD_Rs = 0b00000001;  // RS = 1 (modo dados)
    LCD_Rw = 0b00000000;  // RW = 0 (modo escrita)
    uint8_t highnib = data & 0xf0;  // Extrai os 4 bits mais significativos
    uint8_t lownib = (data << 4) & 0xf0;  // Extrai os 4 bits menos significativos
    lcd_write4bit(highnib);  // Envia os 4 bits mais altos
    lcd_write4bit(lownib);  // Envia os 4 bits mais baixos
}

// Função para imprimir inteiros no formato fixo (ex: 001, 023, 015)
void lcd_printint(int value)
{
    unsigned char thousands, hundreds, tens, ones;

    thousands = value / 1000;
    if (thousands != 0) LCD_data(thousands + 0x30);

    hundreds = (value % 1000) / 100;
    LCD_data(hundreds + 0x30);

    tens = (value % 100) / 10;
    LCD_data(tens + 0x30);

    ones = value % 10;
    LCD_data(ones + 0x30);
}

// Função para imprimir inteiros no formato simples (ex: 1, 23, 6)
void lcd_printint_num(int value)
{
    unsigned char thousands, hundreds, tens, ones;

    thousands = value / 1000;
    if (thousands != 0) LCD_data(thousands + 0x30);

    hundreds = ((value - thousands * 1000) - 1) / 100;
    if (hundreds != 0) LCD_data(hundreds + 0x30);

    tens = (value % 100) / 10;
    if (tens != 0) LCD_data(tens + 0x30);

    ones = value % 10;
    LCD_data(ones + 0x30);
}

// Função para enviar uma string ao LCD
void LCD_string(const char* str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        LCD_data(str[i]);  // Envia cada caractere da string
        delay_us(45);  // Pequeno atraso para estabilização
    }
}

// Função para imprimir uma string na primeira linha
void lcd_print1line(const char *buff)
{
    lcd_setcursor(0, 0);  // Define o cursor para o início da primeira linha
    LCD_string(buff);  // Envia a string
}

// Função para imprimir uma string na segunda linha
void lcd_print2line(const char *buff)
{
    lcd_setcursor(0, 1);  // Define o cursor para o início da segunda linha
    LCD_string(buff);  // Envia a string
}

// Função para imprimir os bits de um valor no LCD
void lcd_bit(unsigned char val)
{
    int ptr;
    for (ptr = 7; ptr >= 0; ptr--)
    {
        if ((val & (1 << ptr)) == 0)
        {
            LCD_string("0");  // Imprime "0" para bits desligados
        }
        else
        {
            LCD_string("1");  // Imprime "1" para bits ligados
        }
    }
}

// Função para definir o endereço da RAM para caracteres personalizados
void setramaddr(uint8_t address)
{
    LCD_command(address);  // Envia o comando de endereço
    delay_us(50);  // Pequeno atraso
}

// Função para enviar um padrão de caractere personalizado ao LCD
void LCD_sendpattern(uint8_t loc, uint8_t pattern[])
{
    int i;
    if (loc < 8)
    {
        setramaddr((lcd_SETCGRAMADDR + (loc * 8)));  // Define o endereço da RAM para o caractere
        for (i = 0; i < 8; i++)
            LCD_data(pattern[i]);  // Envia o padrão
    }
}

// Função para imprimir um caractere personalizado no LCD
void LCD_printpattern(uint8_t loc)
{
    LCD_data(0x00 + loc);  // Imprime o caractere com base na localização
}

// Função de inicialização do LCD
void LCD_init()
{
    i2c_master_init();  // Inicializa a comunicação I2C
    delay_us(100);
    lcd_ioexander(0x00);  // Inicializa o expansor I/O
    delay_ms(25);  // Atraso de inicialização do LCD (>15ms)

    LCD_command(lcd_4bit_2line);  // Configura o LCD para modo 4 bits, 2 linhas
    delay_ms(5);
    LCD_command(lcd_4bit_2line);  // Reenvia o comando
    delay_us(160);
    LCD_command(lcd_4bit_2line);  // Reenvia o comando

    LCD_command(lcd_DN_CF_BF);  // Display ON, cursor OFF, sem blink
    delay_ms(1);
    LCD_command(lcd_CML_DMF);  // Cursor move à esquerda, sem deslocamento do display
    delay_us(40);
}
