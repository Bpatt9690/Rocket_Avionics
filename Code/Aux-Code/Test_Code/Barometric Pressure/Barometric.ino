#include <SFE_BMP180.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

File flightLog;
SFE_BMP180 pressure;

int buzzer = 6;
double baselineP = 0, baselineA = 0, filter, baselineT;
double aCorrect = 0;


void setup()
{
  int i = 0;

  Serial.begin(9600);

  while(!Serial)
  {
    ;
  }

  if(!SD.begin(10))
    while(1);

  flightLog = SD.open("flight.txt", FILE_WRITE);

  if(!(flightLog))
    while(1);

  flightLog.println("Serial connection stable");
  
  flightLog.println("Sd card connection stable");

  if(!(pressure.begin()))
    while(1);

  pinMode(buzzer,OUTPUT);
  
  while(i < 3)
  {
    digitalWrite(buzzer,HIGH);
    delay(300);
    digitalWrite(buzzer,0);
    delay(300);
    i++;
  }
  
  getBaseLine();
}

void getBaseLine(void)
{

 float average = 0, total = 0,baseline = 0;
 int i = 0;
 double P, a;

  flightLog.println("\nDeterming Baseline Barometric Pressure\n");

  while(i < 10)
  {
    baseline = (getPressure(0))/10;
    flightLog.println(baseline);
    delay(1000);

    total += baseline;
    i++;
  }
  
  average = (total/i)*10;

  flightLog.print("\n\nBaseline Barometric Pressure: ");
  
  flightLog.print(average);

  flightLog.print("\n\n");

       
  flightLog.close();

  baselineP = average;

  getBaseAlt();

}

double LPA(double alt)
{

  filter = ((0.20 *filter) + (.80 * alt));
  return filter;
  
}

void getBaseAlt(void)
{


  double P, a, total = 0, average = 0,T,filteredAlt = 0;
  int i = 0;
  float temp;

  delay(1000);

  flightLog = SD.open("flight.txt", FILE_WRITE);

  flightLog.print("\n\nDetermining Baseline Altitude\n\n");

  while(i < 10)
  {

    P = getPressure(0);
   
    a = pressure.altitude(P,baselineP);

    if(i >= 1 && i < 10)
    {

      filteredAlt = LPA(a);

      flightLog.println(filteredAlt);

      total += filteredAlt;
  
      delay(1000);

      i++;
     
    }

    else if(i < 1)
    {
      filteredAlt = LPA(a);
      i++;
      delay(1000);
    }

    i++;
        
  }
 
  average = total / 9;

  baselineA = average;

  flightLog.print("\nBaseline Alt: ");
  flightLog.println(baselineA);

  flightLog.print("Baseline Temp: ");

  baselineT = getPressure(1);

  flightLog.print(baselineT);

  flightLog.print("\n\n");

  delay(1000);

  flightLog.close();

  launchPad();
}

void launchPad(void)
{

  int j = 0, i = 0, flightFlag = 0;

  double P, a, filteredAlt = 0;

  flightLog = SD.open("flight.txt", FILE_WRITE);
  
  flightLog.print("LaunchPad\n\n");

  Serial.print("LaunchPad");

  if(baselineA > 0)
    aCorrect = baselineA * -1;

  else if(baselineA < 0)
    aCorrect = baselineA;

   flightLog.print("Correction : ");
   flightLog.println(aCorrect);
   flightLog.print("\n\n");


   while(j < 10000)
   {

    P = getPressure(0);

    a = pressure.altitude(P,baselineP);

    flightLog.print("Relative Alt: ");
    flightLog.println(a);

    filteredAlt = LPA(a+aCorrect);

    flightLog.print("Relative Filtered Alt: ");
    flightLog.println(filteredAlt);

    flightLog.print("\n\n");

    if(filteredAlt > (baselineA + 4))
      flightFlag++;

    else if(filteredAlt < (baselineA + 4))
      flightFlag = 0;

   /* if(flightFlag >= 10)
    {
      flightLog.close();
      flight();
    }
    */
     
    j++;
    delay(100);    
   }

   flightLog.close();
   
  while(i < 3)
  {
    digitalWrite(buzzer,HIGH);
    delay(300);
    digitalWrite(buzzer,0);
    delay(300);
    i++;
  }
   while(1);

  
}

void flight()
{

  double P, a;
  double filteredAlt = 0;

  flightLog = SD.open("flight.txt", FILE_WRITE);

  flightLog.print("Flight function\n");

  flightLog.print("Correction : ");
  flightLog.println(aCorrect);


  flightLog.print("\n\nFlight Altitude\n\n");
  
  while(1)
  {

   P = getPressure(0);

   a = pressure.altitude(P,baselineP);

   filteredAlt = LPA(a+aCorrect);


   flightLog.print("Relative Alt: ");
   flightLog.println(a);


   flightLog.print("Relative Filtered Alt: ");
   flightLog.println(filteredAlt);
      
 
  }

  
}

double getPressure(int x)
{
  char status;
  double T, P, p0, a;
  float apogeeT;

  
  if(x == 1)
  {


      status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {  

      apogeeT = ((9.0/5.0) * T+32.0);
      return apogeeT;
            
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          ;
        }
      }
    }
   }
  }

  else if(x != 1)
  {

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {  
            
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return (P);
        }
      }
    }
  }

  }
}
