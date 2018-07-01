#pragma once

void tm1637Init(void);
void tm1637DisplayDecimal(int v, int displaySeparator);
void tm1637SetBrightness(char brightness);

// tm1637Init();
// tm1637SetBrightness(2);
// tm1637DisplayDecimal(1045,1);