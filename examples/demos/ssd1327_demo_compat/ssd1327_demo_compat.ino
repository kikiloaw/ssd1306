/*
    MIT License

    Copyright (c) 2019, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
/**
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

/* !!! THIS DEMO RUNS in SSD1306 COMPATIBLE MODE */

#include "ssd1306.h"
#include "nano_engine.h"
#include "sova.h"

#define GRAY_COLOR4(x) (x/16)

/*
 * Heart image below is defined directly in flash memory.
 * This reduces SRAM consumption.
 * The image is defined from bottom to top (bits), from left to
 * right (bytes).
 */
const PROGMEM uint8_t heartImage[8] =
{
    0B00001110,
    0B00011111,
    0B00111111,
    0B01111110,
    0B01111110,
    0B00111101,
    0B00011001,
    0B00001110
};

const PROGMEM uint8_t heartImage4[4 * 8] =
{
    0xF0, 0x0F, 0xF0, 0x0F,
    0xFF, 0xFF, 0x7F, 0xF2,
    0xFF, 0xFF, 0xCF, 0xF7,
    0xFF, 0xFF, 0xFF, 0xF9,
    0xF0, 0xFF, 0xFF, 0x3F,
    0x00, 0xFF, 0xFF, 0x03,
    0x00, 0xF0, 0x3F, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

/*
 * Define sprite width. The width can be of any size.
 * But sprite height is always assumed to be 8 pixels
 * (number of bits in single byte).
 */
const int spriteWidth = sizeof(heartImage);

SAppMenu menu;

const char *menuItems[] =
{
    "draw bitmap",
    "sprites",
    "fonts",
    "canvas gfx",
    "draw lines",
};

static void bitmapDemo()
{
    ssd1306_clearScreen();
    ssd1306_setColor(GRAY_COLOR4(128));
    ssd1306_drawBitmap(0, 0, 96, 64, Sova);
    ssd1306_drawBitmap1_4(8, 72, 8, 8, heartImage4);
    ssd1306_drawBitmap1_4(18, 74, 8, 8, heartImage4);
    ssd1306_drawBitmap1_4(11, 82, 8, 8, heartImage4);
    delay(3000);
}

static void spriteDemo()
{
    ssd1306_setColor(GRAY_COLOR4(192));
    ssd1306_clearScreen();
    /* Declare variable that represents our sprite */
    SPRITE sprite;
    /* Create sprite at 0,0 position. The function initializes sprite structure. */
    sprite = ssd1306_createSprite( 0, 0, spriteWidth, heartImage );
    for (int i=0; i<250; i++)
    {
        delay(15);
        sprite.x++;
        if (sprite.x >= ssd1306_displayWidth())
        {
            sprite.x = 0;
        }
        sprite.y++;
        if (sprite.y >= ssd1306_displayHeight())
        {
            sprite.y = 0;
        }
        /* Erase sprite on old place. The library knows old position of the sprite. */
        sprite.eraseTrace();
        /* Draw sprite on new place */
        sprite.draw();
    }
}

static void textDemo()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    ssd1306_setColor(GRAY_COLOR4(255));
    ssd1306_printFixed(0,  8, "Normal text", STYLE_NORMAL);
    ssd1306_setColor(GRAY_COLOR4(192));
    ssd1306_printFixed(0, 16, "Bold text", STYLE_BOLD);
    ssd1306_setColor(GRAY_COLOR4(128));
    ssd1306_printFixed(0, 24, "Italic text", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_setColor(GRAY_COLOR4(164));
    ssd1306_printFixed(0, 32, "Inverted bold", STYLE_BOLD);
    ssd1306_positiveMode();
    delay(3000);
}

static void canvasDemo()
{
    uint8_t buffer[32*16/2];
    NanoCanvas1_4 canvas(32,16, buffer);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();
    canvas.clear();
    canvas.setColor(GRAY_COLOR4(127));
    canvas.fillRect(10, 3, 80, 5);
    canvas.blt((ssd1306_displayWidth()-32)/2, 1);
    delay(500);
    canvas.setColor(GRAY_COLOR4(255));
    canvas.fillRect(16, 1, 20, 15);
    canvas.blt((ssd1306_displayWidth()-32)/2, 1);
    delay(1500);
    canvas.setColor(GRAY_COLOR4(64));
    canvas.printFixed(3, 1, "DEMO", STYLE_BOLD );
    canvas.blt((ssd1306_displayWidth()-32)/2, 1);
    delay(3000);
}

static void drawLinesDemo()
{
    ssd1306_clearScreen();
    ssd1306_setColor(GRAY_COLOR4(128));
    for (uint8_t y = 0; y < ssd1306_displayHeight(); y += 8)
    {
        ssd1306_drawLine(0,0, ssd1306_displayWidth() -1, y);
    }
    ssd1306_setColor(GRAY_COLOR4(255));
    for (uint8_t x = ssd1306_displayWidth() - 1; x > 7; x -= 8)
    {
        ssd1306_drawLine(0,0, x, ssd1306_displayHeight() - 1);
    }
    delay(3000);
}

void setup()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1327_128x128_spi_init(3, 4, 5);  // Use this line for Atmega328p
//    ssd1327_128x128_spi_init(3, -1, 4); // Use this line for ATTINY
//    ssd1327_128x128_spi_init(24, 0, 23); // Use this line for Raspberry  (gpio24=RST, 0=CE, gpio23=D/C)

    ssd1306_clearScreen( );
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    ssd1306_setColor(GRAY_COLOR4(255));
    ssd1306_showMenu( &menu );
}

void loop()
{
    delay(1000);
    switch (ssd1306_menuSelection(&menu))
    {
        case 0:
            bitmapDemo();
            break;

        case 1:
            spriteDemo();
            break;

        case 2:
            textDemo();
            break;

        case 3:
            canvasDemo();
            break;

        case 4:
            drawLinesDemo();
            break;

        default:
            break;
    }
    ssd1306_clearScreen( );
    ssd1306_setColor(GRAY_COLOR4(255));
    ssd1306_showMenu(&menu);
    delay(500);
    ssd1306_menuDown(&menu);
    ssd1306_updateMenu(&menu);
}
