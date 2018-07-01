#include "stm32f1xx_hal.h"
#include "tm1637_sm.h"

void _tm1637Start(tm1637_struct tm1637_handler);
void _tm1637Stop(tm1637_struct tm1637_handler);
void _tm1637ReadResult(tm1637_struct tm1637_handler);
void _tm1637WriteByte(tm1637_struct tm1637_handler, unsigned char b);
void _tm1637DelayUsec(unsigned int i);
void _tm1637ClkHigh(tm1637_struct tm1637_handler);
void _tm1637ClkLow(tm1637_struct tm1637_handler);
void _tm1637DioHigh(tm1637_struct tm1637_handler);
void _tm1637DioLow(tm1637_struct tm1637_handler);

// Configuration.

// #define CLK_PORT GPIOB
// #define DIO_PORT GPIOB
// #define CLK_PIN GPIO_PIN_0
// #define DIO_PIN GPIO_PIN_1
#define CLK_PORT_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define DIO_PORT_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE

const char segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};


void tm1637Init(tm1637_struct *tm1637_handler)
{
	CLK_PORT_CLK_ENABLE();
	DIO_PORT_CLK_ENABLE();
	GPIO_InitTypeDef g = {0};
	g.Pull = GPIO_PULLUP;
	g.Mode = GPIO_MODE_OUTPUT_PP; // PP = push pull
	g.Speed = GPIO_SPEED_FREQ_LOW;
	g.Pin = tm1637_handler->clk_pin;
	HAL_GPIO_Init(tm1637_handler->clk_port, &g);
	g.Pin = tm1637_handler->dio_pin;
	HAL_GPIO_Init(tm1637_handler->dio_port, &g);

	/*
	  GPIO_InitTypeDef GPIO_InitStruct;

	  CLK_PORT_CLK_ENABLE
	  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
	  GPIO_InitStruct.Pin = CLK_PIN;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(CLK_PORT, &GPIO_InitStruct);

	  DIO_PORT_CLK_ENABLE
	  HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);
	  GPIO_InitStruct.Pin = DIO_PIN;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(DIO_PORT, &GPIO_InitStruct);
	*/
}

void tm1637DisplayDecimal(tm1637_struct *tm1637_handler, int v, int displaySeparator)
{
    unsigned char digitArr[4];
    for (int i = 0; i < 4; ++i) {
        digitArr[i] = segmentMap[v % 10];
        if (i == 2 && displaySeparator) {
            digitArr[i] |= 1 << 7;
        }
        v /= 10;
    }

    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler,0x40);
    _tm1637ReadResult(*tm1637_handler);
    _tm1637Stop(*tm1637_handler);

    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler, 0xc0);
    _tm1637ReadResult(*tm1637_handler);

    for (int i = 0; i < 4; ++i) {
        _tm1637WriteByte(*tm1637_handler, digitArr[3 - i]);
        _tm1637ReadResult(*tm1637_handler);
    }

    _tm1637Stop(*tm1637_handler);
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(tm1637_struct *tm1637_handler, char brightness)
{
    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler, 0x87 + brightness);
    _tm1637ReadResult(*tm1637_handler);
    _tm1637Stop(*tm1637_handler);
}

void _tm1637Start(tm1637_struct tm1637_handler)
{
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DioHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioLow(tm1637_handler);
}

void _tm1637Stop(tm1637_struct tm1637_handler)
{
    _tm1637ClkLow(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioLow(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioHigh(tm1637_handler);
}

void _tm1637ReadResult(tm1637_struct tm1637_handler)
{
    _tm1637ClkLow(tm1637_handler);
    _tm1637DelayUsec(5);
    // while (dio); // We're cheating here and not actually reading back the response.
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637ClkLow(tm1637_handler);
}

void _tm1637WriteByte(tm1637_struct tm1637_handler, unsigned char b)
{
    for (int i = 0; i < 8; ++i) {
        _tm1637ClkLow(tm1637_handler);
        if (b & 0x01) {
            _tm1637DioHigh(tm1637_handler);
        }
        else {
            _tm1637DioLow(tm1637_handler);
        }
        _tm1637DelayUsec(3);
        b >>= 1;
        _tm1637ClkHigh(tm1637_handler);
        _tm1637DelayUsec(3);
    }
}

void _tm1637DelayUsec(unsigned int i)
{
    for (; i>0; i--) {
        for (int j = 0; j < 10; ++j) {
            __asm__ __volatile__("nop\n\t":::"memory");
        }
    }
}

void _tm1637ClkHigh(tm1637_struct tm1637_handler)
{
    HAL_GPIO_WritePin(tm1637_handler.clk_port, tm1637_handler.clk_pin , GPIO_PIN_SET);
}

void _tm1637ClkLow(tm1637_struct tm1637_handler)
{
    HAL_GPIO_WritePin(tm1637_handler.clk_port, tm1637_handler.clk_pin, GPIO_PIN_RESET);
}

void _tm1637DioHigh(tm1637_struct tm1637_handler)
{
    HAL_GPIO_WritePin(tm1637_handler.dio_port, tm1637_handler.dio_pin, GPIO_PIN_SET);
}

void _tm1637DioLow(tm1637_struct tm1637_handler)
{
    HAL_GPIO_WritePin(tm1637_handler.dio_port, tm1637_handler.dio_pin, GPIO_PIN_RESET);
}
