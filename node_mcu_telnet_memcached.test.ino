
#include <ESP8266WiFi.h>

//  apt install mencached
//  modify /etc/memcached.conf
//  -l LOCALHOST, IP_SERVER over LAN, -U (only tcp)
//  -l 127.0.0.1,192.168.2.43 -U
//  service memcached restart
//  from another pc:
//  telnet server 11211
//  set variable_name 0 EXPIRE_TIME(sec) lengh_of_variable (17 in this case, no \r\n count)
//  set AAAA 0 300 17
//  aa,15,dsflkjsdk,d   <--- data
//  response: STORE if ok
//  get AAAA
//  response: VALUE AAAA 0 17
//  response: aa,15,dsflkjsdk,d

const char* ssid     = "************";         
const char* password = "***********************";
long conta;
long ini;
int intervallo=20; //seconds between server access
IPAddress server(192, 168, 2, 43);  //IP address of the memcached server

//WiFiClient client;

void setup()  {

  Serial.begin(9600);
  delay(10);
  Serial.println('\n');
  WiFi.mode(WIFI_STA);
  WiFi.hostname("prova");
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) 
    { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); 
    Serial.print(".");
    if (i>60)
     {
      ESP.restart();
     }
    }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  
  conta=0;
  ini=millis()/1000; 
}


void loop() {
  char msg[200] = "\0";
  unsigned long milli=millis();
  conta++;
  //set data for memcached server
  //using conta and milli
  sprintf(msg,"%ld,%ld,", conta,milli);
  int lun=strlen(msg);
  if ((millis()/1000-ini) > intervallo)
   {
   ini=millis()/1000;
   WiFiClient client;
   Serial.print("Connecting to memcached server... ");
   if (client.connect(server, 11211))
    {
    Serial.println("connected]");
    Serial.print("Sending data ");
    Serial.println(msg);
    
    client.print(String("set VAR_TEST 0 150000 ") + lun + " \r\n" + conta + "," + milli +",\r\n" );
    Serial.println("Response:");
    while (client.connected() || client.available())
    {
      if (client.available())
      {
        String line = client.readStringUntil('\n');
        Serial.println(line);
        client.print(String("quit") + "\r\n");
        client.stop();
      }
    }
    client.stop();
    Serial.println("\n[Disconnected]");
    }
    else
    {
    Serial.println("connection failed!]");
    client.stop();
    }
   }
  Serial.println("---mark---"); 
  delay(5000);
}
