
#include <ArduinoJson.h>

#define CAR_1_FORWARD 11
#define CAR_1_RIGHT 8
#define CAR_1_BACKWARD 10
#define CAR_1_LEFT 9
#define CAR_2_FORWARD 4
#define CAR_2_RIGHT 6
#define CAR_2_BACKWARD 7
#define CAR_2_LEFT 5

String directionArray[4] = {"N", "E", "S", "W"};  // has to be strings and not chars as arduino json screams at me
short carControls[2][4] = {{CAR_1_FORWARD, CAR_1_RIGHT, CAR_1_BACKWARD, CAR_1_LEFT}, {CAR_2_FORWARD, CAR_2_RIGHT, CAR_2_BACKWARD, CAR_2_LEFT}};
bool carStates[2][4] = {{false, false, false, false}, {false, false, false, false}};

void controlStateEvent(StaticJsonDocument<300> &doc) {
  JsonArray docArray = doc["data"].as<JsonArray>();
  for (int i = 0; i < sizeof(carStates) / sizeof(carStates[0]); i++) {
    carStates[i][0] = docArray[i]["N"].as<boolean>();
    carStates[i][1] = docArray[i]["E"].as<boolean>();
    carStates[i][2] = docArray[i]["S"].as<boolean>();
    carStates[i][3] = docArray[i]["W"].as<boolean>();
  }
}

void transmitControls() {  // remember low == false and high == true
  for (int i = 0; i < sizeof(carStates) / sizeof(carStates[0]); i++) {
    for (int j = 0; j < sizeof(carStates[i]) / sizeof(carStates[i][0]); j++) {
      digitalWrite(carControls[i][j], carStates[i][j]);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
}

void loop() {
  transmitControls();
  if (Serial.available()) {
    // Allocate the JSON document
    StaticJsonDocument<300> doc;
    DeserializationError err = deserializeJson(doc, Serial);

    if (err == DeserializationError::Ok) {
      String event = doc["event"];

      if (event == "controls") {
        controlStateEvent(doc);
      }

    }
    else {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.read();
    }
  }
  delay(10);
}
