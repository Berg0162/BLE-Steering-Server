#include <BLESteeringServer.h>

float steeringAngle = 0.0;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); 
  Serial.flush();
  delay(800); // Give Serial I/O time to settle

  Serial.println();
  Serial.println("BLE Steering Controller Serial Input");
  Serial.println("Use keyboard input followed by Enter:");
  Serial.println("         l  -> turn left");
  Serial.println("         r  -> turn right");
  Serial.println("         c  -> center");
  Serial.println("-------------------------------------");

  BLESteeringServer::getInstance().begin();
}

void loop() {
  if (Serial.available()) {
    int input = Serial.read();
    switch (input) {
      case 108: // 'l' decimal
        steeringAngle = -15.0;
        break;
      case 114: // 'r' decimal
        steeringAngle = 15.0;
        break;
      case 99:  // 'c' decimal
        steeringAngle = 0.0;
        break;
      default:
        // Ignore
        return;
    }
    if(BLESteeringServer::getInstance().updateSteeringValue(steeringAngle)) {
        Serial.print("Sending steering angle: ");
        Serial.println(steeringAngle);
    }
  }
  delay(50);
}
