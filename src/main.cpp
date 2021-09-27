#include "Arduino.h"
#include "Ap_29demo.h"
//IO settings
int BUSY_Pin = A14;
int RES_Pin = A15;
int DC_Pin = A16;
int CS_Pin = A17;
int SCK_Pin = A18;
int SDI_Pin = A19;

#define EPD_W21_MOSI_0 digitalWrite(SDI_Pin, LOW)
#define EPD_W21_MOSI_1 digitalWrite(SDI_Pin, HIGH)

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin, LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin, HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin, LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin, HIGH)

#define EPD_W21_DC_0 digitalWrite(DC_Pin, LOW)
#define EPD_W21_DC_1 digitalWrite(DC_Pin, HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin, LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin, HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_W21_Init(void);
void EPD_init(int switch_init);
void PIC_display(int switch_display);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
void Acep_color(unsigned char color);
unsigned char Color_get(unsigned char color);

unsigned char HRES_byte1, HRES_byte2, VRES_byte1, VRES_byte2;

void setup()
{
  Serial.begin(115200);
  pinMode(BUSY_Pin, INPUT);
  pinMode(RES_Pin, OUTPUT);
  pinMode(DC_Pin, OUTPUT);
  pinMode(CS_Pin, OUTPUT);
  pinMode(SCK_Pin, OUTPUT);
  pinMode(SDI_Pin, OUTPUT);
}

void loop()
{
  //PICTURE1
  EPD_init(5); //EPD init
  PIC_display(5);
  EPD_refresh(); //EPD_refresh
  EPD_sleep();   //EPD_sleep,Sleep instruction is necessary, please do not delete!!!
  delay(3000);
  /*
    //PICTURE Clean
     EPD_init(); //EPD init
    PIC_display_Clean();
    EPD_refresh();//EPD_refresh   
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!*/
  while (1)
    ;
}

void driver_delay_us(unsigned int xus) //1us
{
  for (; xus > 1; xus--)
    ;
}
void driver_delay_xms(unsigned long xms) //1ms
{
  unsigned long i = 0, j = 0;

  for (j = 0; j < xms; j++)
  {
    for (i = 0; i < 256; i++)
      ;
  }
}
void DELAY_S(unsigned int delaytime)
{
  int i, j, k;
  for (i = 0; i < delaytime; i++)
  {
    for (j = 0; j < 4000; j++)
    {
      for (k = 0; k < 222; k++)
        ;
    }
  }
}
//////////////////////SPI///////////////////////////////////
void SPI_Delay(unsigned char xrate)
{
  unsigned char i;
  while (xrate)
  {
    for (i = 0; i < 2; i++)
      ;
    xrate--;
  }
}
void SPI_Write(unsigned char value)
{
  unsigned char i;
  SPI_Delay(1);
  for (i = 0; i < 8; i++)
  {
    EPD_W21_CLK_0;
    SPI_Delay(1);
    if (value & 0x80)
      EPD_W21_MOSI_1;
    else
      EPD_W21_MOSI_0;
    value = (value << 1);
    SPI_Delay(1);
    driver_delay_us(1);
    EPD_W21_CLK_1;
    SPI_Delay(1);
  }
}
void EPD_W21_WriteCMD(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_0; // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_1; // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

void EPD_W21_Init(void)
{
  EPD_W21_RST_0; // Module reset
  delay(100);    //At least 10ms
  EPD_W21_RST_1;
  delay(100);
}
void EPD_init(int switch_init) //   0: 7.5黑白红800*480  1: 7.5黑白红640*384  2: 7.5黑白800*480  3: 5.83黑白红640*480  4：5.83黑白黄640*384  5：5.3七彩
{
  switch (switch_init)
  {
  case 0:
    HRES_byte1 = 0x03; //800
    HRES_byte2 = 0x20;
    VRES_byte1 = 0x01; //480
    VRES_byte2 = 0xE0;

    EPD_W21_Init();

    EPD_W21_WriteCMD(0x01); //POWER SETTING
    EPD_W21_WriteDATA(0x07);
    EPD_W21_WriteDATA(0x07); //VGH=20V,VGL=-20V
    EPD_W21_WriteDATA(0x3f); //VDH=15V
    EPD_W21_WriteDATA(0x3f); //VDL=-15V

    EPD_W21_WriteCMD(0x04); //Power on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0X00);  //PANNEL SETTING
    EPD_W21_WriteDATA(0x0F); //KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x61);        //tres
    EPD_W21_WriteDATA(HRES_byte1); //source 800
    EPD_W21_WriteDATA(HRES_byte2);
    EPD_W21_WriteDATA(VRES_byte1); //gate 480
    EPD_W21_WriteDATA(VRES_byte2);

    EPD_W21_WriteCMD(0X15);
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x11);
    EPD_W21_WriteDATA(0x07);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);
    break;

  case 1:

    HRES_byte1 = 0x02; //640
    HRES_byte2 = 0x80;
    VRES_byte1 = 0x01; //384
    VRES_byte2 = 0x80;

    EPD_W21_Init(); //Electronic paper IC reset
    EPD_W21_WriteCMD(0x01);
    EPD_W21_WriteDATA(0x37); //POWER SETTING
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X00); //PANNEL SETTING
    EPD_W21_WriteDATA(0xCF);
    EPD_W21_WriteDATA(0x08);

    EPD_W21_WriteCMD(0x30);  //PLL setting
    EPD_W21_WriteDATA(0x3a); //PLL:    0-15℃:0x3C, 15+:0x3A

    EPD_W21_WriteCMD(0X82);  //VCOM VOLTAGE SETTING
    EPD_W21_WriteDATA(0x28); //all temperature  range

    EPD_W21_WriteCMD(0x06); //boost设定
    EPD_W21_WriteDATA(0xc7);
    EPD_W21_WriteDATA(0xcc);
    EPD_W21_WriteDATA(0x15);

    EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x77);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);

    EPD_W21_WriteCMD(0X65); //FLASH CONTROL
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0x61);        //tres
    EPD_W21_WriteDATA(HRES_byte1); //source 600
    EPD_W21_WriteDATA(HRES_byte2);
    EPD_W21_WriteDATA(VRES_byte1); //gate 448
    EPD_W21_WriteDATA(VRES_byte2);

    EPD_W21_WriteCMD(0xe5); //FLASH MODE
    EPD_W21_WriteDATA(0x03);

    EPD_W21_WriteCMD(0x04); //POWER ON
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
    break;
  case 2:
    HRES_byte1 = 0x03; //800
    HRES_byte2 = 0x20;
    VRES_byte1 = 0x01; //480
    VRES_byte2 = 0xE0;

    EPD_W21_Init(); //Electronic paper IC reset

    EPD_W21_WriteCMD(0x01); //POWER SETTING
    EPD_W21_WriteDATA(0x07);
    EPD_W21_WriteDATA(0x07); //VGH=20V,VGL=-20V
    EPD_W21_WriteDATA(0x3f); //VDH=15V
    EPD_W21_WriteDATA(0x3f); //VDL=-15V

    EPD_W21_WriteCMD(0x04); //Power on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0X00);  //PANNEL SETTING
    EPD_W21_WriteDATA(0x1F); //KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x61);        //tres
    EPD_W21_WriteDATA(HRES_byte1); //source 800
    EPD_W21_WriteDATA(HRES_byte2);
    EPD_W21_WriteDATA(VRES_byte1); //gate 480
    EPD_W21_WriteDATA(VRES_byte2);

    EPD_W21_WriteCMD(0X15);
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x10);
    EPD_W21_WriteDATA(0x07);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);

    break;
  case 3:
    HRES_byte1 = 0x02; //648
    HRES_byte2 = 0x88;
    VRES_byte1 = 0x01; //480
    VRES_byte2 = 0xE0;

    EPD_W21_Init(); //Electronic paper IC reset

    EPD_W21_WriteCMD(0x01); //POWER SETTING
    EPD_W21_WriteDATA(0x07);
    EPD_W21_WriteDATA(0x07); //VGH=20V,VGL=-20V
    EPD_W21_WriteDATA(0x3f); //VDH=15V
    EPD_W21_WriteDATA(0x3f); //VDL=-15V

    EPD_W21_WriteCMD(0x04); //POWER ON
    driver_delay_xms(100);
    lcd_chkstatus(); //waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0X00);  //PANNEL SETTING
    EPD_W21_WriteDATA(0x0F); //KW-3f   KWR-2F	BWROTP 0f	BWOTP 1f

    EPD_W21_WriteCMD(0x61);        //tres
    EPD_W21_WriteDATA(HRES_byte1); //source 648
    EPD_W21_WriteDATA(HRES_byte2);
    EPD_W21_WriteDATA(VRES_byte1); //gate 480
    EPD_W21_WriteDATA(VRES_byte2);

    EPD_W21_WriteCMD(0X15);
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x11);
    EPD_W21_WriteDATA(0x07);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);
    break;
  case 4:
    HRES_byte1 = 0x02; //600
    HRES_byte2 = 0x58;
    VRES_byte1 = 0x01; //448
    VRES_byte2 = 0xc0;

    EPD_W21_Init(); //Electronic paper IC reset
    EPD_W21_WriteCMD(0x01);
    EPD_W21_WriteDATA(0x37); //POWER SETTING
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X00); //PANNEL SETTING
    EPD_W21_WriteDATA(0xCF);
    EPD_W21_WriteDATA(0x08);

    EPD_W21_WriteCMD(0x30);  //PLL setting
    EPD_W21_WriteDATA(0x3a); //PLL:    0-15¡æ:0x3C, 15+:0x3A

    EPD_W21_WriteCMD(0X82);  //VCOM VOLTAGE SETTING
    EPD_W21_WriteDATA(0x28); //all temperature  range

    EPD_W21_WriteCMD(0x06); //boostÉè¶¨
    EPD_W21_WriteDATA(0xc7);
    EPD_W21_WriteDATA(0xcc);
    EPD_W21_WriteDATA(0x15);

    EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x77);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);

    EPD_W21_WriteCMD(0X65); //FLASH CONTROL
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0x61);        //tres
    EPD_W21_WriteDATA(HRES_byte1); //source 600
    EPD_W21_WriteDATA(HRES_byte2);
    EPD_W21_WriteDATA(VRES_byte1); //gate 448
    EPD_W21_WriteDATA(VRES_byte2);

    EPD_W21_WriteCMD(0xe5); //FLASH MODE
    EPD_W21_WriteDATA(0x03);

    EPD_W21_WriteCMD(0x04); //Power on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
    break;
  case 5:

    EPD_W21_Init();         //Electronic paper IC reset
    EPD_W21_WriteCMD(0x01); //POWER SETTING
    EPD_W21_WriteDATA(0x37);
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteDATA(0x23);
    EPD_W21_WriteDATA(0x23);

    EPD_W21_WriteCMD(0X00); //PANNEL SETTING
    EPD_W21_WriteDATA(0xEF);
    EPD_W21_WriteDATA(0x08);

    EPD_W21_WriteCMD(0x03); //PFS
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0x06); //boostÉè¶¨
    EPD_W21_WriteDATA(0xC7);
    EPD_W21_WriteDATA(0xC7);
    EPD_W21_WriteDATA(0x1D);

    EPD_W21_WriteCMD(0x30);  //PLL setting
    EPD_W21_WriteDATA(0x3C); //PLL:    0-25¡æ:0x3C,25+:0x3A

    EPD_W21_WriteCMD(0X41); //TSE
    EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x37); //0x77

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);

    EPD_W21_WriteCMD(0X60); //TCON SETTING
    EPD_W21_WriteDATA(0x22);

    EPD_W21_WriteCMD(0x61);  //tres
    EPD_W21_WriteDATA(0x02); //source 600
    EPD_W21_WriteDATA(0x58);
    EPD_W21_WriteDATA(0x01); //gate 448
    EPD_W21_WriteDATA(0xC0);

    EPD_W21_WriteCMD(0xE3); //PWS
    EPD_W21_WriteDATA(0xAA);

    EPD_W21_WriteCMD(0x04); //PWR on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

    break;
  default:
    break;
  }
}

void EPD_refresh(void)
{
  EPD_W21_WriteCMD(0x12); //DISPLAY REFRESH
  driver_delay_xms(100);  //!!!The delay here is necessary, 200uS at least!!!
  lcd_chkstatus();
}
void EPD_sleep(void)
{
  EPD_W21_WriteCMD(0X50); //VCOM AND DATA INTERVAL SETTING
  EPD_W21_WriteDATA(0x97);

  EPD_W21_WriteCMD(0X02); //power off
  lcd_chkstatus();
  EPD_W21_WriteCMD(0X07); //deep sleep
  EPD_W21_WriteDATA(0xA5);
}

void PIC_display(int switch_display)//void PIC_display(int switch_display,unsigned *pic_data)pic_data 用来分包传入数据写入数据，7.5黑白红48000数据，7.5黑白红640*384 61400数据，7.5黑白 48000，5.83黑白红38880红/黑白，5.83黑白黄21750，七彩5.3 268800
{
  switch (switch_display)
  {
    unsigned int i, j, k, temp1, temp2, temp3, data_H, data_L, data;
  case 0:
    EPD_W21_WriteCMD(0x10); //Transfer old data
    for (i = 0; i < 48000; i++)
      EPD_W21_WriteDATA(~pgm_read_byte(&gImage_BW[i])); //BW

    EPD_W21_WriteCMD(0x13); //Transfer new data
    for (i = 0; i < 48000; i++)
      EPD_W21_WriteDATA(pgm_read_byte(&gImage_R[i])); //red
    break;
  case 1:
    EPD_W21_WriteCMD(0x10);     //start to transport picture
    for (i = 0; i < 61440; i++) //2bit for a pixels(old is 4bit for a pixels)
    {
      temp1 = gImage_BWR[i];
      for (j = 0; j < 2; j++) //2bit to 4bit
      {
        temp2 = temp1 & 0xc0; //Analysis the first 2bit
        if (temp2 == 0xc0)
          temp3 = 0x00; //black(2bit to 4bit)
        else if (temp2 == 0x00)
          temp3 = 0x03; //white(2bit to 4bit)
        else
          temp3 = 0x04; //red(2bit to 4bit)

        temp3 <<= 4;          //move to the Hight 4bit
        temp1 <<= 2;          //move 2bit
        temp2 = temp1 & 0xc0; //Analysis the second 2bit
        if (temp2 == 0xc0)
          temp3 |= 0x00; //black(2bit to 4bit),Data consolidation
        else if (temp2 == 0x00)
          temp3 |= 0x03; //white(2bit to 4bit),Data consolidation
        else
          temp3 |= 0x04; //red(2bit to 4bit),Data consolidation

        temp1 <<= 2; //move 2bit，turn the next 2bit

        EPD_W21_WriteDATA(temp3); //write a byte,Contains two 4bit pixels
      }
    }
    break;
  case 2:

    EPD_W21_WriteCMD(0x10); //Transfer old data
    for (i = 0; i < 48000; i++)
      EPD_W21_WriteDATA(0x00);

    EPD_W21_WriteCMD(0x13); //Transfer new data
    for (i = 0; i < 48000; i++)
      EPD_W21_WriteDATA(pgm_read_byte(&gImage_Black[i]));
    break;
  case 3:
    EPD_W21_WriteCMD(0x10);
    for (i = 0; i < 38880; i++)
    {
      EPD_W21_WriteDATA(~(picData_BW[i]));
    }
    EPD_W21_WriteCMD(0x13);
    for (i = 0; i < 38880; i++)
    {
      EPD_W21_WriteDATA(picData_R[i]);
    }
    break;
  case 4:
    EPD_W21_WriteCMD(0x10);     //start to transport picture
    for (i = 0; i < 21750; i++) //2bit for a pixels(old is 4bit for a pixels)
    {
      temp1 = pgm_read_byte(&gImage_600x145_1[i]);
      for (j = 0; j < 2; j++) //2bit to 4bit
      {
        temp2 = temp1 & 0xc0; //Analysis the first 2bit
        if (temp2 == 0xc0)
          temp3 = 0x00; //black(2bit to 4bit)
        else if (temp2 == 0x00)
          temp3 = 0x03; //white(2bit to 4bit)
        else
          temp3 = 0x04; //red(2bit to 4bit)

        temp3 <<= 4;          //move to the Hight 4bit
        temp1 <<= 2;          //move 2bit
        temp2 = temp1 & 0xc0; //Analysis the second 2bit
        if (temp2 == 0xc0)
          temp3 |= 0x00; //black(2bit to 4bit),Data consolidation
        else if (temp2 == 0x00)
          temp3 |= 0x03; //white(2bit to 4bit),Data consolidation
        else
          temp3 |= 0x04; //red(2bit to 4bit),Data consolidation

        temp1 <<= 2; //move 2bit£¬turn the next 2bit

        EPD_W21_WriteDATA(temp3); //write a byte,Contains two 4bit pixels
      }
    }

    for (i = 21750; i < 67200; i++)
    {
      for (j = 0; j < 2; j++)    //1bit to 4bit
        EPD_W21_WriteDATA(0x44); //write a byte,Contains two 4bit pixels
    }

    break;
  case 5:
    //Acep_color(Clean);
    EPD_W21_WriteCMD(0x10);
    for (i = 0; i < 448; i++)
    {
      k = 0;
      for (j = 0; j < 300; j++)
      {

        temp1 = gImage_color256[i * 600 + k++];
        temp2 = gImage_color256[i * 600 + k++];
        data_H = Color_get(temp1) << 4;
        data_L = Color_get(temp2);
        data = data_H | data_L;
        EPD_W21_WriteDATA(data);
      }
    }

    //Refresh
    EPD_W21_WriteCMD(0x12); //DISPLAY REFRESH
    delay(1);               //!!!The delay here is necessary, 200uS at least!!!
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

    break;
  default:
    break;
  }
}

void PIC_display_Clean(void)
{
  unsigned int i;
  EPD_W21_WriteCMD(0x10); //Transfer old data
  for (i = 0; i < 48000; i++)
  {
    EPD_W21_WriteDATA(0xff);
  }

  EPD_W21_WriteCMD(0x13); //Transfer new data
  for (i = 0; i < 48000; i++)
  {
    EPD_W21_WriteDATA(0x00);
  }
}
void lcd_chkstatus(void)
{
  EPD_W21_WriteCMD(0x71);
  while (!isEPD_W21_BUSY)
    ; //0 is busy
}

void Acep_color(unsigned char color)
{
  unsigned int i, j;
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < 448; i++)
  {
    for (j = 0; j < 300; j++)
    {
      EPD_W21_WriteDATA(color);
    }
  }

  //Refresh
  EPD_W21_WriteCMD(0x12); //DISPLAY REFRESH
  delay(1);               //!!!The delay here is necessary, 200uS at least!!!
  lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
}
unsigned char Color_get(unsigned char color)
{
  unsigned datas;
  switch (color)
  {
  case 0xFF:
    datas = white;
    break;
  case 0xFC:
    datas = yellow;
    break;
  case 0xEC:
    datas = orange;
    break;
  case 0xE0:
    datas = red;
    break;
  case 0x35:
    datas = green;
    break;
  case 0x2B:
    datas = blue;
    break;
  case 0x00:
    datas = black;
    break;
  default:
    break;
  }
  return datas;
}
