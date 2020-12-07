
//--------------------------------------------------------------------
// File Name:       Project_v1.ino
// Description:     Group Project code(Evapotation Cooling System)
// Compiler:        Arduino 1.8.13
//--------------------------------------------------------------------
// Name:                      Version:                    Date:
// Guillermo Hernandez        1.0 Created file            12/04/2020
//
//--------------------------------------------------------------------
#include <LiquidCrystal.h>    // For LCD display
#include <dht_nonblocking.h>   // For Temp. sensor
// 

// Definitions:

// TEMP. SENSOR
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 34;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );


// LEDS
volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* ddr_b  = (unsigned char*) 0x24;
volatile unsigned char* pin_b  = (unsigned char*) 0x23;

// ADC
volatile unsigned char* my_ADMUX   = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB  = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA  = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*)  0x78;

// LCD
LiquidCrystal lcd (22, 23, 24, 25, 26, 27);

void setup()
{
  *ddr_b |= 0x40;
  *port_b &= 0xFF;
  
  //adc_init();                 //Setup the ADC

  // Setup LCD
  lcd.begin(16, 2);
  lcd.print("Display Data");
  lcd.clear();
}

void loop()
{
  
  
  float temperature;
  float humidity;
  lcd.print( "T = " );
  lcd.print( temperature, 1 );
  lcd.print( " deg. C, H = " );
  lcd.print( humidity, 1 );
  lcd.print( "%" );
  delay(500);
  lcd.clear();

}


void adc_init()
{
  *my_ADCSRA |= 0x80;       // Set bit 7
  *my_ADCSRA &= 0xDF;       // Clear bit 5 = 0
  *my_ADCSRA &= 0xF6;       // Clear bit 3 = 0
  *my_ADCSRA &= 0xF8;       // Clear bit 2-0 = 0

  *my_ADCSRB &= 0xF6;       // Clear bit 3 = 0
  *my_ADCSRB &= 0xF8;       // Clear bit 2-0 = 0

  *my_ADMUX &= 0x7F;        // Clear bit 7 = 0
  *my_ADMUX |= 0x40;        // Set bit 6 = 1
  *my_ADMUX &= 0xDF;        // Clear bit 5 = 0
  *my_ADMUX &= 0xE0;        // Clear bit 4-0 = 0
}

unsigned int adc_read(unsigned char adc_channel_num)
{
  *my_ADMUX &= 0xE0;                  // Clear channel bit MUX 4:0

  *my_ADCSRB &= 0xF6;                 // Clear channel bit MUX 5 (bit 3)

  if (adc_channel_num > 7)            // Set channel number
  {

    adc_channel_num -= 8;             // Set the channel selection bits, but remove the most sig. bit (bit 3)

    *my_ADMUX |= 0x20;                // Set MUX bit 5
  }

  *my_ADMUX += adc_channel_num;       // Set the channel selection bits

  *my_ADCSRA |= 0x40;                 // Set bit 6 of ADCSRA = 1

  while ((*my_ADCSRA & 0x40) != 0);   // Wait for the conversion to finish


  return *my_ADC_DATA;                // Return the results in the ADC data register
}
