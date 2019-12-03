#include <SoftwareSerial.h>  
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h> 
        
SoftwareSerial esp01(10,11);

//bool waitForWifiConnection =false;
//3332 /16bit address FFFF
// MAc czujnika 2 : 0013A200417CA448
byte sourceMac[8];
byte data[86]; 
byte noData[86];
byte frameType=99;
bool frame = false;
bool isThere = false;
bool isSensorThere = false;
byte serwerMacAddress[8] = {0x00,0x13,0xA2,0x00,0x41,0x7E,0xB7,0x3D};
byte sensorMac[8];
byte sensor1Mac[8];
byte sensor2Mac[8];
byte sensor3Mac[8];
int amountOfSensors =0;
bool registeredSensors[3] = {false,false,false};
const char* sensorsStatus[3] = {"off","off","off"};
bool sensorWaitingForStatus[3] = {false,false,false};
bool waitingForStatus = false;
int waitingForStatusCounter=0;
bool check =false;
bool realTime =false;
int whichSensor=99;
char wifiFrameType;
int whichSensorToUnregister=99;
unsigned long actualTime=0;
unsigned long counterTime=0;
unsigned long counter2Time=0;
unsigned long differenceTime=0;
unsigned long waitingForStatusTime=0;

/////////////////////////////////////WIFI
int wifiReconnectionCounter =0;
bool wifiAutonomous=false;
bool checkWifiStatus = false;
bool isWifiAvailable=false;
bool serverLastCall;
char ssid[] = "Server1234";           
char pass[] = "19961996";      
int wifiStatus = WL_IDLE_STATUS; 
byte packetBuffer[10];
char server[] = "192.168.4.1";
unsigned long actualTimeWifi=0;
unsigned long counterTimeWifi=0;
unsigned long differenceTimeWifi=0;
unsigned long counterWifiTime=0;
unsigned long differenceWifiTime=0;
WiFiEspUDP Udp;
bool test=false;
RingBuffer buf(8);

float randNum;;
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  esp01.begin(9600);
  WiFi.init(&esp01);


   if (WiFi.status() == WL_NO_SHIELD) {
    isWifiAvailable = false;
   generateFrame(9,serwerMacAddress,noData);
  }else{
    if(WiFi.status() != WL_CONNECTED){
          wifiStatus = WiFi.begin(ssid, pass);
          if(wifiStatus == 1){
           isWifiAvailable = true;
            Udp.begin(4040);
            
          }else{
            isWifiAvailable = false;
     
             }
    }else if(serverLastCall == true){
      isWifiAvailable = true;
    }
  }
  
  if(isWifiAvailable == true){
    generateWifiFrame(0,noData);
  }else{
      while(Serial1.available()){
        byte discardByte = Serial1.read();
      }
     generateFrame(0,serwerMacAddress,noData);
  }

  

}


void loop() {
    
    //getRandomData();
    
    readWifiFrame();
    readFrame();
  
    actualTime = millis();
    differenceTime = actualTime - counterTime;
    differenceTimeWifi = actualTime - counterTimeWifi;
    differenceWifiTime = actualTime - counterWifiTime;



    if(differenceTime >= 15000UL){

      if(amountOfSensors == 0){
        if(isWifiAvailable == true){
          generateWifiFrame(0,noData);
        }else{
          generateFrame(0,serwerMacAddress,noData);
        }
      }else{
        if(realTime == false){
       checkStatus(0);
      }
      }
      counterTime = actualTime;
      
                     
     }

    if(differenceTimeWifi >= 4000){
      checkIfWifiAvailable();
      counterTimeWifi = actualTime;
     // Serial.println(WiFi.RSSI());
      }

    if(differenceWifiTime >= 5000){
      serverLastCall = true;
      counterWifiTime = actualTime;
     }

    waitingForStatusResponse();
    

    }
void readWifiFrame(){
  
   int packetSize = Udp.parsePacket();
   int len = Udp.read(packetBuffer, 25);
   
    if (len > 0) {
      packetBuffer[len] = 0;
      for(int i=0;i<25;i++){
        if(i==0){
          frameType = packetBuffer[i];
        }else{
          data[i-1] = packetBuffer[i];
       
        }
    }
    len=0;
        
          Serial.println();
         frameDecision();
          serverLastCall = true;
          Serial.println();

          
    }
    
  
} 
void readFrame(){
  if(Serial1.available()>101){
      if(Serial1.read() == 0x7E){
          for(int i=2;i<102;i++){
            if(i>4 && i<13){
              sourceMac[i-5] = (Serial1.read());
            }else if(i==15){
              frameType=(Serial1.read());
            }
            else if(i>15 && i<101){
              data[i-16] = (Serial1.read());
            }else{
              byte discardByte = Serial1.read();
            }
            
            if(i == 101){
              frame = true;
            }
        } 
      }
    }
   
   if(frame == true){
       checkMac(sourceMac); 
       if(isThere == true){
          frameDecision();   
       }else{
        
        Serial.print("Invalid MAC address");
        generateFrame(7,sourceMac,noData);
        frame=false;
        isThere = false; 
       } 
   }
    
 }

void frameDecision(){
   
      if(isThere == true || isWifiAvailable == true){

        switch(frameType){
          
          case 0:
                                      
                                      if(whichSensor == 0 || isWifiAvailable == true && realTime == false){
                                         registerSensor(data);
                                      Serial.print("Registered sensors...:");
                                      Serial.println();
                                      for(int i=0;i<3;i++){
                                        if(registeredSensors[i] == true){
                                             switch(i){
                                                case 0:
                                                  for(int i=0;i<8;i++){
                                                     Serial.print(sensor1Mac[i],HEX);
                                                    }
                                                    Serial.println();
                                                break;
                                                
                                                case 1:
                                                     for(int i=0;i<8;i++){
                                                      Serial.print(sensor2Mac[i],HEX);
                                                    }
                                                     Serial.println();
                                                break;
                                        
                                                case 2:
                                                     for(int i=0;i<8;i++){
                                                     Serial.print(sensor3Mac[i],HEX);
                                                    }
                                                    Serial.println();
                                                break;
                                             }
                                        }
                                      }
                                      }
                                     
                               

                                 
           break;

          case 1:
                                          
                              switch(whichSensor){
                                
                                  case 1:
                                  sensorsStatus[0] = "on";
                                  sensorWaitingForStatus[0] = false;
                                  break;
                                  case 2:
                                  sensorsStatus[1] = "on";
                                  sensorWaitingForStatus[1] = false;
                                  break;
                                  case 3:
                                  sensorsStatus[2] = "on";
                                  sensorWaitingForStatus[2] = false;
                                  break;

                                  
                                  }
                                  
                                  for(int i=0;i<3;i++){
                                    if(registeredSensors[i] == true){
                                         if(sensorWaitingForStatus[i] == true){
                                        check = true;
                                        break;
                                      }else{
                                        check = false;
                                      }
                                    }else{
                                      sensorWaitingForStatus[i] = false;
                                    }
                                   
                                  }
    
                                  if(check == false){
                                    waitingForStatus = false;
                                  }
    
          break;

          case 2:
                      
                         
                          switch(whichSensor){
                            case 1:
                            generateFrame(2,serwerMacAddress,sensor1Mac);
                            sensorWaitingForStatus[0] = false;
                            break;

                            case 2:
                            generateFrame(2,serwerMacAddress,sensor2Mac);
                            sensorWaitingForStatus[1] = false;
                            break;

                            case 3:
                            generateFrame(2,serwerMacAddress,sensor3Mac);
                            sensorWaitingForStatus[2] = false;
                            break;
                          }
                         
          break;

          case 3:
                  
                          // RealTimeMeasuremen od czujnika pod "data" -               
                          
                           Serial.println();
                           switch(whichSensor){
 
                            
                            case 1:
                            
                           for(int i=0;i<8;i++){
                              sensorMac[i]=sensor1Mac[i];
                            }


                         
                            generateFrame(3,serwerMacAddress,data);
                            
                 
                            sensorWaitingForStatus[0] = false;
                            
                            
                            
                            break;

                            case 2:
                            for(int i=0;i<8;i++){
                              sensorMac[i]=sensor2Mac[i];
                            }
                            
                          
                            generateFrame(3,serwerMacAddress,data);
                          
                 
                             sensorWaitingForStatus[1] = false;
                            
                            
                            break;

                            case 3:
                             for(int i=0;i<8;i++){
                              sensorMac[i] = sensor3Mac[i];
                            }
                          
                            
                             generateFrame(3,serwerMacAddress,data);
                            
                 
                             sensorWaitingForStatus[2] = false;
                           
                            
                            break;
                          } 
                          
                         realTime = true;
                          
          break;
    
          case 4:
                           // rejestracja nowego czujnika
                           Serial.println("Rejestracja");
                           
                           Serial.print("New sensor registerration...");
                           registerSensor(data);
                           for(int i=0;i<8;i++){
                            Serial.print(data[i],HEX);
                            Serial.print(",");
                           }
                              
  
          break;

          case 5:
                          Serial.println("wyrejestrowanie");
                          
                          amountOfSensors--;
                          checkSensorMac(data);
                          if(isSensorThere == true){
                          Serial.println( whichSensorToUnregister);
                          Serial.print("Sensor unregistering.....");
                          registeredSensors[whichSensorToUnregister] = false; 
                          sensorWaitingForStatus[whichSensorToUnregister] = false;
                        
                          }else{
                            Serial.print("No such sensor in database.."); //dodac iteracje w petli... i zmienic wartosc isThere
                          }
                        
                               
          break;
          case 6:
          
                if(amountOfSensors==0){
                  if(isWifiAvailable == true){
                       generateWifiFrame(0,noData);
                  }else{
                      generateFrame(0,serwerMacAddress,noData);
                  }
                
                }else{
                   Serial.println("Ruch ciagly");
                
                for(int i=0;i<8;i++){
                  sensorMac[i] = (data[i]);
                }

       
                 generateFrame(6,sensorMac,noData);
                }
               
                
               
               
                
          break;
          case 7:
               if(amountOfSensors==0){
                  if(isWifiAvailable == true){
                       generateWifiFrame(0,noData);
                  }else{
                      generateFrame(0,serwerMacAddress,noData);
                  }
                
                }else{
                  Serial.println("zatrzymaj ruch ciagly");
                for(int i=0;i<8;i++){
                  sensorMac[i] = (data[i]);
                }
              
                for(int i=0;i<3;i++){
                
                   generateFrame(7,sensorMac,noData);
                }
                 
                realTime = false;
                }
                
               
          break;

          case 9:

                counterWifiTime = actualTime;
                Serial.println("Wifi disabled");
                isWifiAvailable = false;
                serverLastCall = false;

          break;
          case 11:
                if(data[0] == false){
                  wifiAutonomous = false;
                }else{
                  wifiAutonomous = true;
                }
          break;
        }
  
      }
        isThere = false;
        frame = false;
        frameType=99;
        whichSensor=99;
        isSensorThere = false;
        whichSensorToUnregister =99;
  
}
void sendAlarm(){
  for(int i=0;i<3;i++){
    if(sensorWaitingForStatus[i] == true){
      if(isWifiAvailable == true){
            switch(i){
            case 0:
            generateWifiFrame(1,sensor1Mac);
            break;
            case 1:
            generateWifiFrame(1,sensor2Mac);
            break;
            case 2:
            generateWifiFrame(1,sensor3Mac);
            break;
          }
      }else{
          switch(i){
          case 0:
          generateFrame(1,serwerMacAddress,sensor1Mac);
          break;
          case 1:
          generateFrame(1,serwerMacAddress,sensor2Mac);
          break;
          case 2:
          generateFrame(1,serwerMacAddress,sensor3Mac);
          break;
          }
      }     
    }   
  }
}

void checkStatus(int option){
  switch(option){

    case 0:
            for(int i=0;i<3;i++){
              if(registeredSensors[i] == true){
                  switch(i){
                  case 0:
                  Serial.println("Geting Sensor1 status");
                  sensorWaitingForStatus[0] = true;
                  generateFrame(2,sensor1Mac,noData);
              
                  break;
                  case 1:
                   Serial.println("Geting Sensor2 status");
                  sensorWaitingForStatus[1] = true;
                  generateFrame(2,sensor2Mac,noData);
               
                  break;
                  case 2:
                  Serial.println("Geting Sensor3 status");
                   sensorWaitingForStatus[2] = true;
                   generateFrame(2,sensor3Mac,noData);
               
                  break;
                }
              waitingForStatus = true;
              }
            }
    break;

    case 1:
          for(int i=0;i<3;i++){

            if(sensorWaitingForStatus[i] == true && registeredSensors[i] == true ){
              switch(i){

                  case 0:
                  Serial.println("Geting Sensor1 status again...");
                  generateFrame(2,sensor1Mac,noData);
              
                  break;
                  case 1:
                   Serial.println("Geting Sensor2 status again...");
                  generateFrame(2,sensor2Mac,noData);
                
               
                  break;
                  case 2:
                  Serial.println("Geting Sensor3 status again...");
                  generateFrame(2,sensor3Mac,noData);
               
                  break;
                
              }
            }
            
          }
    break;
    
  }

}

bool checkMac(byte mac[]){
 

 for(int x=0;x<5;x++){
  if(isThere == false){
   for(int i=5;i<7;i++){
    if(x==0){          
            if(mac[i] == (serwerMacAddress[i])){
            isThere = true;
            whichSensor=0;
            
          }else{
            isThere = false;
             break;
          }
       
     }else if(x == 1){
      if(registeredSensors[0] == true){
            if(mac[i] == (sensor1Mac[i])){
            isThere = true;
            whichSensor=1;
          }
          else{
            isThere = false;
            break;
          }
       }else{
            isThere = false;
            break;
          }
      }
      else if(x == 2){
         if(registeredSensors[1] == true){
            if(mac[i] == (sensor2Mac[i])){
            isThere = true;
           whichSensor=2;
          }
          else{
            isThere = false;
          
             break;
          }
     
        
      }
       else{
            isThere = false;
            break;
          }
      }
      else if(x == 3){
        if(registeredSensors[2] == true){
            if(mac[i] == (sensor3Mac[i])){
            isThere = true;
            whichSensor=3;
          }
          else{
            isThere = false;
            break;
          }   
       
      }
             else{
            isThere = false;
            break;
          }
        
          }
       else if(x ==4){
         whichSensor =99;
          isThere = false;
       }
    
      
     
   }   
 }
 }
}

void checkSensorMac(byte mac[]){
    

 for(int x=0;x<4;x++){
  if(isSensorThere == false){
   for(int i=5;i<7;i++){
    if(x==0){
            if(registeredSensors[0] == true){
            if(mac[i] == (sensor1Mac[i])){
            isSensorThere = true;
            whichSensorToUnregister=0;
            
          }else{
            isSensorThere = false;
             break;
          }}
       else{
            isSensorThere = false;
            break;
          }
     }else if(x == 1){
      if(registeredSensors[1] == true){
            if(mac[i] == (sensor2Mac[i])){
            isSensorThere = true;
            whichSensorToUnregister=1;
          }
          else{
            isSensorThere = false;
            break;
          }
       }else{
            isSensorThere = false;
            break;
          }
      }
      else if(x == 2){
         if(registeredSensors[2] == true){
            if(mac[i] == (sensor3Mac[i])){
           isSensorThere = true;
           whichSensorToUnregister=2;
          }
          else{
            isSensorThere = false;
          
             break;
          }
     
        
      }
       else{
            isSensorThere = false;
            break;
          }
      }
       else if(x ==3){
         whichSensorToUnregister =99;
          isSensorThere = false;
       }
    
      
     
   }   
 }
 }
}


void registerSensor(byte mac[]){
    bool isTheSame = false;
   for(int i=0;i<3;i++){ 

    if(isTheSame == true){
      Serial.println("Sensor already exist in database");
      isTheSame = false;
      break;
      
    }
    if(registeredSensors[i] == false){
      Serial.print("Sensor registered..");
      Serial.println();
      registeredSensors[i] = true;
      amountOfSensors++;
      switch(i){
        case 0:
          for(int i=0;i<8;i++){
              sensor1Mac[i] = mac[i];
            }
        break;
        
        case 1:
             for(int i=0;i<8;i++){
              sensor2Mac[i] = mac[i];
            }
        break;

        case 2:
             for(int i=0;i<8;i++){
            sensor3Mac[i] = mac[i];
            }
        break;
      }
    break;
    }else{

        switch(i){
          case 0:
          for(int i=0;i<8;i++){
              if(mac[i] == sensor1Mac[i]){
                isTheSame = true;
              }else{
                isTheSame = false;
                break;
              }
             }
          break;
          case 1:
          for(int i=0;i<8;i++){
              if(mac[i] == sensor2Mac[i]){
                isTheSame = true;
              }else{
                isTheSame = false;
                break;
              }
             }
          break;
          case 2:
          for(int i=0;i<8;i++){
              if(mac[i] == sensor3Mac[i]){
                isTheSame = true;
              }else{
                isTheSame = false;
                break;
              }
             }
          break;
        }
      
    }
   }

}

void waitingForStatusResponse(){
      if(waitingForStatus == true){
        waitingForStatusTime = actualTime - counter2Time;
        
        if(waitingForStatusTime >= 3000UL){
          if(waitingForStatusCounter<3){
             checkStatus(1);
             waitingForStatusCounter++;
             counter2Time = actualTime;
          }else{
            sendAlarm();
            Serial.println("Warinig, one of the sensors did not respond");
             waitingForStatus = false;
             waitingForStatusCounter=0;        
        }
      }
      
      }else{
      counter2Time = actualTime;
     }
}

void getRandomData(){
   randNum = random(-200,200);
    int kropkaCounter=1;
    int zeroJedenCounter=0;
    byte liczba;
  int counter =0;
  for(int i=0;i<8;i++){
      data[i] = 0;
  }
  for(int i=0;i<100;i++){
    if(zeroJedenCounter - i == 0){
        zeroJedenCounter = i+4;
        data[i] = random(48,49);
      }
    if(kropkaCounter - i == 0){
      kropkaCounter = i+4;
      data[i] = 46;
    }else{
        data[i] = random(48,57);
    }
  }
    
   

  
    generateFrame(10,serwerMacAddress,data);
  
    
    
  }
 


void generateFrame(int frameType,byte destinationMac[8], byte data[86]){
   byte checksum = 0x00;
      // Frame start
      Serial1.write(0x7E);
      // Length
      Serial1.write((byte)0x0);
     
        Serial1.write(0x62);
      
      
      // frame Type 0x00 -> Transmit
      Serial1.write((byte)0x00);
      // respond? 0x00 -No -> no respond from xbee
      Serial1.write((byte)0x00);
      //MAC
      for(int i=0;i<8;i++){
        Serial1.write(destinationMac[i]);
 
        checksum += destinationMac[i];
      }
    
      // Option -> 0x00 no special option
      if(frameType == 3){
        Serial1.write((byte)0x01);
        checksum += 0x01;
      }else{
        Serial1.write((byte)0x00);
      }
       
      // Data
      // frame type, 0x00 -> getMac, 0x01->Status, 0x02 -> Alarm, 0x03->real time measurments
      switch(frameType){
       
        case 0:
        Serial1.write((byte)0x00);
        checksum += 0x00;
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        checksum += 0x00;
        }
        break;
        case 1:
        Serial1.write((byte)0x01);
        checksum += 0x01;
         for(int i=0;i<86;i++){
          if(i<8){
            Serial1.write(data[i]);
            checksum += data[i];
          }else{
            Serial1.write(0x00);
            checksum += 0x00;
          }    
        }
        break;
        case 2:
        Serial1.write((byte)0x02);
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        }
        checksum += 0x02;
        break;
        case 3:
        Serial1.write((byte)0x03);
        checksum += 0x03;
            for(int i=0;i<86;i++){
                  if(i<8){
                    Serial1.write(sensorMac[i]);
                    checksum += sensorMac[i];
                  }else{
                     Serial1.write(data[i-8]);
                    checksum += data[i-8];
                  }  
                
 
        }
    
        break;
        case 6:
         Serial1.write((byte)0x06);
        checksum += 0x06;
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        }
        break;
        
        case 7:
         Serial1.write((byte)0x07);
        checksum += 0x07;
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        }
        break;
        
        case 8:
         Serial1.write((byte)0x08);
        checksum += 0x08;
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        }
        break;

        case 9:
        Serial1.write((byte)0x09);
        checksum += 0x09;
        for(int i=0;i<86;i++){
        Serial1.write((byte)0x00);
        }
        break;
         case 10:
        Serial1.write((byte)0x10);
        checksum += 0x10;
        for(int i=0;i<86;i++){
        Serial1.write(byte(data[i]));
        checksum += byte(data[i]);
        }
        break;
      }
      
       // Checksum
        Serial1.write((0xFF - checksum));
        Serial.print("Checksum: ");
        Serial.print((0xFF - checksum),HEX);
        Serial.println();
  
}
void checkIfWifiAvailable(){

  if (WiFi.status() == WL_NO_SHIELD) {
    isWifiAvailable = false;
   generateFrame(9,serwerMacAddress,noData);
  }else{
    if(WiFi.status() != WL_CONNECTED){
        if(wifiAutonomous == true){
          wifiReconnectionCounter++;
          wifiStatus = WiFi.begin(ssid, pass);
           if(wifiStatus == 1){
         
           isWifiAvailable = true;
            Udp.begin(4040);
            
          }else{
            isWifiAvailable = false;
            generateFrame(9,serwerMacAddress,noData);
     
             }
        }else{
           generateFrame(9,serwerMacAddress,noData);
          isWifiAvailable = false;
 
        }
          if(wifiReconnectionCounter == 3){
            wifiAutonomous = false;
          }
         
    }else if(serverLastCall == true){
      isWifiAvailable = true;
    }
  }

    
 }
  
void generateWifiFrame(int frameType, byte data[99]){

        byte test[25];
        byte newWifiData[25];
        
        switch(frameType){
          case 0:
              for(int i=0;i<25;i++){
              if(i == 0){
                newWifiData[i] = frameType;
              }else{
                newWifiData[i] = data[i-1];
              }
              
            }


          if(Udp.beginPacket(server,4040)){
              Udp.write(newWifiData, 25);
             Udp.endPacket();
             Serial.println("UDP packet send");
             }
           break;

          case 1:
            for(int i=0;i<25;i++){
              if(i == 0){
                newWifiData[i] = frameType;
              }else if(i>0 && i<9){
                newWifiData[i] = data[i-1];
              }else{
                newWifiData[i] = 0;
              }
            }
          if(Udp.beginPacket(server,4040)){
              Udp.write(newWifiData, 25);
             Udp.endPacket();
             Serial.println("UDP packet send");
             }
           break;

           case 3:
              for(int i=0;i<25;i++){
              if(i == 0){
                newWifiData[0] = frameType;
               
                
              }else if(i>0 && i<9){
               newWifiData[i] = sensorMac[i-1];
          
              }else{
                newWifiData[i] = data[i-9];
              }
              }

          if(Udp.beginPacket(server,4040)){
              Udp.write(newWifiData, 25);
             Udp.endPacket();
             Serial.println("UDP packet send");
             }

           break;
           case 10:
              for(int i=0;i<25;i++){
                newWifiData[i] = data[i];
              }
           
             if(Udp.beginPacket(server,4040)){
              Udp.write(newWifiData, 25);
             Udp.endPacket();
             Serial.println("UDP packet send");
             }
           break;
        }
        
}
