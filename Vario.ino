#include <SFE_BMP180.h>
#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SFE_BMP180 pressure;

#define BUZZERPIN 6
#define PAUSE 250
#define ALTITUDE 337.0
#define SINK_ASK13 -1.0
#define GLIDERATIO_ASK13 27

int VARIO_STATE = 0;
boolean VARIO_ON=false;

int ALT_STATE =0;
boolean ALT_ON=false;

int RANGE_STATE =0;
boolean RANGE_ON=false;

int SOUND_STATE =0;
boolean SOUND_ON=false;

int btnVario = 8;
int btnAlt = 9;
int btnRange = 10;
int btnSound =7; 


int lastButtonState = 0; 
int buttonPushCounter = 0; 

float a_vario1 = 0;
float vario = 0;

float range;

void setup()
{
  Serial.begin(9600);
  Serial.println("BOOT");

  //init LCD
  lcd.begin(16,2);
  
  // Initialize the sensor
  if (pressure.begin())
  {
    lcd.print("BMP180 init");
    lcd.setCursor(0,1);
    lcd.print("successful");
    delay(PAUSE);
  }
  else
  {
    lcd.print("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
 
  pinMode(btnVario, INPUT);
  pinMode(btnAlt, INPUT);
  pinMode(btnRange, INPUT);
  pinMode(btnSound, INPUT);

  
  lcd.home();
  lcd.print("Buttons init ");
  lcd.setCursor(0,1);
  lcd.print("successful");
  delay(PAUSE);
  lcd.clear();
}


void loop()
{
  char status,statusvario;
  double T,P,p0,a,a_vario,vario,p0_vario,P_vario;

  lcd.home();

/////CALC///////////
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    

    status = pressure.getTemperature(T);
    if (status != 0)
    {     
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        
        if (status != 0)
        {
          // relative / sea level pressure
          p0 = pressure.sealevel(P,ALTITUDE); 
          
          // altitude m/ft
          a = pressure.altitude(P,p0);   
        }
        else lcd.print("error pressure");
      }
      else lcd.print("error pressure");
    }
    else lcd.print("error temperature ");
  }
  else lcd.print("error temperature");


////////VARIO////////
  statusvario = pressure.startPressure(3);
  if(statusvario != 0)
  {
    delay(statusvario);  
    statusvario = pressure.getPressure(P_vario,T);
    
    if (statusvario != 0)
    {
      p0_vario = pressure.sealevel(P_vario,ALTITUDE);
      a_vario = pressure.altitude(P_vario,p0_vario);

      vario = a_vario - a;
    } else lcd.print("error vario");
    
  } else lcd.print("error vario");


//////////beep beep im a sheep ////////
// go up, stay up
    if((vario /*+ SINK_ASK13*/) >= 0)
    {
      tone(BUZZERPIN,(2000+(250*vario)),300-(vario*80));
      delay(PAUSE/(vario+1));
    }
// go down, stay down
    if((vario /*+ SINK_ASK13*/) < 0)
    {
      tone(BUZZERPIN,(950+(125*vario)),125);
      delay(PAUSE);
    }

  
////////BUTTON VARIO///////
  VARIO_STATE = digitalRead(btnVario);

    if(VARIO_STATE == HIGH)
    {
      if(VARIO_ON==true)
      {
        VARIO_ON = false;
      }
      else
      {
        VARIO_ON = true;
      }

  while(VARIO_ON == true)
  {    
    //print absolute pressure
          lcd.clear();
          lcd.home();
          lcd.print("vario: ");
          lcd.setCursor(0,1);
          lcd.print(vario,2);
          lcd.print(" m/s");
        
    ALT_STATE = digitalRead(btnAlt);
    if(ALT_STATE == HIGH)
    {
      break;
    }
    RANGE_STATE = digitalRead(btnRange);
      if(RANGE_STATE == HIGH)
    {
      break;
    }
    delay(PAUSE);
  }
 }
////////BUTTON ALTITUDE///////
  ALT_STATE = digitalRead(btnAlt);
   
    if(ALT_STATE == HIGH)
    {
      if(ALT_ON==true)
      {
        ALT_ON = false;
      }
      else
      {
        ALT_ON = true;
      }

  while(ALT_ON == true)
  { 
      //print altitude
          lcd.clear();
          lcd.home();
          lcd.print(a,0);
          lcd.print(" meters");
          
          lcd.setCursor(0,1);
          lcd.print(a*3.28084,0);
          lcd.print(" feet");

    RANGE_STATE = digitalRead(btnRange);
    if(RANGE_STATE == HIGH)
    {
      break;
    }
    VARIO_STATE = digitalRead(btnVario);
      if(VARIO_STATE == HIGH)
    {
      break;
    }
    delay(PAUSE);   
   }

 }

////////BUTTON RANGE///////
  RANGE_STATE = digitalRead(btnRange);

    if(RANGE_STATE == HIGH)
    {
      if(RANGE_ON==true)
      {
        RANGE_ON = false;
      }
      else
      {
        RANGE_ON = true;
      }

  while(RANGE_ON == true)
  {
    //(max) Gleitzahl, Eigensinken m/s:
    // PW-5 = 1/32
    // Ka13 = 1/27 , 1
    // DG300 = 1/41
    // DG500 = 1/40
    
      //calc Range KA13 (bei geringstem Sinken)
      if(a_vario <= ALTITUDE)
      {
        range = 0;
      }
      else
      {
      range = ((a_vario /*+ SINK_ASK13*/ - ALTITUDE)*GLIDERATIO_ASK13)/1000;
      }
      
      //print Range
          lcd.clear();
          lcd.home();
          lcd.print("curr. max Range:");
          lcd.setCursor(0,1);
          lcd.print(range,2);
          lcd.print(" km");

    ALT_STATE = digitalRead(btnAlt);
    if(ALT_STATE == HIGH)
    {
      break;
    }
    VARIO_STATE = digitalRead(btnVario);
      if(VARIO_STATE == HIGH)
    {
      break;
    }
    delay(PAUSE);
    }

 }

////////BEEP BEEP IM A SHEEP////////////
//tone(BUZZERPIN,1000,500);

  delay(PAUSE);
}
