/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */

 #include "LCD_GFX.h"
 #include "ST7735.h"
 #include <math.h>
 #include <stdlib.h>
 #include <util/delay.h>
 #include <string.h>

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if (((pixels>>j)&1)==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(const uint8_t x0, const uint8_t y0, uint8_t radius,uint16_t color){
	// Fill this out
	int x = 0, y = radius;
	int d = 3 - 2 * radius;
	drawCircle(x0, y0, x, y, color);
	while (y >= x){
		x++;
		if (d > 0){
			y--;
			d = d + 4 * (x - y) + 10;
		}else{
			d = d + 4 * x + 6;
		}
		drawCircle(x0, y0, x, y, color);
	}
}

//function for LCD_drawCircle function above
void drawCircle(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint16_t color){
	LCD_drawPixel(xc, yc, color);
	
	LCD_drawPixel(xc+x, yc+y, color);
	LCD_drawPixel(xc-x, yc+y, color);
	LCD_drawPixel(xc+x, yc-y, color);
	LCD_drawPixel(xc-x, yc-y, color);
	LCD_drawPixel(xc+y, yc+x, color);
	LCD_drawPixel(xc-y, yc+x, color);
	LCD_drawPixel(xc+y, yc-x, color);
	LCD_drawPixel(xc-y, yc-x, color);
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c){
	// Fill this out
	//the following 2 ifs are making sure x1>x0 and y1>y0
	if(x0>x1){
		int temp = x0;
		x0 = x1;
		x1 = temp;
	}
	if(y0>y1){
		int temp = y0;
		y0 = y1;
		y1 = temp;
	}
	
	//draw line in horizontal direction
	float m = (float)(y1-y0)/(x1-x0);
	int i,j;
	for (i=x0;i<=x1;i++){
		j = round(m*i + y0);
		LCD_drawPixel(i,j,c);
	}
	
	//draw line in vertical direction
	float n = (float)(x1-x0)/(y1-y0);
	for (i=y0;i<=y1;i++){
		j = round(n*i + x0);
		LCD_drawPixel(j,i,c);
	}	
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color){
	LCD_setAddr(x0,y0,x1,y1);
	int i;
	for(i=0;i<(x1-x0+1)*(y1-y0+1);i++){
		SPI_ControllerTx_16bit(color);
	}
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color){
	LCD_setAddr(0,0,160,128);
	int i;
	for(i=0;i<160*128;i++){
		SPI_ControllerTx_16bit(color);
	}
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg){
	// Fill this out
	int i;
	for(i=0;i<strlen(str);i++){
		LCD_drawChar(x+6*i,y,str[i],fg,bg);
	}
}

//draw number to screen, take in int variable
void LCD_drawNum(uint8_t x, uint8_t y, int num, uint16_t fg, uint16_t bg){
	// Fill this out
	char str[20];
	itoa(num,str,10);
	int i;
	for(i=0;i<strlen(str);i++){
		LCD_drawChar(x+6*i,y,str[i],fg,bg);
	}
}
