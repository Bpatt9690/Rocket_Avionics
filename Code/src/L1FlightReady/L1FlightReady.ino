#include <SFE_BMP180.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

File flightLog;
SFE_BMP180 pressure;

int buzzer = 6, ejection = 2;
double baselineP = 0, baselineA = 0, filter, baselineT;
double aCorrect = 0, apogee = 0, apTemp = 0;

void setup(void)
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

double LPA(double alt)
{
  filter = ((0.20 *filter) + (.80 * alt));
  return filter;
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

  baselineP = average;

  flightLog.print("\n\nBaseline Barometric Pressure: ");

  flightLog.print(average);

  flightLog.print("\n\n");

  flightLog.close();

  getBaseAlt();

}

void getBaseAlt(void)
{
  double P, a, total = 0, average = 0,filteredAlt = 0;
  int i = 0;
  float temp = 0;

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
  int flightFlag = 0;
  double P, a, filteredAlt = 0;

  flightLog = SD.open("flight.txt", FILE_WRITE);

  flightLog.print("LaunchPad\n\n");

  if(baselineA > 0)
    aCorrect = baselineA * -1;

  else if(baselineA <= 0)
    aCorrect = baselineA;

   while(1)
   {
    P = getPressure(0);

    a = pressure.altitude(P,baselineP);

    filteredAlt = LPA(a+aCorrect);

    flightLog.println(filteredAlt);

    if(filteredAlt > (baselineA + 3)) 
      flightFlag += 1;

    else if(filteredAlt < (baselineA + 3)) 
      flightFlag = 0;

    if(flightFlag >= 10)
    {
      flightLog.close();
      flight();
    }
    delay(10);
   }
}

void flight()
{
  double P, a, filteredAlt = 0, alt = 0;
  int aFlag = 0;

  flightLog = SD.open("flight.txt", FILE_WRITE);

  flightLog.print("\n\n\t\tFlight Altitude\n\n");

  alt = baselineA + 3;

  while(1)
  {
    P = getPressure(0);

    a = pressure.altitude(P,baselineP);

    filteredAlt = LPA(a+aCorrect);

    flightLog.println(filteredAlt);

    if(filteredAlt > alt)
    {
      alt = filteredAlt;

      aFlag = 0;
    }

    else if(filteredAlt <= alt - 3)
      aFlag +=1;

    if(aFlag  >= 10)
    {
      apogee = alt;

      apTemp = getPressure(1);

      flightLog.close();

      recovery();
    }
    delay(10);
  }
}

void recovery(void)
{
  int i = 0;

  flightLog = SD.open("flight.txt", FILE_WRITE);

  flightLog.print("\n\nApogee: ");

  flightLog.println(apogee);

  flightLog.print("\n\nTemp at Apogee: ");

  flightLog.println(apTemp);

  pinMode(ejection,OUTPUT);

  while(i <= 5)
  {
    digitalWrite(ejection,HIGH);
    delay(2000);
    digitalWrite(ejection,LOW);
    delay(500);
    i++;
  }

  flightLog.print("\n\nParachute Deployment\n\n");

  flightLog.close();

  while(1);
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
