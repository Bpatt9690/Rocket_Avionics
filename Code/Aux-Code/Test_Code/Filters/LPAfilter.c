#include <stdio.h>
#include <time.h>

double value = 0;


void LPA(double x)
{

  value = ((0.30)*value + (.70) * (x));
  printf("%lf\n", value);
}


int main(void)
{

  double x;

  while(1)
  {
    printf("Number : ");
    scanf("%lf",&x);
    LPA(x);
  }
  return 0;
}
