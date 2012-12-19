#include <U8glib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdlib.h>
#include <avr/io.h> 
#include <avr/interrupt.h> 

//will take care of dallas 1wire on pin4
OneWire oneWireSensors(4); 
//will talk with DS18B20s and DS18S12 over 1wire
DallasTemperature sensors(&oneWireSensors);
//ST7565 connected to hwSPI with  
//CS on pin A0 on pin5, CS on pin10 and reset on pin6
U8GLIB_LM6059 u8g(5, 10, 6); 

//oilcan symbol
const uint8_t sym_oil[] U8G_PROGMEM = {
  0b11110000, 0b00000010, 
  0b10010011, 0b10000111,
  0b10010001, 0b00001100,
  0b11111111, 0b11111000,
  0b01000000, 0b00001000,
  0b01011101, 0b01001001,
  0b01010101, 0b01001000,
  0b01011101, 0b01101000,
  0b01000000, 0b00001000,
  0b01111111, 0b11111000,
};
//thermometer symbol
const uint8_t sym_temp[] U8G_PROGMEM = {
  0b01110000, 
  0b11010000,
  0b01010000,
  0b11010000,
  0b01010000,
  0b11010000,
  0b01010000,
  0b10001000,
  0b10001000,
  0b01110000 
};

const uint8_t sym_bata[] U8G_PROGMEM = {
  0b00110000, 0b00011000, 
  0b00110000, 0b00011000,
  0b11111111, 0b11111110,
  0b10000000, 0b00000010,
  0b10010011, 0b10000010,
  0b10111010, 0b10111010,
  0b10010011, 0b10000010,
  0b10000010, 0b10000010,
  0b10000000, 0b00000010,
  0b11111111, 0b11111110,
};

const uint8_t sym_batv[] U8G_PROGMEM = {
  0b00110000, 0b00011000, 
  0b00110000, 0b00011000,
  0b11111111, 0b11111110,
  0b10000000, 0b00000010,
  0b10010100, 0b01000010,
  0b10111010, 0b10111010,
  0b10010010, 0b10000010,
  0b10000001, 0b00000010,
  0b10000000, 0b00000010,
  0b11111111, 0b11111110,
};

//string of the outside temperature
char outsidetemps[6];
//string of the oil temperature
char oiltemps[5];
//time seperator
byte count = 0;
//float of the average temperature
float ftemp = 94.0;
unsigned int timercapture;
unsigned int lastcapture;
unsigned int timediff;
//rpm/10 (will need to append a 0 on display)
int rpm = 0;
char rpms[4];
//battery amperage
int amper = 0;
char ampers[4];

//setup function called on reset/powerup
void setup(void)   
{
  //tachometerinput
  //pinMode(2,INPUT);
  //search for 1wire sensors and index their addresses
  sensors.begin();
  //set the resolution of sensor 0 to 9bit (ought to be enough for anybody)
  sensors.setResolutionByIndex(0,9);
  //set u8glib to 1bit b/w mode
  u8g.setColorIndex(1);
  //set a readable default font
  //  Serial.begin(9600);
  PORTB |= 1 << 0;//pullup enable 
  DDRB&=~(1<<0);//ICR1 as input // Timer1 initialisieren:
  TCCR1A = 0;                      // normal mode, keine PWM Ausgänge
  TCCR1B = (1<< ICNC1) + (1<<CS10) + (1<<CS11)+ (1 << ICES1);   
  TIMSK1 = (1<<ICIE1);   // overflow und Input-capture aktivieren, Mega32: TIMSK
}


//main loop running untill poweroff
void loop(void)
{
  //temperature buffer
  int temp;
  //read the outside temperature every 128 LCD refreshes
  if (count == 0)
  {
    count = 8;
    sensors.requestTemperaturesByIndex(0);
    dtostrf(sensors.getTempCByIndex(0),5,1,outsidetemps);
  }
  count--;

  //calc the revolutions per minute
  if (count == 5 || count == 7 || count == 3 || count == 1)
  {
    if (timediff < 20000 && timediff > 1100) 
    {
      rpm = (float) (((rpm)+(2*((1.5*1000000)/timediff)))/3.0);
      //rpm = (float) (0.857143*rpm)+(214286/timediff);
    } 
    else {
      rpm = (rpm*0.6);
    }
    if (rpm == 0)
    {
      rpms[0] = NULL;
    }  
    else {

      itoa(rpm,rpms,10);
    }
  }

  if (count == 4) 
  {
    amper = (float) (((amper*3.0)+analogRead(5))/4.0);
    itoa(map(amper,512,943,0,75),ampers,10);
  }

  if (count == 6)
  {
    temp = analogRead(0);
    //create moving average of the read temperature
    ftemp = ((ftemp*5.0)+temp)/6.0;
    //convert the read temperaturevalue to celsius
    temp = (int) map(ftemp, 128, 148, 16, 22 );
    //make string of temp
    itoa(temp,oiltemps,10);
  }

  //loop through all 8 segments and render them
  u8g.firstPage();
  do {
    draw();
  } 
  while(u8g.nextPage());
}

//This Functions will draw the current page (in segments by u8g.currentPage())
//it also pushes them via SPI to the ST7565
void draw(void) 
{
  //outlines
  u8g.drawLine(0,34,127,34);
  u8g.drawLine(0,35,127,35);
  u8g.drawLine(63,36,63,63);
  u8g.drawLine(64,36,64,63);
  u8g.drawLine(0,49,127,49);
  u8g.drawLine(0,50,127,50);
  //symbols
  u8g.drawBitmapP( 5, 38, 2, 10, sym_bata);
  u8g.drawBitmapP( 70, 38, 2, 10, sym_batv);
  u8g.drawBitmapP( 5, 53, 2, 10, sym_oil);
  u8g.drawBitmapP( 75, 53, 1, 10, sym_temp);
  //temps and "°C"
  //u8g.setFontPosBaseline();
  u8g.setFont(u8g_font_freedoomr25n);
  u8g.drawStr(72-(u8g.getStrWidth(rpms)), 32, rpms);
  u8g.drawStr(72, 32, "0");
  u8g.setFont(u8g_font_7x14);
  u8g.drawStr(97, 31, "rpm");
  u8g.drawStr(45-(strlen(ampers)*7), 48, ampers);
  u8g.drawStr(51, 48, "A");  
  u8g.drawStr(45-(strlen(oiltemps)*7), 63, oiltemps);
  u8g.drawStr(44, 63, "\xb0""C");
  u8g.drawStr(78, 63, outsidetemps);
  u8g.drawStr(112, 63, "\xb0""C");
}

ISR(TIMER1_CAPT_vect)
{
  //read ICR1
  timercapture = ICR1;
  //calculate the last period
  timediff = timercapture - lastcapture; 
  //keep current measurement
  lastcapture = timercapture;
}


