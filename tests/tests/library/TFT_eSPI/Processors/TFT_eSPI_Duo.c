        ////////////////////////////////////////////////////
        //       TFT_eSPI generic driver functions        //
        ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////

// Select the SPI port to use
#ifdef TFT_SPI_PORT
  SPIClass& spi = TFT_SPI_PORT;
#else
  SPIClass& spi = SPI;
#endif

////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ) && !defined (TFT_PARALLEL_8_BIT)
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           tft_Read_8
** Description:             Bit bashed SPI to read bidirectional SDA line
***************************************************************************************/
uint8_t TFT_eSPI::tft_Read_8(void)
{
  uint8_t  ret = 0;

  for (uint8_t i = 0; i < 8; i++) {  // read results
    ret <<= 1;
    SCLK_L;
    if (digitalRead(TFT_MOSI)) ret |= 1;
    SCLK_H;
  }

  return ret;
}

/***************************************************************************************
** Function name:           beginSDA
** Description:             Detach SPI from pin to permit software SPI
***************************************************************************************/
void TFT_eSPI::begin_SDA_Read(void)
{
  // Release configured SPI port for SDA read
  spi.end();
}

/***************************************************************************************
** Function name:           endSDA
** Description:             Attach SPI pins after software SPI
***************************************************************************************/
void TFT_eSPI::end_SDA_Read(void)
{
  // Configure SPI port ready for next TFT access
  spi.begin();
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // #if defined (TFT_SDA_READ)
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_PARALLEL_8_BIT) // Code for generic (i.e. any) processor
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for generic processor and parallel display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  while (len>1) {tft_Write_32D(color); len-=2;}
  if (len) {tft_Write_16(color);}
}

/***************************************************************************************
** Function name:           pushPixels - for gereric processor and parallel display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if(_swapBytes) {
    while (len>1) {tft_Write_16(*data); data++; tft_Write_16(*data); data++; len -=2;}
    if (len) {tft_Write_16(*data);}
    return;
  }

  while (len>1) {tft_Write_16S(*data); data++; tft_Write_16S(*data); data++; len -=2;}
  if (len) {tft_Write_16S(*data);}
}

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set parallel bus to INPUT or OUTPUT
***************************************************************************************/
void TFT_eSPI::busDir(uint32_t mask, uint8_t mode)
{
  // mask is unused for generic processor
  // Arduino native functions suited well to a generic driver
  pinMode(TFT_D0, mode);
  pinMode(TFT_D1, mode);
  pinMode(TFT_D2, mode);
  pinMode(TFT_D3, mode);
  pinMode(TFT_D4, mode);
  pinMode(TFT_D5, mode);
  pinMode(TFT_D6, mode);
  pinMode(TFT_D7, mode);
  return;
}

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Faster GPIO pin input/output switch
***************************************************************************************/
void TFT_eSPI::gpioMode(uint8_t gpio, uint8_t mode)
{
  // No fast port based generic approach available
}

/***************************************************************************************
** Function name:           read byte  - supports class functions
** Description:             Read a byte - parallel bus only
***************************************************************************************/
uint8_t TFT_eSPI::readByte(void)
{
  uint8_t b = 0;

  busDir(0, INPUT);
  digitalWrite(TFT_RD, LOW);

  b |= digitalRead(TFT_D0) << 0;
  b |= digitalRead(TFT_D1) << 1;
  b |= digitalRead(TFT_D2) << 2;
  b |= digitalRead(TFT_D3) << 3;
  b |= digitalRead(TFT_D4) << 4;
  b |= digitalRead(TFT_D5) << 5;
  b |= digitalRead(TFT_D6) << 6;
  b |= digitalRead(TFT_D7) << 7;

  digitalWrite(TFT_RD, HIGH);
  busDir(0, OUTPUT); 

  return b;
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (RPI_WRITE_STROBE)  // For RPi TFT with write strobe                      
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for ESP32 or STM32 RPi TFT
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  if(len) { tft_Write_16(color); len--; }
  while(len--) {WR_L; WR_H;}
}

/***************************************************************************************
** Function name:           pushPixels - for ESP32 or STM32 RPi TFT
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
  uint16_t *data = (uint16_t*)data_in;

  if (_swapBytes) while ( len-- ) {tft_Write_16S(*data); data++;}
  else while ( len-- ) {tft_Write_16(*data); data++;}
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (SPI_18BIT_DRIVER) // SPI 18 bit colour                         
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for STM32 and 3 byte RGB display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  // Split out the colours
  uint8_t r = (color & 0xF800)>>8;
  uint8_t g = (color & 0x07E0)>>3;
  uint8_t b = (color & 0x001F)<<3;

  while ( len-- ) {tft_Write_8(r); tft_Write_8(g); tft_Write_8(b);}
}

/***************************************************************************************
** Function name:           pushPixels - for STM32 and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if (_swapBytes) {
    while ( len-- ) {
      uint16_t color = *data >> 8 | *data << 8;
      tft_Write_8((color & 0xF800)>>8);
      tft_Write_8((color & 0x07E0)>>3);
      tft_Write_8((color & 0x001F)<<3);
      data++;
    }
  }
  else {
    while ( len-- ) {
      tft_Write_8((*data & 0xF800)>>8);
      tft_Write_8((*data & 0x07E0)>>3);
      tft_Write_8((*data & 0x001F)<<3);
      data++;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////
#else //                   Standard SPI 16 bit colour TFT                               
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for STM32
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
#define BUF_SIZE 256
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  // pr_printf("[%s]: run pushBlock \n\r", __func__);
  // pr_printf("[%s]: %x %d \n\r", __func__, color, len);

  // while ( len-- ) {tft_Write_16(color);}
 

  // uint8_t col[2];
  // uint8_t out[2];
  // col[0]=(uint8_t)((color) >> 8);
  // col[1]=(uint8_t)((color) >> 0);

  // while (len--)
  // {
  //   spi.transferBytes((uint8_t*)col, (uint8_t*)out, 2);
  // }

  uint8_t col[BUF_SIZE * 2];
  uint8_t out[BUF_SIZE * 2];
  if (len < BUF_SIZE)
  {
    for (uint32_t i = 0; i < len; i++)
    {
      col[2 * i] = (uint8_t)((color) >> 8);
      col[2 * i + 1] = (uint8_t)((color) >> 0);
    }

    spi.transferBytes((uint8_t *)col, (uint8_t *)out, len << 1);
    return;
  }

  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    col[2 * i] = (uint8_t)((color) >> 8);
    col[2 * i + 1] = (uint8_t)((color) >> 0);
  }

  do
  {
    spi.transferBytes((uint8_t *)col, (uint8_t *)out, BUF_SIZE << 1);
    len -= BUF_SIZE;
  } while (len >= BUF_SIZE);
  if (len)
    spi.transferBytes((uint8_t *)col, (uint8_t *)out, len << 1);
}

/***************************************************************************************
** Function name:           pushPixels - for STM32
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  // uint16_t *data = (uint16_t*)data_in;

  // if (_swapBytes) while ( len-- ) {tft_Write_16(*data); data++;}
  // else while ( len-- ) {tft_Write_16S(*data); data++;}

  uint16_t *data = (uint16_t *)data_in;
 
  if (_swapBytes)
  {
    uint8_t col[BUF_SIZE*2];
    while (len >= BUF_SIZE)
    {
      for (uint32_t i = 0; i < BUF_SIZE; i++)
      {
        col[2*i] = (uint8_t)((*data) >> 8);;
        col[2*i+1] = (uint8_t)((*data) >> 0);
        data++;
      }
      spi.transferBytes((uint8_t *)col, NULL, BUF_SIZE << 1);
      len -= BUF_SIZE;
    }
    for (uint32_t i = 0; i < len; i++)
    {
      col[2*i] = (uint8_t)((*data) >> 8);;
      col[2*i+1] = (uint8_t)((*data) >> 0);
      data++;
    }
    spi.transferBytes((uint8_t *)col, NULL, len << 1);
  }
  else
  {

    uint8_t newdata[0x800*2];
    while (len > 0x7FFF)
    {
      
      for (uint32_t i = 0; i < 0x800; i++)
      {
        newdata[2*i] = (uint8_t)((*data) >> 0);;
        newdata[2*i+1] = (uint8_t)((*data) >> 8);
      }
      spi.transferBytes((uint8_t *)newdata, NULL, 0x800 << 1);
      len -= 0x800;
      data += 0x800;
    }
    for (uint32_t i = 0; i < len; i++)
    {
        newdata[2*i] = (uint8_t)((*data) >> 0);;
        newdata[2*i+1] = (uint8_t)((*data) >> 8);
    }
    spi.transferBytes((uint8_t *)newdata, NULL, len << 1);
  }

}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of display interface specific functions
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//                                DMA FUNCTIONS                                         
////////////////////////////////////////////////////////////////////////////////////////

//                Placeholder for DMA functions

/*
Minimal function set to support DMA:

bool TFT_eSPI::initDMA(void)
void TFT_eSPI::deInitDMA(void)
bool TFT_eSPI::dmaBusy(void)
void TFT_eSPI::pushPixelsDMA(uint16_t* image, uint32_t len)
void TFT_eSPI::pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* image)

*/
