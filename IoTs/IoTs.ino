// ###################################################################
#define BLYNK_PRINT Serial
// Blynk Cloud ID to connect
#define BLYNK_TEMPLATE_ID "TMPLc1hEL9yS"
// Include needed library
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <TimeLib.h>
// Temperature and humidity 
DHT dht;
// Blynk Setup
char auth[] = "tNg710OafaXPWxoEf-4T3JEpEDY-Hds-";
// WiFi Setup.
/*
char ssid[] = "Bảo Anh";
char pass[] = "baoanh2103";
*/
char ssid[] = "iPhone (2)";
char pass[] = "MinhDuc12";
#define BLYNK_DEVICE_NAME "ASSIGNMENTSMARTCAGE"
// ###################################################################

long rtc_sec_server;                                 // second counter
unsigned char weekday_server;                        // weekday get from Blynk Server
// ###################################################################
// Define Pump for purpose
#define WASHING_PUMP_OUT             D1
#define FEEDING_PUMP_OUT             D2
#define DHT_READ_PIN                 D3
// State of pump
#define ON                           0
#define OFF                          1
// Blynk coomunicate pin set up
#define BLYNK_TEMP              V0
#define BLYNK_RH                V1
#define BLYNK_TIMER_1           V3
#define BLYNK_TIMER_2           V4
#define BLYNK_PUMP_1           V5
#define BLYNK_PUMP_2           V6
#define BLYNK_EN_TIMER_1        V7
#define BLYNK_EN_TIMER_2        V8
// ###################################################################
// Variables for temperature and humidity
float humidity; 
float temperature; 
// ###################################################################
// Time and timer set up
// Pump 1
unsigned char start_time_hour_1;
unsigned char start_time_min_1;
unsigned char stop_time_hour_1;
unsigned char stop_time_min_1;
unsigned char day_timer_1;
//Timerflag for pump 2
bool flag_timer1_en;
bool flag_timer_on_pump_1;
// Pump 2
unsigned char start_time_hour_2;
unsigned char start_time_min_2;
unsigned char stop_time_hour_2;
unsigned char stop_time_min_2;
unsigned char day_timer_2;
// Timerflag for pump 2
bool flag_timer2_en;
bool flag_timer_on_pump_2;
//  Set flag's variable
bool flag_pump_1_set;
bool flag_pump_2_set;
// Set flag's status
bool flag_pump_1_status;
bool flag_pump_2_status;
// Set variable for Blynk
bool flag_BLYNK_PUMP_1_update;
bool flag_BLYNK_PUMP_2_update;
// Blynk
bool flag_blynk_guage_update;
// Real time clock synchronization
bool rtc_synchronized;

// ######################################################################
BLYNK_CONNECTED()
{
  Serial.print("BLYNK SERVER CONNECTED !!!");
 // Blynk.syncAll();
  Blynk.syncVirtual(BLYNK_TIMER_1);
  Blynk.syncVirtual(BLYNK_TIMER_2);
  Blynk.syncVirtual(BLYNK_EN_TIMER_1);
  Blynk.syncVirtual(BLYNK_EN_TIMER_2);
  Blynk.syncVirtual(BLYNK_PUMP_1);
  Blynk.syncVirtual(BLYNK_PUMP_2);

  Blynk.sendInternal("rtc", "sync");
}

// ######################################################################
BLYNK_WRITE (BLYNK_PUMP_1)
{
  int val = param.asInt();  // assigning incomming value from pin to a var

  if ( flag_timer_on_pump_1 == 0 )
    flag_pump_1_set = val;
  else
    flag_BLYNK_PUMP_1_update = 1;
  
  Serial.print("Valve 1 Set: ");
  Serial.println(val);
}

// ######################################################################
BLYNK_WRITE (BLYNK_PUMP_2)
{
  int val = param.asInt();  
  if ( flag_timer_on_pump_2 == 0)
    flag_pump_2_set = val;
  else
    flag_BLYNK_PUMP_2_update = 1;
  
  Serial.print("Valve 2 Set: ");
  Serial.println(val);
}

// ######################################################################
BLYNK_WRITE (BLYNK_EN_TIMER_1)
{
  int val = param.asInt();  
  flag_timer1_en = val;
  Serial.println("Timer 1 EN: " + String(flag_timer1_en));
}

// ######################################################################
BLYNK_WRITE (BLYNK_EN_TIMER_2)
{
  int val = param.asInt();  
  flag_timer2_en = val;
  Serial.println("Timer 2 EN: " + String(flag_timer2_en));
}

// ######################################################################
BLYNK_WRITE(BLYNK_TIMER_1)
{
  unsigned char week_day;
  
  TimeInputParam  t(param);
  
  if (t.hasStartTime() && t.hasStopTime() )
  {
     start_time_hour_1 = t.getStartHour();
     start_time_min_1 = t.getStartMinute();
     Serial.println(String("Time1 Start: ") +  start_time_hour_1 + ":" +  start_time_min_1);
    
     stop_time_hour_1 = t.getStopHour();
     stop_time_min_1 = t.getStopMinute();
     Serial.println(String("Time1 Stop: ") +  stop_time_hour_1 + ":" +  stop_time_min_1);
    
     for (int i = 1; i <= 7; i++)
     {
       if (t.isWeekdaySelected(i))  
       {
         day_timer_1 |= (0x01 << (i-1));
       }
       else
         day_timer_1 &= (~(0x01 << (i-1)));
     }
    
     Serial.print("Time1 Selected Days: ");
     Serial.println(day_timer_1, HEX);
    // flag_timer1_en = 1;
  }
  else
  {
   // flag_timer1_en = 0;
    Serial.println("Disabled Timer 1");
  }
}

// ######################################################################
BLYNK_WRITE(BLYNK_TIMER_2)
{
  unsigned char week_day;
  
  TimeInputParam  t(param);
  
  if (t.hasStartTime() && t.hasStopTime())
  {
     start_time_hour_2 = t.getStartHour();
     start_time_min_2 = t.getStartMinute();
     Serial.println(String("Time2 Start: ") +
                     start_time_hour_2 + ":" +
                     start_time_min_2);
    
     stop_time_hour_2 = t.getStopHour();
     stop_time_min_2 = t.getStopMinute();
     Serial.println(String("Time2 Stop: ") +
                     stop_time_hour_2 + ":" +
                     stop_time_min_2);
    
     for (int i = 1; i <= 7; i++)
     {
       if (t.isWeekdaySelected(i))  
       {
         day_timer_2 |= (0x01 << (i-1));
       }
       else
         day_timer_2 &= (~(0x01 << (i-1)));
     }
    
     Serial.print("Time1 Selected Days: ");
     Serial.println(day_timer_2, HEX);
   //  flag_timer2_en = 1;
  }
  else
  {
  //  flag_timer2_en = 0;
    Serial.println("Disabled Timer 2");
  }
}

// ######################################################################
BLYNK_WRITE(InternalPinRTC) 
{
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  unsigned long blynkTime = param.asLong(); 
  
  if (blynkTime >= DEFAULT_TIME) 
  {
    setTime(blynkTime);

    weekday_server = weekday();
  
    if ( weekday_server == 1 )
      weekday_server = 7;
    else
      weekday_server -= 1; 
    
    rtc_sec_server = (hour()*60*60) + (minute()*60) + second();
   
    Serial.println(blynkTime);
    Serial.println(String("RTC Server: ") + hour() + ":" + minute() + ":" + second());
    Serial.println(String("Day of Week: ") + weekday()); 
    rtc_synchronized = 1;
  }
}

// ######################################################################
void fn_valve_mng (void)
{
  bool time_set_overflow;
  long start_timer_sec;
  long stop_timer_sec;
  bool flag_timer_on_1_buf = flag_timer_on_pump_1;
  bool flag_timer_on_2_buf = flag_timer_on_pump_2;

  // VALVE 1
  time_set_overflow = 0;
  start_timer_sec = start_time_hour_1*3600 + start_time_min_1*60;
  stop_timer_sec = stop_time_hour_1*3600 + stop_time_min_1*60;

  if ( stop_timer_sec < start_timer_sec ) time_set_overflow = 1;
  
  if ( rtc_synchronized && flag_timer1_en && (((time_set_overflow == 0 && (rtc_sec_server >= start_timer_sec) && (rtc_sec_server < stop_timer_sec)) ||
        (time_set_overflow  && ((rtc_sec_server >= start_timer_sec) || (rtc_sec_server < stop_timer_sec)))) && 
        (day_timer_1 == 0x00 || (day_timer_1 & (0x01 << (weekday_server - 1) )))) )
  {
    flag_timer_on_pump_1 = 1;
  }
  else
    flag_timer_on_pump_1 = 0;

  // VALVE 2
  time_set_overflow = 0;
  start_timer_sec = start_time_hour_2*3600 + start_time_min_2*60;
  stop_timer_sec = stop_time_hour_2*3600 + stop_time_min_2*60;

  if ( stop_timer_sec < start_timer_sec ) time_set_overflow = 1;
  
  if ( rtc_synchronized && flag_timer2_en && (((time_set_overflow == 0 && (rtc_sec_server >= start_timer_sec) && (rtc_sec_server < stop_timer_sec)) ||
        (time_set_overflow  && ((rtc_sec_server >= start_timer_sec) || (rtc_sec_server < stop_timer_sec)))) && 
        (day_timer_2 == 0x00 || (day_timer_2 & (0x01 << (weekday_server - 1) )))) )
  {
    flag_timer_on_pump_2 = 1;
  }
  else
    flag_timer_on_pump_2 = 0;


 
  if ( flag_timer_on_pump_1 )
  {
    flag_pump_1_status = 1;
    flag_pump_1_set = 0;
  }
  else
  {
    flag_pump_1_status = flag_pump_1_set;
  }


  if ( flag_timer_on_pump_2 )
  {
    flag_pump_2_status = 1;
    flag_pump_2_set = 0;
  }
  else
  {
    flag_pump_2_status = flag_pump_2_set;
  }

  if ( flag_timer_on_1_buf != flag_timer_on_pump_1 )
    flag_BLYNK_PUMP_1_update = 1;

  if ( flag_timer_on_2_buf != flag_timer_on_pump_2 )
    flag_BLYNK_PUMP_2_update = 1;


digitalWrite(WASHING_PUMP_OUT, flag_pump_1_status);  
 digitalWrite(FEEDING_PUMP_OUT, flag_pump_2_status); 
}

// ######################################################################
unsigned char time_10_sec;
void checkTime() 
{
  time_10_sec++;
  if(time_10_sec >= 10)
  {
    time_10_sec = 0;
    Blynk.sendInternal("rtc", "sync"); 
  }
  humidity = dht.getHumidity(); 
  temperature = dht.getTemperature(); 

  flag_blynk_guage_update = 1;
}

// ######################################################################
void update_blynk_data(void)
{
  if ( flag_blynk_guage_update )
  {
    flag_blynk_guage_update = 0;
    Blynk.virtualWrite(BLYNK_TEMP, temperature);
    Blynk.virtualWrite(BLYNK_RH, humidity);
  }

  if ( flag_BLYNK_PUMP_1_update )
  {
    flag_BLYNK_PUMP_1_update = 0;
    Blynk.virtualWrite(BLYNK_PUMP_1, flag_pump_1_status);
  }

  if ( flag_BLYNK_PUMP_2_update )
  {
    flag_BLYNK_PUMP_2_update = 0;
    Blynk.virtualWrite(BLYNK_PUMP_2, flag_pump_2_status);
  }
}

// ######################################################################
void setup()
{
  // Debug print
  Serial.begin(9600);

  Blynk.begin("tNg710OafaXPWxoEf-4T3JEpEDY-Hds-","Bảo Anh", "baoanh2103" );
  
   // DHT initialization
  dht.setup(DHT_READ_PIN); 
    
  pinMode(WASHING_PUMP_OUT, OUTPUT);
  pinMode(FEEDING_PUMP_OUT, OUTPUT);

  digitalWrite(WASHING_PUMP_OUT, OFF);  
  digitalWrite(FEEDING_PUMP_OUT, OFF);  
}

// ######################################################################
unsigned long ms_buf;
void loop()
{
  unsigned long ms_dif = millis() - ms_buf;
  if ( ms_dif >= 1000 ) // 1 second
  {
     ms_buf = millis();
     checkTime();
  }
  
  Blynk.run();
  fn_valve_mng();
  update_blynk_data();
  delay(50);
}
