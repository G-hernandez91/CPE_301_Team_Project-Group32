
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
//
//--------------------------------------------------------------------
#include <LiquidCrystal.h>    // For LCD display
#include <dht_nonblocking.h>   // For Temp. sensor
// 

//as-built circuit inputs as follows:
//
//blue LED: PA0 (Dig Pin 22) (OUTPUT)
//green LED: PA1 (Dig Pin 23) (OUTPUT)
//yellow LED: PA2 (Dig Pin 24) (OUTPUT)
//red LED: PA3 (Dig Pin 25) (OUTPUT)
//
//ball switch: PA6 (Dig Pin 28) (INPUT)
//DHT22: PA7 (Dig Pin 29) (INPUT)
//
//LCD RS: PC1 (Dig Pin 36) (OUTPUT)
//LCD enable: PC0 (Dig Pin 37) (OUTPUT)
//LCD data: PC2:PC5 (Dig Pin 35:32) (labeled backwards on the arduino for no good reason)
//
//push button: PC7 (Dig Pin 30) (INPUT)
//
//vent pot: PF0/ADC0 (Analog Pin 0) (INPUT)
//
//NEED: update port register definitions, etc. to reflect built circuit
//NEED: ADD/FIGURE OUT STEPPER FOR VENT AND DC MOTOR FOR FAN

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

// TEMP. SENSOR
#define DHT_SENSOR_TYPE DHT_TYPE_11
static const int DHT_SENSOR_PIN = 29;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

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
 
  //intilize 
   
  *port_b &= 0xFF;
  
  //adc_init();                 //Setup the ADC

  // Setup LCD
  lcd.begin(16, 2);
  lcd.print("Display Data");
  lcd.clear();
}

void loop()
{
  //NEED: to determine how to handle low water error condition. Could use an interupt, or poll state of ball switch pin
  //NEED: to determine how to handle enabled/disabled condition. Could use an interupt, or poll state of push button pin
 
  //detect whether disabled
  //state is diabled: light yellow LED, wait until enabled

   //then detect whether water level too low (error)
   //state is error: light red LED, wait until water level restored

    //then detect whether temperature is below threshold... (idle)
    //state is idle: light green LED, turn off motor
    
    //...or detect whether temperature is above threshold (running)
    //state is running: light blue LED, turn on fan motor
  
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

//NEED: subroutine to light LED. Takes desired pin, return void (writes pin high)

//NEED: push button handler subroutine. The push button should force state change to disabled (or from disabled to idle)

//NEED: vent motor subroutine. Takes a normalized input value (which will have originated by digital conversion
//of the potentiometer), drives stepper motor to specific location based on that value, return void

//NEED: fan motor subroutine. Should drive fan motor if/only if state is "running." No values passed, rerturn void (drives fan motor)

//NEED: LCD ini and print subroutines:
  //LCD ini should wrap initilization into a function call, take LCD size as parameter and return void
  //LCD print should be able to print chars passed to it. We will need to call this to print temp/humidity and the error message

//NEED: ADC ini and read subroutines:
  //ADC ini should initilize the ADC
  //ADC read should return a normalized integer value from the voltage output of the potentiometer (0V-5V). This value will be used to
  //modulate to position of the stepper motor for the vent

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
