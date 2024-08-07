#include "stm32f1xx_hal.h"
#include "tm1637_sm.h"
 //*****************************************************************

	void _tm1637Start		(tm1637_struct tm1637_handler);
	void _tm1637Stop		(tm1637_struct tm1637_handler);
	void _tm1637ReadResult	(tm1637_struct tm1637_handler);
	void _tm1637WriteByte	(tm1637_struct tm1637_handler, uint8_t byte);
	void _tm1637DelayUsec	(uint32_t i);
	void _tm1637ClkHigh		(tm1637_struct tm1637_handler);
	void _tm1637ClkLow		(tm1637_struct tm1637_handler);
	void _tm1637DioHigh		(tm1637_struct tm1637_handler);
	void _tm1637DioLow		(tm1637_struct tm1637_handler);
//*****************************************************************

const uint8_t segmentMap[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
    0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
    0x00
};  //*****************************************************************


void TM1637_Init(tm1637_struct *tm1637_handler) {
	if (tm1637_handler == NULL) {
		return;
	}

	HAL_GPIO_WritePin(tm1637_handler->clk_port, tm1637_handler->clk_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(tm1637_handler->dio_port, tm1637_handler->dio_pin , GPIO_PIN_RESET);

	GPIO_InitTypeDef tm1637_gpio = {0};
	tm1637_gpio.Pull = GPIO_PULLUP;
	tm1637_gpio.Mode = GPIO_MODE_OUTPUT_PP; // PP = push pull
	tm1637_gpio.Speed = GPIO_SPEED_FREQ_LOW;

	tm1637_gpio.Pin = tm1637_handler->clk_pin;
	HAL_GPIO_Init(tm1637_handler->clk_port, &tm1637_gpio);

	tm1637_gpio.Pin = tm1637_handler->dio_pin;
	HAL_GPIO_Init(tm1637_handler->dio_port, &tm1637_gpio);
} //*****************************************************************

void TM1637_Display_Decimal(tm1637_struct *tm1637_handler, uint32_t _tm_value, double_dot_enum _double_dot) {
	if (tm1637_handler == NULL) {
		return;
	}

	if (_double_dot != no_double_dot) {
		_double_dot = double_dot;
	}

    uint8_t digitArr[6];

    for (uint8_t i = 0; i < tm1637_handler->digit_qnt; i++) {
        digitArr[i] = segmentMap[_tm_value % 10];
        if (i == 2 && _double_dot) {
            digitArr[i] |= 1 << 7;
        }
        _tm_value /= 10;
    }

    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler,0x40);
    _tm1637ReadResult(*tm1637_handler);
    _tm1637Stop(*tm1637_handler);

    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler, 0xc0);
    _tm1637ReadResult(*tm1637_handler);

    if (tm1637_handler->digit_qnt == 4) {
		for (int i = 3; i >= 0; i--) {
			_tm1637WriteByte(*tm1637_handler, digitArr[i]);
			_tm1637ReadResult(*tm1637_handler);
		}
    }
    if (tm1637_handler->digit_qnt == 6) {
    	for ( int i=3; i<6; i++) {
    		_tm1637WriteByte(*tm1637_handler, digitArr[i]);
    		_tm1637ReadResult(*tm1637_handler);
    	}
       	for ( int i=0; i<3; i++) {
			_tm1637WriteByte(*tm1637_handler, digitArr[i]);
			_tm1637ReadResult(*tm1637_handler);
       	}
    }
    _tm1637Stop(*tm1637_handler);
} //*****************************************************************

void TM1637_Set_Brightness(tm1637_struct *tm1637_handler, brightness_enum _brightness) {
	if (tm1637_handler == NULL) {
		return;
	}

	if (_brightness > bright_full) {
		_brightness = bright_full;
	}

	// Valid brightness values: 0 - 8.
	// 0 = display off.

    // Brightness command:
    // 1000 0XXX = display off
    // 1000 1BBB = display on, brightness 0-7
    // X = don't care
    // B = brightness
    _tm1637Start(*tm1637_handler);
    _tm1637WriteByte(*tm1637_handler, 0x87 + _brightness);
    _tm1637ReadResult(*tm1637_handler);
    _tm1637Stop(*tm1637_handler);
} //*****************************************************************

void _tm1637Start(tm1637_struct tm1637_handler) {
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DioHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioLow(tm1637_handler);
} //*****************************************************************

void _tm1637Stop(tm1637_struct tm1637_handler) {
    _tm1637ClkLow(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioLow(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637DioHigh(tm1637_handler);
} //*****************************************************************

void _tm1637ReadResult(tm1637_struct tm1637_handler) {
    _tm1637ClkLow(tm1637_handler);
    _tm1637DelayUsec(5);
    // while (dio); // We're cheating here and not actually reading back the response.
    _tm1637ClkHigh(tm1637_handler);
    _tm1637DelayUsec(2);
    _tm1637ClkLow(tm1637_handler);
} //*****************************************************************

void _tm1637WriteByte(tm1637_struct tm1637_handler, uint8_t byte) {
    for (uint8_t i = 0; i < 8; ++i) {
        _tm1637ClkLow(tm1637_handler);
        if (byte & 0x01) {
            _tm1637DioHigh(tm1637_handler);
        } else {
            _tm1637DioLow(tm1637_handler);
        }
        _tm1637DelayUsec(3);
        byte >>= 1;
        _tm1637ClkHigh(tm1637_handler);
        _tm1637DelayUsec(3);
    }
} //*****************************************************************

void _tm1637DelayUsec(uint32_t i) {
    for (; i>0; i--) {
        for (int j = 0; j < 10; ++j) {
            __asm__ __volatile__("nop\n\t":::"memory");
        }
    }
} //*****************************************************************

void _tm1637ClkHigh(tm1637_struct tm1637_handler) {
    HAL_GPIO_WritePin(tm1637_handler.clk_port, tm1637_handler.clk_pin , GPIO_PIN_SET);
} //*****************************************************************

void _tm1637ClkLow(tm1637_struct tm1637_handler) {
    HAL_GPIO_WritePin(tm1637_handler.clk_port, tm1637_handler.clk_pin, GPIO_PIN_RESET);
} //*****************************************************************

void _tm1637DioHigh(tm1637_struct tm1637_handler) {
    HAL_GPIO_WritePin(tm1637_handler.dio_port, tm1637_handler.dio_pin, GPIO_PIN_SET);
} //*****************************************************************

void _tm1637DioLow(tm1637_struct tm1637_handler) {
    HAL_GPIO_WritePin(tm1637_handler.dio_port, tm1637_handler.dio_pin, GPIO_PIN_RESET);
} //*****************************************************************
