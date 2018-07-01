#ifndef TM1637_SM_H_INCLUDED
#define TM1637_SM_H_INCLUDED

typedef struct
{
	GPIO_TypeDef 	*clk_port;
    uint16_t 		clk_pin;

    GPIO_TypeDef 	*dio_port;
    uint16_t 		dio_pin;
} tm1637_struct;

void tm1637Init(tm1637_struct *tm1637_handler);
void tm1637DisplayDecimal(tm1637_struct *tm1637_handler, int v, int displaySeparator);
void tm1637SetBrightness(tm1637_struct *tm1637_handler, char brightness);



/* example:
	 tm1637Init();
	 tm1637SetBrightness(2);
	 tm1637DisplayDecimal(1045,1);
 */

#endif // TM1637_SM_H_INCLUDED
