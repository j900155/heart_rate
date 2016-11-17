#include <LGPS.h>

const int type = 0x01;
const int id = 0x00;

int a;
int count = 0,num = 0;  

String apple;
char sdata;
String asd;
char NS,WE;

gpsSentenceInfoStruct info;
char buff[256];
char cLatitude[10];
char cLongitude[10];
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
  double latitude;
  double longitude;
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
    
    sprintf(cLatitude,  "%10.4f", latitude);
    sprintf(cLongitude, "%10.4f", longitude);
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
int charToInt(char *c, int s, int e)
{
    int i = 0;
    int p = 0;
    Serial.println("e-s" + String(c));
    for(i = s;i<e;i++)
    {
      
      if(c[i]>= '0' && c[i] <='9')
      {
        Serial.print(String(i) + String(c[i]));
        p += (c[i]-'0')*pow(10,(e-i-1)); 
      }
    }
    Serial.println("");
    Serial.println("charToInt 1  " + String(p));
    if(p<0)
    {
     p = 0;  
    }
    Serial.println("charToInt " + String(p));
    return p;
}
byte hexTobyte(String s)
{
  byte b= 0x00;
  int j = 0;
  if(s[0] <= '9')
  {
   j = (s[0]-'0')*16; 
  }
  else
  {
    j = (s[0]-  'a'+10)*16;
  }
  if(s[1] <= '9')
  {
   j += (s[1]-'0');
  }
  else
  {
    j += (s[1]-  'a'+10);
  }
  //Serial.println("j2 " + String(j));
  b = j;
  return b; 
}

String fix4(String s)
{
  int i =0;
  int l = s.length();
  Serial.println("s.length " + String(l));
  for(i=0;i<(4 - l);i++)
  {
    s ="0"+s;  
  }
  return s;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  byte data[20],b;
  data[0] = type;
  data[1] = id;
 
  // put your main code here, to run repeatedly:
  //Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  //Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);

  
  
  Serial.println(cLatitude);
  Serial.println("NS1 " + String(NS) );
  Serial.println(cLongitude);
  Serial.println(String(WE));
  
  Serial.println("fix " + String(fix));
  //N1
  l = charToInt(cLatitude,1,5);
  s = String(l,HEX);
  //Serial.println("hex "+s);
  s = fix4(s);
  Serial.println(s);
  
  s2 = s.substring(0,2);
  Serial.println("N1 " + s2);
  b = hexTobyte(s2);
  data[2] = b;
  
  s2 = s.substring(2,5);
  Serial.println("N1 " + s2);
  b = hexTobyte(s2);
  data[3] = b;
  //Serial.println(String(b));
  
  //N2
  l = charToInt(cLatitude,6,10);
  s = String(l,HEX);
  //Serial.println("hex2 "+s);
  s = fix4(s);
  Serial.println("N2 s "+s);
  s2 = s.substring(0,2);
  Serial.println("N2 " + s2);
  b = hexTobyte(s2);
  data[4] = b;
  
  s2 = s.substring(2,5);
  Serial.println("N2 " + s2);
  b = hexTobyte(s2);
  data[5] = b;
  
  //Serial.println("NS " + String(NS));
  if(NS =='N')
  {
    data[6] = 0x00;  
  }
  else if(NS =='S')
  {
    data[6] = 0x01;  
  }
  //--------------
  l = charToInt(cLongitude,0,5);
  s = String(l,HEX);
  s = fix4(s);
  //Serial.println("hex WE1 "+s);
  s2 = s.substring(0,2);
  Serial.println("W1 " + s2);
  b = hexTobyte(s2);
  data[7] = b;
  s2 = s.substring(2,4);
  Serial.println("W1 " + s2);
  b = hexTobyte(s2);
  data[8] = b;
  l = charToInt(cLongitude,6,10);
  s = String(l,HEX);
  s = fix4(s);
  //Serial.println("W2 "+s);
  s2 = s.substring(0,2);
  Serial.println("W2 " + s2);
  b = hexTobyte(s2);
  data[9] = b;
  s2 = s.substring(2,4);
  Serial.println("W2 " + s2);
  b = hexTobyte(s2);
  data[10] = b;
   if(WE =='E' )
  {
    data[11] = 0x00;  
  }
  else if(WE =='W')
  {
    data[11] = 0x01;  
  }
  if(fix =='0')
  {
    data[6] = 0x10; 
    data[11] = 0x10;
  }
  //heart
  data[12] = 0x00;
  data[13] = 0x00;
  //keep
  data[14] = 0x00;
  data[15] = 0x00;
  //check sum
  byte check = 0x00;
  for(i = 0;i<16; i++)
  {
    check = check ^ data[i] ;
  }
  data[16] = check;
  //Serial.println("data"); 
  
  Serial1.print("p2p tx ");
  for(i=0;i<17;i++)
  {
   String fk = String(data[i],HEX);
   if(fk.length() <2)
   {
    fk = "0"+fk; 
   }
   Serial1.print(fk);
    //Serial1.write(data[i]);
  }
  Serial1.print('\r');
  
  //Serial1.print("p2p tx 000000");
  //Serial1.print("\r\n");
  delay(5000);
  
}
