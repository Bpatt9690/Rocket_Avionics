#include <SPI.h>
#include <Wire.h>

int ejectCharge = 2, buzzer = 6;

void setup() {

  Serial.begin(9600);

  pinMode(buzzer,OUTPUT);
  pinMode(ejectCharge,OUTPUT);

  int i = 0, j = 0,d =0;

  while(i < 3)
  {
    digitalWrite(buzzer,HIGH);
    delay(300);
    digitalWrite(buzzer,0);
    delay(300);
    i++;
  }

  while(j < 30)
  {

    if(j >= 26 && j != 29)
    {
      digitalWrite(buzzer,HIGH);
      delay(300);
      digitalWrite(buzzer,0);
      delay(300);
    }

    else if(j == 29)
    {

      while(d < 3)
      {
        digitalWrite(ejectCharge,HIGH);
        delay(2000);
        digitalWrite(ejectCharge,LOW);
        delay(1000);
        d++;
      }
            
    }
 
   delay(1000);
   j++; 
  }

  while(1);
}
