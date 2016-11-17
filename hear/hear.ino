int avg = 0;
int j = 0;

int heart_avg()
{
  int _avg = 0;
  int sum = 0;
  int i =0;
  int j = 0;
  while(i<100)
  {
    while(j<10)
    {
      int b = analogRead(1);
      sum +=b;
   
      j++;
      delay(10);
    }
    _avg =+ sum/10;
    Serial.println("_avg " + String(_avg));
    i++;
  }
  Serial.println("_avg 121231 " + String(_avg));
  return _avg/100;
}
void setup() {
 Serial.begin(9600);
 delay(100);
 avg = heart_avg();
 Serial.println("avg " + String(avg));
 delay(100);
}



int a = 0;

void loop() {
  
  int b =0;
  b = analogRead(1);
  Serial.println(String(b));
  if(b > avg)
  {
    a++;  
  }
  delay(100);
  
  if(a>10)
  {
    j++;
   Serial.println("10" + String(j)) ;
   }

}
