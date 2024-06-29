#ifndef TM1637_SM_H_INCLUDED
#define TM1637_SM_H_INCLUDED
 //*****************************************************************

typedef struct {
	GPIO_TypeDef 	*clk_port;
    uint16_t 		clk_pin;

    GPIO_TypeDef 	*dio_port;
    uint16_t 		dio_pin;
} tm1637_struct;
//*****************************************************************

typedef enum {
	bright_off			= 0,
	bright_15percent	= 1,
	bright_30percent	= 2,
	bright_45percent	= 3,
	bright_60percent	= 4,
	bright_75percent	= 5,
	bright_90percent	= 6,
	bright_full			= 7
}	brightness_enum;

typedef enum {
	no_double_dot		= 0,
	   double_dot		= 1
}	double_dot_enum;
//*****************************************************************

void TM1637_Init			(tm1637_struct *tm1637_handler);
void TM1637_Set_Brightness	(tm1637_struct *tm1637_handler, brightness_enum _brightness);
void TM1637_Display_Decimal	(tm1637_struct *tm1637_handler, uint32_t _tm_value, double_dot_enum double_dot);

//*****************************************************************

	// Init:
	//	tm1637_struct h1_tm1637;
	//	h1_tm1637.clk_pin  = GPIO_PIN_6;
	//	h1_tm1637.clk_port = GPIOB;
	//	h1_tm1637.dio_pin  = GPIO_PIN_7;
	//	h1_tm1637.dio_port = GPIOB;
	//	TM1637_Init(&h1_tm1637);
	//	TM1637_Set_Brightness(&h1_tm1637, bright_15percent);
	//	TM1637_Display_Decimal(&h1_tm1637, 8689 , double_dot);
	// Main:
	//	TM1637_Display_Decimal(&h1_tm1637, volt_u32, no_double_dot);
	//*****************************************************************

#endif // TM1637_SM_H_INCLUDED
