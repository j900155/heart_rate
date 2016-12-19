#include <LGPS.h>

const byte type = 0x01;
const byte id = 0x00;

int a;
int count = 0,num = 0;  

String apple;
char sdata;
String asd;
char NS,WE;
int SoS = 0;
gpsSentenceInfoStruct info;
char buff[256];
double latitude;
double longitude;
char fix = '0';

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  //double latitude;
  //double longitude;
  int tmp, hour, minute, second;
  
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    //Serial.println(buff);
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(3, GPGGAstr);
    NS = GPGGAstr[tmp];
    //Serial.println("NS1 " + String(NS) );
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(5, GPGGAstr);
    WE = GPGGAstr[tmp];
    //Serial.println("WE" + String(WE) );
    
    //sprintf(cLatitude,  "%10.4f", latitude);
    //sprintf(cLongitude, "%10.4f", longitude);
    //sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    //Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);
    if(num >0)
    {
      fix = '1' ;
    }
    else
    {
      fix = '0';  
    }
    //sprintf(buff, "satellites number = %d", num);
    //Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void loraInit()
{
  Serial1.print("p2p set_freq 868300000");
  Serial1.print('\r');
  delay(500);
  Serial1.print("p2p set_sync 34");
  Serial1.print('\r');
  delay(500);
}

int strToByte(int s, byte *data, String str)
{
  int i  = 0, j;
  byte tmp = 0x00;
  Serial.println("str " + str + "len" + str.length());
  if(str.length() %2 != 0 )
  {
    return -1;  
  }
  for(i = 0; i<(str.length()/2); i++)
  {
    j = 0;
    int k = i*2;
    if(str[k] <= '9')
    {
      j = (str[k]-'0')*16; 
    }
    else
    {
      j = (str[k]-  'a'+10)*16;
    }
    if(str[k+1] <= '9')
    {
      j += (str[k+1]-'0');
    }
    else
    {
      j += (str[k+1]-  'a'+10);
    }
    tmp = j;
    Serial.print( String(tmp, HEX) + " j " + String(j) + "  ");
    data[s+i] = tmp;
  }
  Serial.println();

  return 0;
}

void interrupt()
{
  Serial.println("interrupt");
  SoS = 1;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  attachInterrupt(0, interrupt, RISING);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ..."); 
  delay(5000);
  Serial1.begin(115200);
  Serial.println("Hello, world1?");
  loraInit();
}

void loop() {
  int heart = 0;
  int i,l;
  String s="",s2 = "";
  int iLongitude,iLatitude;
  byte data[20];
  data[12] = 0x00;
  unsigned long time = time = millis();
  while( millis() - time <5000 )
  {
    if(SoS ==1)
    {
      SoS = 1;
      data[12] = 0x80;
      SoS = 0;
      break;  
    }
  }
  
  data[0] = type;
  data[1] = id;
  // put your main code here, to run repeatedly:
  //Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  //Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);

  iLongitude = (int)longitude*1000;
  iLatitude = (int)latitude*1000;

  s2 = String(iLatitude, HEX);
  //fix iLatitude.length = 7
  l = s2.length();
  l = 7-l;
  for(i = 0; i < l;i++)
  {
    s2 = "0" + s2;
  }
    if(NS = 'N')
  {
    s2 = "0" + s2;
  }
  else
  {
    s2 = "1" + s2;
  }
  Serial.println(s2);
  //data[2 ~5]
  strToByte(2,data,s2);
  s2 = "";
  
  s2 = String(iLongitude, HEX);
  l = s2.length();
  l = 7-l;
  //fix iLongitude.length = 7
  for(i = 0; i <l;i++)
  {
    s2 = "0" + s2;
  }
  if(WE = 'E')
  {
    s2 = "0" + s2;
  }
  else
  {
    s2 = "1" + s2;
  }
  Serial.println(s2);
  //data[6 ~9]
  strToByte(6,data,s2);
 
  //heart
  data[10] = 0x00;
  data[11] = 0x00;
  //status
  //data[12] = 0x00;
   if(fix =='0')
  {
    data[12] |= 0x01;
  }
  data[12] |= 0x02;
  //check sum
  byte check = 0x00;
  for(i = 0;i<13; i++)
  {
    check = check ^ data[i] ;
  }
  data[13] = check;
  //Serial.println("data"); 
  noInterrupts();
  Serial1.print("p2p tx ");
  Serial.print("p2p tx ");
  for(i=0;i<14;i++)
  {
   String fk = String(data[i],HEX);
   if(fk.length() <2)
   {
    fk = "0"+fk; 
   }
   Serial1.print(fk);
   Serial.print(fk);
  }
  Serial1.print('\r');
  Serial.println();
  interrupts();
  //Serial1.print("p2p tx 000000");
  //Serial1.print("\r\n");
  
  
}
