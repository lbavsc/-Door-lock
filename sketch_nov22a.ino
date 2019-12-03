#define BLINKER_WIFI
#include<Servo.h>
#include <Adafruit_Fingerprint.h>


#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#define BLINKER_ALIGENIE_OUTLET
#define BLINKER_MIOT_OUTLET
#include <Blinker.h>

SoftwareSerial mySerial(13, 15);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

Servo myservo;

char auth[] = "9910dafd5986";
char ssid[] = "@PHICOMM_D4";
char pswd[] = "asdf123456";

// 新建组件对象
BlinkerButton Button1("btn-abc");
BlinkerNumber Number1("num-abc");

bool oState = false; //天猫精灵
bool oStates[5] = { false };
void aligeniePowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        digitalWrite(LED_BUILTIN, HIGH);
        BlinkerAliGenie.powerState("on");
        BlinkerAliGenie.print();
        button1_callback("on");
        oState = true;
        
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(LED_BUILTIN, LOW);
        BlinkerAliGenie.powerState("off");
        BlinkerAliGenie.print();

        oState = false;
    }
    
}

void aligenieQuery(int32_t queryCode)
{
    BLINKER_LOG("AliGenie Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("AliGenie Query All");
            BlinkerAliGenie.powerState(oState ? "on" : "off");
            BlinkerAliGenie.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("AliGenie Query Power State");
            BlinkerAliGenie.powerState(oState ? "on" : "off");
            BlinkerAliGenie.print();
            break;
        default :
            BlinkerAliGenie.powerState(oState ? "on" : "off");
            BlinkerAliGenie.print();
            break;
    }
}

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();

    uint32_t BlinkerTime = millis();
    Blinker.print(BlinkerTime);
    Blinker.print("millis", BlinkerTime);
}

//小爱同学
void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        digitalWrite(LED_BUILTIN, HIGH);
        button1_callback("on");
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();

        oState = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(LED_BUILTIN, LOW);

        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();

        oState = false;
    }
}

void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(oState ? "on" : "off");
            BlinkerMIOT.print();
            break;
    }
}
// 按下按键即会执行该函数
void button1_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(14, HIGH);
    delay(100);
    myservo.write(0);
    delay(5000);
    myservo.write(120);
    delay(500);
  
   
   
   digitalWrite(14, LOW);
}



void setup()
{
 
   #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // 初始化串口
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    // 初始化舵机
    myservo.attach(12);
    myservo.write(120);
    // 初始化有LED的IO
    pinMode(14, OUTPUT);
    digitalWrite(14, LOW);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    //天猫精灵
    BlinkerAliGenie.attachPowerState(aligeniePowerState);
    BlinkerAliGenie.attachQuery(aligenieQuery);
    //小爱同学
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachQuery(miotQuery);
    Button1.attach(button1_callback);

    //指纹
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
    delay(100);
    Serial.println("\n\nAdafruit finger detect test");
  
    // set the data rate for the sensor serial port
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
    } else {
      Serial.println("Did not find fingerprint sensor :(");
      while (1) { delay(1); }
    }
  
    finger.getTemplateCount();
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
     
  
}

void loop() {
   
    Blinker.run();
    getFingerprintIDez();  
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  if(finger.confidence > 100){
    button1_callback("on");
  }
  return finger.fingerID; 
}
