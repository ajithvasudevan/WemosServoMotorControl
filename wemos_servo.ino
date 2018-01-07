#include <ESP8266WiFi.h>
 
const char* ssid = "<Wi-Fi SSID>";
const char* password = "<Password>";
 
int servoInputPin = D4;
int cycleTime = 19850;  // Should be 20000 microseconds, but compensating for other loop delays
int onTime = 1360;      // variable that actually sets the pulse-width of the output square wave (microseconds)
                        // 1360 is approximately the half-way point, i.e., 90 degrees.
int targetOnTime = 1360;  // temp variable for onTime
int stepInt = 25;        // when onTime is changed from t1 to t2, it is actually varied in steps of 'stepInt'
int pw0 = 525;          // Pulse-Width for 0 degrees
int pw180 = 2350;       // pulse-width for 180 degrees
int light = 1;          // Whether to enable light sensor or not (==1 => enabled, 0 => disabled)
int switchPin = D3;     // GPIO used to sense the position of an external (real) switch, to enable/disable light sensor 

int dir, equalsPos, firstSpacePos, secondSpacePos, clientWaitCount, deg, sensorValue;  
String pwStr, stepStr, pw0Str, pw180Str, degStr, lightStr;


WiFiServer server(80);   // Ceate the webserver object
 
void setup() {
  Serial.begin(115200);
  pinMode(servoInputPin, OUTPUT);    // setup servoInputPin as an output, to be used as input (square wave) for the servo motor
  pinMode(switchPin, INPUT_PULLUP);  // setup switchPin, to connect the external switch, as an input (other terminal of switch is connected to GND), but with an internal "pull-up" so that the pin is "HIGH" when the switch is OFF
  delay(10);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {  // Wait till Wi-Fi connects
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}


//long nw, dy, tm = 0;
void loop() {              // This gets called again and again, as fast as the loop contents allow

  // The next 5 lines change the 'onTime' value such that it moves towards 'targetOnTime' by one unit of 'stepInt'
  // 'onTime'value is not changed if it is equal to 'targetOnTime'
  // Setting 'stepInt' == 0 disables the stepped change of 'onTime'. In this case, 'onTime' changes instantly to 'targetOnTime'
  dir = stepInt;
  if (targetOnTime < onTime) dir = -1 * stepInt;
  if(abs(targetOnTime - onTime) < abs(dir)) dir = targetOnTime - onTime;
  if(targetOnTime != onTime) onTime+= dir;
  if(stepInt == 0) onTime = targetOnTime;

//  nw =  micros();
//  dy = nw - tm;
//  tm = nw;
//  Serial.println("************************************************************ ");
//  Serial.print(dy);

  digitalWrite(servoInputPin, HIGH);     // Start the pulse
  delayMicroseconds(onTime);             // wait for 'onTime' microseconds
  digitalWrite(servoInputPin, LOW);      // end the pulse
  delayMicroseconds(cycleTime - onTime); // wait for the remaining time for a total cycle time of ~20000 microseconds 


  light = digitalRead(switchPin);     // Read the position of the switch (it coud be 0 or 1)
  
  if(light == 1) {                  // this means the switch is in "enable light sensor' mode
    sensorValue = analogRead(A0);   // read the analog pin (where the LDR is connected). The value is in the range of 0-1024
//    Serial.print("A0 = ");
//    Serial.println(sensorValue);
    targetOnTime = map(sensorValue, 0, 1024, pw0, pw180);   // calculate 'targetOnTime' by translating and scaling the 'sensorValue' to the known limits of the scales
  }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  clientWaitCount = 0;
  while(!client.available()){
    clientWaitCount++;
    Serial.print("~");
    delay(10);
    if(clientWaitCount > 50) break;
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  if (request.indexOf("/pw=") != -1) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      pwStr = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + pwStr.indexOf("=");
      pwStr = pwStr.substring(equalsPos);
      targetOnTime = pwStr.toInt();
      deg = map(targetOnTime, pw0, pw180, 0, 180);
      Serial.println(pwStr);
      Serial.println(deg);
  } 
  if (request.indexOf("/step=") != -1) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      stepStr = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + stepStr.indexOf("=");
      stepStr = stepStr.substring(equalsPos);
      stepInt = stepStr.toInt();
      Serial.println(stepStr);
  } 
 
  if (request.indexOf("/pw0=") != -1) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      pw0Str = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + pw0Str.indexOf("=");
      pw0Str = pw0Str.substring(equalsPos);
      pw0 = pw0Str.toInt();
      Serial.println(pw0Str);
  } 

  if (request.indexOf("/pw180=") != -1) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      pw180Str = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + pw180Str.indexOf("=");
      pw180Str = pw180Str.substring(equalsPos);
      pw180 = pw180Str.toInt();
      Serial.println(pw180Str);
  } 

  if (request.indexOf("/light=") != -1) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      lightStr = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + lightStr.indexOf("=");
      lightStr = lightStr.substring(equalsPos);
      light = lightStr.toInt();
      Serial.println(lightStr);
  } 


  if (request.indexOf("/deg=") != -1 && (light == 0)) {
      firstSpacePos = 1 + request.indexOf(" ");
      secondSpacePos = request.lastIndexOf(" ");
      degStr = request.substring(firstSpacePos, secondSpacePos);
      equalsPos = 1 + degStr.indexOf("=");
      degStr = degStr.substring(equalsPos);
      deg = degStr.toInt();
      //targetOnTime = map(deg, 0, 180, pw0, pw180);   // applying a linear equation (found to be less accurate)
      targetOnTime = 9.84239 * deg + 509.625 + 57.54 * sin( (-0.526292/20 ) * deg);  // Applying a regression equation for more accurate values
      if(deg < 2 && deg >= 0) targetOnTime = 520;       // some correction at the beginning and end of the scale!
      if(deg > 178 && deg <= 180) targetOnTime = 2350;
      
      Serial.println(degStr);
      Serial.println(targetOnTime);
  } 


 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><body><font size='20'>");

  client.print("Current values:<br>light = ");
  client.print(light);
  client.println("<br>");
   
  client.print("deg = ");
  client.print(deg);
  client.println("<br>");
  
  client.print("pw = ");
  client.print(onTime);
  client.println("<br>");

  client.print("pw0 = ");
  client.print(pw0);
  client.println("<br>");

  client.print("pw180 = ");
  client.print(pw180);
  client.println("<br>");

  client.print("step = ");
  client.print(stepInt);
  client.println("<br><br>");
  
  client.println("Set <a href=\"/light=0\">light=0</a>&nbsp;&nbsp;&nbsp;<a href=\"/light=1\">light=1</a><br>");
  client.println("Set <a href=\"/step=0\">step=0</a>&nbsp;&nbsp;&nbsp;<a href=\"/step=1\">step=1</a>&nbsp;&nbsp;&nbsp;<a href=\"/step=5\">step=5</a>&nbsp;&nbsp;&nbsp;<a href=\"/step=10\">step=10</a>&nbsp;&nbsp;&nbsp;<a href=\"/step=25\">step=25</a>&nbsp;&nbsp;&nbsp;<a href=\"/step=100\">step=100</a><br>");
  client.println("Set <a href=\"/deg=0\">deg=0</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=30\">deg=30</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=45\">deg=45</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=60\">deg=60</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=90\">deg=90</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=120\"> deg=120</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=135\">deg=135</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=150\">deg=150</a>&nbsp;&nbsp;&nbsp;<a href=\"/deg=180\">deg=180</a><br>");
  client.println("Set <a href=\"/pw=1350\">pw=1350 (90 deg)</a><br>");
  client.println("Set <a href=\"/pw0=525\">pw0=525</a><br>");
  client.println("Set <a href=\"/pw180=525\">pw180=2350</a><br>");
  client.println("</font></body></html>");
 
//  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}
