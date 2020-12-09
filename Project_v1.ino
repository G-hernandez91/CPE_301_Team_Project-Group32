
//--------------------------------------------------------------------
// File Name:       Project_v1.ino
// Description:     Group Project code(Evapotation Cooling System)
// Compiler:        Arduino 1.8.13
//--------------------------------------------------------------------
// Name:                      Version:                    Date:
// Guillermo Hernandez        1.0 Created file            12/04/2020
// Anthony Dyer               1.1 Added/Fixed port disc.  12/08/2020
// Guillermo Hernandez        1.2 Worked on code          12/08/2020
// Anthony Dyer               1.3 Worked on code          12/08/2020
// Anthony Dyer               1.4 Added Implemintation    12/09/2020
// Guillermo Hernandez        1.5 worked on code          12/09/2020
//--------------------------------------------------------------------
#include <LiquidCrystal.h>    // For LCD display
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
//#include <dht.h>   // For Temp. sensor
//dht DHT;

//as-built circuit inputs as follows:
//
//blue LED: PA0 (Dig Pin 22) (OUTPUT)
//green LED: PA1 (Dig Pin 23) (OUTPUT)
//yellow LED: PA2 (Dig Pin 24) (OUTPUT)
//red LED: PA3 (Dig Pin 25) (OUTPUT)
//
//ball switch: PA6 (Dig Pin 28) (INPUT)
//DHT11: PA7 (Dig Pin 29) (INPUT)
//
//LCD RS: PC1 (Dig Pin 36) (OUTPUT)
//LCD enable: PC0 (Dig Pin 37) (OUTPUT)
//LCD data: PC2:PC5 (Dig Pin 35:32) (labeled backwards on the arduino for no good reason)
//
//push button: PC7 (Dig Pin 30) (INPUT)
//
//vent pot: PF0/ADC0 (Analog Pin 0) (INPUT)
//
//DC motor command: PF3 (Analog Pin 2) (OUTPUT)
//NEED: ADD/FIGURE OUT STEPPER FOR VENT

// Definitions:

//PORT DEFINITIONS
volatile unsigned char* port_a = (unsigned char*) 0x22;
volatile unsigned char* ddr_a  = (unsigned char*) 0x21;
volatile unsigned char* pin_a  = (unsigned char*) 0x20;

volatile unsigned char* port_c = (unsigned char*) 0x28;
volatile unsigned char* ddr_c  = (unsigned char*) 0x27;
volatile unsigned char* pin_c  = (unsigned char*) 0x26;

volatile unsigned char* port_f = (unsigned char*) 0x31;
volatile unsigned char* ddr_f  = (unsigned char*) 0x30;
volatile unsigned char* pin_f  = (unsigned char*) 0x2F;

// ADC
volatile unsigned char* my_ADMUX   = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB  = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA  = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*)  0x78;

// LCD
LiquidCrystal lcd (36,37,35,34,33,32);

void setup()
{
  //initilize data directions
  *ddr_a = 0x0F;
  *ddr_c = 0x3F;
  *ddr_f = 0x0F;
  
  //initilize ADC

  //initilize LCD
  LCD_init();
}

void loop()
{
  //NEED: to determine how to handle low water error condition. Could use an interupt, or poll state of ball switch pin
  //NEED: to determine how to handle enabled/disabled condition. Could use an interupt, or poll state of push button pin

  float threshold = 20;
  float water_level = 25;
  float water_threshold = 0;
  
  //state is diabled: light yellow LED, wait until enabled
  button_switch(*port_c);

  //state is error: light red LED, wait until water level restored
  while(water_level <= water_threshold)
  {
    //light red LED
    write_pa(3,1);

    //print error message
    lcd.clear();
    lcd.print("     ERROR!     ");
    lcd.setCursor(0,1);
    lcd.print("   LOW WATER!   ");
    //NEED: wait for water level to be normal again
  }
  
  //pulling humidity and temp data using functions outlined below
  String dht_str; //I am bad and I feel bad 
  byte hum1, hum2, temp1, temp2;
  dht_req(7);
	if(dht_resp(7) != 1){
    hum1 = dht_data(7);
    hum2 = dht_data(7);
    temp1 = dht_data(7);
    temp2 = dht_data(7);
  }
  //temperature = 0;
  //humidity = 0;

  //print temp and humidity
  dht_str = String(temp1);
  lcd.print( "T = " );
  lcd.print(dht_str);
  lcd.print(".");
  dht_str = String(temp2);
	lcd.print(dht_str);
  lcd.print( " deg. C," );

  lcd.setCursor(0,1);
  dht_str = String(hum1);
  lcd.print( "H = " );
  lcd.print(dht_str);
  lcd.print(".");
  dht_str = String(hum2);
	lcd.print(dht_str);
  lcd.print( "%" );
  delay(500);
  lcd.clear();
  _delay_ms(10);

  //state is idle: light green LED, turn off motor
  while(0 <= threshold)
  {
    //light green LED and reset others
    write_pa(0,0);
    write_pa(1,1);
    write_pa(2,0);
    write_pa(3,0);

    //disable fan motor
    write_pf(2,0);
    break;
  }
  
  //state is running: light blue LED, turn on fan motor
  while(0 > threshold)
  {
    //light blue LED and reset others
    write_pa(0,1);
    write_pa(1,0);
    write_pa(2,0);
    write_pa(3,0);

    //enable fan motor
    write_pf(2,1);
    break;
  }

}

//subroutine to light LED. Takes desired pin, return void (changes pin state)
void write_pa(unsigned char pin, unsigned char state)
{
  if(state == 0)
  {
    *port_a &= ~(0x01 << pin);
  }
  else
  {
    *port_a |= 0x01 << pin;
  }
  
}

void write_pf(unsigned char pin, unsigned char state)
{
  if(state == 0)
  {
    *port_f &= ~(0x01 << pin);
  }
  else
  {
    *port_f |= 0x01 << pin;
  }
  
}

void write_pc(unsigned char pin, unsigned char state)
{
  if(state == 0)
  {
    *port_c &= ~(0x01 << pin);
  }
  else
  {
    *port_c |= 0x01 << pin;
  }

}
//NEED: push button handler subroutine. The push button should force state change to disabled (or from disabled to idle)
<<<<<<< HEAD
void button_switch(unsigned int sub_switch)
{
   unsigned int sub_switch = 0;

  if(sub_switch == 1)
=======
void button_switch(unsigned char _switch)
{
   unsigned int sub_switch = 0;
  
  if(_switch == 1)
>>>>>>> 786d35de7fd8d79cde882adc8ba30c064064728e
  {  
    sub_switch = _switch;
  }
  _delay_ms(10);

  while( sub_switch == 1)
  {
    write_pa(0,0);
    write_pa(1,0);
    write_pa(2,1);
    write_pa(3,0);

    if(sub_switch == 1)
    {
    sub_switch = 0; 
    }
  }

    write_pa(0,0);
    write_pa(1,1);
    write_pa(2,0);
    write_pa(3,0);
}



//NEED: vent motor subroutine. Takes a normalized input value (which will have originated by digital conversion
//of the potentiometer), drives stepper motor to specific location based on that value, return void

//DHT request
void dht_req(unsigned char pin_num)
{
	*ddr_a |= (1<<pin_num);
	*port_a &= ~(1<<pin_num);	
	_delay_ms(20);
	*port_a |= (1<<pin_num);
}

//DHT response handler
int dht_resp(unsigned char pin_num)
{
	*ddr_a &= ~(1<<pin_num);
  _delay_us(40);
  if((*pin_a & (1<<pin_num)))
  {
    return 1;
  }
  _delay_us(80);
  if(!(*pin_a & (1<<pin_num)))
  {
    return 1;
  }
  _delay_us(80);
  return 0;
}

//dht data setup
byte dht_data(unsigned char pin_num)
{	
  byte dat = 0;
	for (int i=0; i<8; i++)
	{
		while((*pin_a & (1<<pin_num)) == 0);
		_delay_us(30);
		if(*pin_a & (1<<pin_num)) //if a high pulse is sent after 30ms
    {
		  dat = (dat<<1)|(0x01); //shift a 1 into dat
    }
    else
    {
      dat = (dat<<1); //else shift to make a 0
    }
		while(*pin_a & (1<<pin_num));
	}
	return dat;
}

//LCD ini: wrap initilization into a function call, return void
void LCD_init()
{
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
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
