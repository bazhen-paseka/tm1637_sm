#ifndef TM1637_SM_H_INCLUDED
#define TM1637_SM_H_INCLUDED

typedef struct
{
	GPIO_TypeDef 	*clk_port;
    uint16_t 		clk_pin;

    GPIO_TypeDef 	*dio_port;
    uint16_t 		dio_pin;
} tm1637_struct;

typedef enum
{
	bright_off			= 0,
	bright_15percent	= 1,
	bright_30percent	= 2,
	bright_45percent	= 3,
	bright_60percent	= 4,
	bright_75percent	= 5,
	bright_90percent	= 6,
	bright_full			= 7,
} brightness_enum;

void tm1637_Init(tm1637_struct *tm1637_handler);
void tm1637_Display_Decimal(tm1637_struct *tm1637_handler, uint32_t value, uint8_t DisplaySeparator);
void tm1637_Set_Brightness(tm1637_struct *tm1637_handler, brightness_enum brightness);

#endif // TM1637_SM_H_INCLUDED
