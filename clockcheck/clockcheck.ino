#include <DFRobot_OxygenSensor.h>
#include <Arduino_HTS221.h>
#include <Arduino_LSM9DS1.h>
#define COLLECT_NUMBER    1             // collect number, the collection range is 1-100.
#define Oxygen_IICAddress ADDRESS_3
/*   iic slave Address, The default is ADDRESS_3.
       ADDRESS_0               0x70      // iic device address.
       ADDRESS_1               0x71
       ADDRESS_2               0x72
       ADDRESS_3               0x73
*/
#define LSM9DS1_ADDRESS            0x6b
#define LSM9DS1_ADDRESS_M          0x1E
#define LSM9DS1_WHO_AM_I           0x0f
#define LSM9DS1_CTRL_REG1_G        0x10
#define LSM9DS1_STATUS_REG         0x17
#define LSM9DS1_OUT_X_G            0x18
#define LSM9DS1_CTRL_REG6_XL       0x20
#define LSM9DS1_CTRL_REG8          0x22
#define LSM9DS1_OUT_X_XL           0x28

#define PACKET_SIZE                240

//#define DEBUG 1

DFRobot_OxygenSensor Oxygen;
unsigned int XL_start = 0;
unsigned int start2 = 0;
unsigned int now = 0;

int O2_INDEX = 0;
int IMU_INDEX = 0;
int TEMP_INDEX = 0;
int CAPA_INDEX = 0;

int i;
int j;
int Send_index = 0;

#ifdef DEBUG

int IMUCLOCK[20000];
int CAPACLOCK[10];
int TEMPCLOCK[10];
int O2CLOCK[10];
int BLUECLOCK[20000];

int IMUCLOCK_INDEX = 0;
int CAPACLOCK_INDEX = 0;
int TEMPCLOCK_INDEX = 0;
int O2CLOCK_INDEX = 0;
int BLUECLOCK_INDEX = 0;
#endif

byte sendBuffer[241];
int BUFFER_INDEX = 0;

float tempbuf;
float O2;
short capacitive;
float capatotal;

int16_t XL_data[3];
int16_t GS_data[3];
int16_t M_data[3];
int cnt = 0;
int capacnt;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  #ifdef DEBUG
    while(!Serial);
  #endif
  IMUInit();
  HTSInit();
  OxygenInit();
  Wire.begin();
  start2 = XL_start = micros();
}
void loop() {
  now = micros();
  if (abs(now - XL_start) >= 4200)
  {
    XL_start = now;
    #ifdef DEBUG
      IMUCLOCK[IMUCLOCK_INDEX++] = micros();
    #endif
    if(cnt == 0)
    {
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      Serial1.write(0xFF);
      Serial1.write(0xFE);
      capatotal = 0;
      capacnt = 0;
    }
    if(cnt % 3 == 0)
    {
      IMU.readRegisters(0x1E, 0x28, (uint8_t*)M_data, sizeof(M_data));
      for(i=0;i<3;i++)
      {
      sendBuffer[BUFFER_INDEX] = ((M_data[i] & 0xFF00) >> 8);
      sendBuffer[BUFFER_INDEX+1] = M_data[i] & 0x00FF;
      BUFFER_INDEX +=2;
      }
    }
    if(cnt % 238 == 0)
    {
      capacitive = readDataFromSensor(0x04);
      if(capacitive != -1)
      {
        capatotal += capacitive;
        capacnt++;  
      }
    }
    IMU.readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_XL, (uint8_t*)XL_data, sizeof(XL_data));
    IMU.readRegisters(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_G, (uint8_t*)GS_data, sizeof(GS_data));
    for(i=0;i<3;i++)
    {
      sendBuffer[BUFFER_INDEX] = ((XL_data[i] & 0xFF00) >> 8);
      sendBuffer[BUFFER_INDEX+1] = XL_data[i] & 0x00FF;
      BUFFER_INDEX +=2;
    }
    for(i=0;i<3;i++)
    {
      sendBuffer[BUFFER_INDEX] = ((GS_data[i] & 0xFF00) >> 8);
      sendBuffer[BUFFER_INDEX+1] = GS_data[i] & 0x00FF;
      BUFFER_INDEX +=2;
    }

    #ifdef DEBUG
      IMUCLOCK[IMUCLOCK_INDEX++] = micros();
    #endif

    if(cnt == 1000)
    {
      IMU.Reset();
      delayMicroseconds(10);
      IMU.SetXL(50, 8);
      IMU.SetGS(238, 500);
      IMU.SetFIFO(6);
    }
    
    if(cnt == 2377)
    {
      
    #ifdef DEBUG
      TEMPCLOCK[TEMPCLOCK_INDEX++] = micros();
    #endif

    
    tempbuf = HTS.readTemperature();
    //tempbuf to byte array
    //0Serial.println(tempbuf,6);
    byte* ptr = (byte*)(&tempbuf);
    for(i=0;i<4;i++)
    {
      sendBuffer[BUFFER_INDEX++] = *(ptr+i);
    }


    
    #ifdef DEBUG
      TEMPCLOCK[TEMPCLOCK_INDEX++] = micros();
    #endif
    
    }

    
    if(cnt == 2378)
    {
      #ifdef DEBUG
        CAPACLOCK[CAPACLOCK_INDEX++] = micros();
      #endif


      if(capacnt != 0)
        capatotal = capatotal / capacnt;
      else
        capatotal = -1;
      //capacitive to byte array

      byte* ptr = (byte*)(&capatotal);
      //Serial.println(capatotal);
      for(i=0;i<4;i++)
      {
        sendBuffer[BUFFER_INDEX++] = *(ptr+i);
      }
      
      #ifdef DEBUG
        CAPACLOCK[CAPACLOCK_INDEX++] = micros();
      #endif
    }



    
    if(cnt == 2379)
    {
      #ifdef DEBUG
        O2CLOCK[O2CLOCK_INDEX++] = micros();
      #endif
      O2 = Oxygen.ReadOxygenData(COLLECT_NUMBER);
      //Serial.println(O2);
      byte* ptr = (byte*)(&O2);
      //O2 to byte array
      for(i=0;i<4;i++)
      {
        sendBuffer[BUFFER_INDEX++] = *(ptr+i);
      }


      
      #ifdef DEBUG
        O2CLOCK[O2CLOCK_INDEX++] = micros();
      #endif
    }



    #ifdef DEBUG
      BLUECLOCK[BLUECLOCK_INDEX++] = micros();
    #endif
    
    
    Serial1.write(sendBuffer, BUFFER_INDEX);
    Send_index++;
    BUFFER_INDEX = 0;
   
    
    #ifdef DEBUG
      BLUECLOCK[BLUECLOCK_INDEX++] = micros();
    #endif
    
    cnt++;
    if(cnt == 2380)
    {
      cnt = 0;
    }
  }



  #ifdef DEBUG
  if(Send_index == 2380)
  {
    Serial.println("IMU");
    for(i=0;i<IMUCLOCK_INDEX;i++)
    {
      Serial.print(IMUCLOCK[i]);
      Serial.print(" ");
    }
    Serial.println("");
    Serial.println("TEMP");
    for(i=0;i<TEMPCLOCK_INDEX;i++)
    {
      Serial.print(TEMPCLOCK[i]);
      Serial.print(" ");
    }
    Serial.println("");
    Serial.println("CAPA");
    for(i=0;i<CAPACLOCK_INDEX;i++)
    {
      Serial.print(CAPACLOCK[i]);
      Serial.print(" ");
    }
    Serial.println("");
    Serial.println("O2");
    for(i=0;i<O2CLOCK_INDEX;i++)
    {
      Serial.print(O2CLOCK[i]);
      Serial.print(" ");
    }
    Serial.println("");
    Serial.println("BLUE");
    for(i=0;i<BLUECLOCK_INDEX;i++)
    {
      Serial.print(BLUECLOCK[i]);
      Serial.print(" ");
    }
    
    while(1);
  }
  #endif
}
void IMUInit()
{
  if (!IMU.begin(50, 238, 8, 500, 6))  // 59.5Hz, 2g, 500dps, continuous Mode
  {
    while (1)
      Serial.println("IMU Initialize Fail");;
  }
}

void HTSInit()
{
  if (!HTS.begin(0))    // oneshot
  {
    while (1)
      Serial.println("HTS Initialize Fail");
  }
}

void OxygenInit()
{
  if(!Oxygen.begin(Oxygen_IICAddress))
  {
    while(1)
      Serial.println("Oxgen Initialize Fail");;
  }
}
short readDataFromSensor(short address)
{
  byte i2cPacketLength = 6;
  byte outgoingI2CBuffer[3];
  byte incomingI2CBuffer[6];

  outgoingI2CBuffer[0] = 0x01;
  outgoingI2CBuffer[1] = 128;
  outgoingI2CBuffer[2] = i2cPacketLength;

  Wire.beginTransmission(address); 
  Wire.write(outgoingI2CBuffer, 3);
  if(Wire.endTransmission() != 0)
    return -1;
  Wire.requestFrom(address, i2cPacketLength);
  byte incomeCount = 0;
  while (incomeCount < i2cPacketLength)
  {
    if (Wire.available())
    {
      incomingI2CBuffer[incomeCount] = Wire.read();
      incomeCount++;
    }
    else
    {
      delayMicroseconds(10);
    }
  }

  short rawData = (incomingI2CBuffer[4] << 8) + incomingI2CBuffer[5]; //get the raw data

  return rawData;
}
