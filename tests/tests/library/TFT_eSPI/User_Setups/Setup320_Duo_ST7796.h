
#define USER_SETUP_ID 320

#define ST7796_DRIVER

// #define TFT_MOSI 10 // In some display driver board, it might be written as "SDA" and so on.15
// #define TFT_SCLK 9 //14
// #define TFT_MISO   -1   // Not connected
#define TFT_CS   22  // Chip select control pin
#define TFT_DC   26  // Data Command control pin
#define TFT_RST  21  // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL   27  // LED back-light

#define TOUCH_CS -1

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define TFT_BACKLIGHT_ON 1

#define TFT_WIDTH  480
#define TFT_HEIGHT 320
#define TFT_INVERSION_ON

// #define TFT_SDA_READ

// #define TFT_INVERSION_OFF

// #define SPI_FREQUENCY  27000000
// #define SPI_READ_FREQUENCY  20000000
// #define SPI_TOUCH_FREQUENCY  2500000 
#define SPI_FREQUENCY  46000000

// Optional reduced SPI frequency for reading TFT
// #define SPI_READ_FREQUENCY  5000000

// #define TFT_RGB_ORDER TFT_RGB

#define SPI_HAS_TRANSACTION