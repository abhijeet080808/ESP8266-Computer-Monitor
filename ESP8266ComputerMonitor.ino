#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <TaskScheduler.h>
#include <limits>

const char* SSID = "<WIFI_AP_NAME>";
const char* PASSWORD = "<WIFI_AP_PASSWORD>";

// Make sure to allow ICMPv4 Ping in Windows Firewall
const IPAddress MONITOR_IP(192, 168, 0, 100);
const int MONITOR_DURATION_MS = 15000;
const int FAIL_THRESHOLD = 4;

const char* CONTROL_SERVER = "maker.ifttt.com";
const char* OFF_URL =
  "/trigger/computer_panel_off/with/key/<KEY>";

const int BUILTIN_LED1 = D4;   // GPIO2
const int BUILTIN_LED2 = D0;   // GPIO16

WiFiClientSecure client;

bool armed = false;
int pingFailCount = std::numeric_limits<int>::max();

Scheduler runner;
void pingTaskCallback();
Task pingTask(MONITOR_DURATION_MS, TASK_FOREVER, &pingTaskCallback);

void setup() {
  pinMode(BUILTIN_LED1, OUTPUT);
  pinMode(BUILTIN_LED2, OUTPUT);
  digitalWrite(BUILTIN_LED1, LOW);
  digitalWrite(BUILTIN_LED2, LOW);

  Serial.begin(115200);

  Serial.println(String(millis()) + " Connecting to WiFi network: " + SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  Serial.println(String(millis()) + " Connected to WiFi network with IP: " + WiFi.localIP().toString());
  digitalWrite(BUILTIN_LED1, HIGH);
  digitalWrite(BUILTIN_LED2, HIGH);

  runner.init();
  runner.addTask(pingTask);
  pingTask.enable();
}

void loop() {
  runner.execute();
}

void pingTaskCallback() {
  bool reachable = Ping.ping(MONITOR_IP);
  String ip = IPAddress(MONITOR_IP).toString();
  Serial.println(String(millis()) + " Ping " + ip + ": " + (reachable ? "UP" : "DOWN"));
  if (reachable) {
    pingFailCount = 0;
    digitalWrite(BUILTIN_LED1, LOW);
    delay(50);
    digitalWrite(BUILTIN_LED1, HIGH);
  } else {
    // prevent rollover
    if (pingFailCount < std::numeric_limits<int>::max()) {
      pingFailCount++;
    }
    Serial.println(String(millis()) + " Ping Fail Count: " + pingFailCount);
    digitalWrite(BUILTIN_LED2, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED2, HIGH);
  }

  if (armed == false) {
    if (pingFailCount == 0) {
      armed = true;
    }
  } else {
    // ping fails for FAIL_THRESHOLD consequtive times
    if (pingFailCount >= FAIL_THRESHOLD) {
      digitalWrite(BUILTIN_LED2, LOW);
      turnOffSwitch();
      digitalWrite(BUILTIN_LED2, HIGH);
    }
  }
}

void turnOffSwitch() {
      // time to turn off the switch
    if (client.connect(CONTROL_SERVER, 443)) {
      Serial.println(String(millis()) + " Connected to server: " + CONTROL_SERVER);
      // Make a HTTP request
      client.print(String("GET ") + OFF_URL + " HTTP/1.1\r\n" +
                   "Host: " + CONTROL_SERVER + "\r\n" +
                   // "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Connection: close\r\n\r\n");
      // Read HTTP response
      while (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(String(millis()) + " " + line);
      }
      client.stop();
      Serial.println(String(millis()) + " Disconnected from server");
    } else {
      Serial.println(String(millis()) + " Failed to connect to server: "+ CONTROL_SERVER);
    }
}
