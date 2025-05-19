#include "BLESteeringServer.h"

// -------------------------------------------------------------------------------------------
// COMPILER DIRECTIVE to allow/suppress DEBUG messages that help debugging...
// Uncomment general "#define DEBUG" to activate for this class
#define DEBUG
// Include these debug utility macros in all cases!
#include "DebugUtils.h"
// -------------------------------------------------------------------------------------------

// BLE Definitions
#define STEERING_DEVICE_UUID      BLEUUID("347b0001-7635-408b-8918-8ff3949ce592")
#define STEERING_ANGLE_CHAR_UUID  BLEUUID("347b0030-7635-408b-8918-8ff3949ce592")    // notify
#define STEERING_RX_CHAR_UUID     BLEUUID("347b0031-7635-408b-8918-8ff3949ce592")    // write
#define STEERING_TX_CHAR_UUID     BLEUUID("347b0032-7635-408b-8918-8ff3949ce592")    // indicate

#define UUID16_SVC_BATTERY        BLEUUID((uint16_t)0x180F)
#define UUID16_CHR_BATTERY_LEVEL  BLEUUID((uint16_t)0x2A19)

// Steering defines
#define MAX_STEER_ANGLE (35.0F)
#define STEERANGLETHRESHOLD (1.5F)

/* -------------------------------------------------------------------------------------------
* Short Device Name --------------------------------------------------------------------------
* --------------------------------------------------------------------------------------------
* This project is named: "Open Virtual Steering", too long to be suitable for BLE device 
* naming and advertising. A made-up short device name is default inserted.
* However, users may need to configure the short device name manually for interoperability on 
* certain platforms to allow for instant steering device recognition and full functionality.  
* For general information on BLE naming conventions, 
*                    refer to https://www.bluetooth.com/specifications/specs/ 
* -------------------------------------------------------------------------------------------*/
#define SHORTDEVICENAME "STERZOPN"  // String length is max 8
// -------------------------------------------------------------------------------------------

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
#ifdef DEBUG
      // Get some connection parameters of the peer device.
      uint8_t address[6];
      memcpy(&address, param->connect.remote_bda, 6);
      std::string full_addr = BLESteeringServer::getInstance().toString(address);
      DEBUG_PRINTF("Server connected to Client with MAC Address: [%s]\n", full_addr.c_str());
#endif
      BLEDevice::stopAdvertising();
      BLESteeringServer::getInstance().isConnected = true;
    };

    void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
#ifdef DEBUG
      // Get some connection parameters of the peer device.
      uint8_t address[6];
      memcpy(&address, param->connect.remote_bda, 6);
#endif
      if(BLESteeringServer::getInstance().pSteeringChar_Notify_Enabled == true) {
         DEBUG_PRINTLN("Central Notify Disabled SteeringChar (30)");
         BLESteeringServer::getInstance().pSteeringChar_Notify_Enabled = false;
      }
      if(BLESteeringServer::getInstance().pTxChar_Indicate_Enabled == true) {
         DEBUG_PRINTLN("Central Indicate Disabled TxChar (32)");
         BLESteeringServer::getInstance().pTxChar_Indicate_Enabled = false;
      }
      if(BLESteeringServer::getInstance().pBatteryChar_Notify_Enabled == true) {
         DEBUG_PRINTLN("Central Notify Disabled BatteryChar (0x2A19)");
         BLESteeringServer::getInstance().pBatteryChar_Notify_Enabled = false;
      }
#ifdef DEBUG
      std::string full_addr = BLESteeringServer::getInstance().toString(address);
      DEBUG_PRINTF("Server disconnected from Client with MAC Address: [%s]\n", full_addr.c_str());
#endif
      BLESteeringServer::getInstance().isConnected = false;
      BLEDevice::startAdvertising();
    }
};

class pRxCharCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
        String rxDataString = pCharacteristic->getValue();
        uint8_t RxDataLen = rxDataString.length();    // Get the actual length of data bytes
        uint8_t RxData[RxDataLen];
        memset(RxData, 0, RxDataLen); // set to zero
        // Display the raw request packet actual length
        DEBUG_PRINTF("pRxChar (31) [Len: %d] [Data: ", RxDataLen);
        // Transfer the contents of data to RxData for further processing!
        for (int i = 0; i < RxDataLen; i++) {
            RxData[i] = rxDataString[i];
            // Display the raw request packet byte by byte in HEX
            DEBUG_PRINTF("%02X ", RxData[i], HEX);
        }
        DEBUG_PRINTLN("]"); 
  } // onWrite
}; // pRxCharCallbacks 

class SteeringChar2902Callbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor* pDescriptor) {
    uint8_t *value = pDescriptor->getValue();
    if(value[0] == 0) {
      DEBUG_PRINTLN("Central Notify Disabled SteeringChar (30)");
      BLESteeringServer::getInstance().pSteeringChar_Notify_Enabled = false;
    } else {
      DEBUG_PRINTLN("Central Notify Enabled SteeringChar (30)");
      BLESteeringServer::getInstance().pSteeringChar_Notify_Enabled = true;
    }
  }
};

class TxChar2902Callbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor* pDescriptor) {
    uint8_t *value = pDescriptor->getValue();
    if(value[0] == 0) {
      DEBUG_PRINTLN("Central Indicate Disabled TxChar (32)");
      BLESteeringServer::getInstance().pTxChar_Indicate_Enabled = false;
    } else {
      DEBUG_PRINTLN("Central Indicate Enabled TxChar (32)");
      BLESteeringServer::getInstance().pTxChar_Indicate_Enabled = true;
    }
  }
};

class BatteryChar2902Callbacks : public BLEDescriptorCallbacks {
  void onWrite(BLEDescriptor* pDescriptor) {
    uint8_t *value = pDescriptor->getValue();
    if(value[0] == 0) {
      DEBUG_PRINTLN("Central Notify Disabled BatteryChar (0x2A19)");
      BLESteeringServer::getInstance().pBatteryChar_Notify_Enabled = false;
    } else {
      DEBUG_PRINTLN("Central Notify Enabled BatteryChar (0x2A19)");
      BLESteeringServer::getInstance().pBatteryChar_Notify_Enabled = true;
      BLESteeringServer::getInstance().updateBatteryPercentage((uint8_t)100); // Notify pBatteryChar first time 
    }
  }
};

// BLESteeringServer Class Members----------------------------------------------------------------------------------------

BLESteeringServer& BLESteeringServer::getInstance() {
    static BLESteeringServer instance;
    return instance;
}

void BLESteeringServer::begin(void) {
    BLEDevice::init(SHORTDEVICENAME);
    DEBUG_PRINTLN("Creating Bluedroid Server...");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    // Create the Steering Device Service (01)
    DEBUG_PRINTLN("Define Steering Device Service...");
    pService = pServer->createService(STEERING_DEVICE_UUID);
    // Create Steering Device Characteristics (30)
    pSteeringChar = pService->createCharacteristic(STEERING_ANGLE_CHAR_UUID, BLECharacteristic::PROPERTY_NOTIFY);
    pSteeringChar2902 = new BLE2902();
    pSteeringChar2902->setCallbacks(new SteeringChar2902Callbacks()); 
    pSteeringChar->addDescriptor(pSteeringChar2902);
    pSteeringChar->setValue(defaultValue, 4); // Set initial value
    // Create pTxChar Device Characteristics (32)
    pTxChar = pService->createCharacteristic(STEERING_TX_CHAR_UUID, BLECharacteristic::PROPERTY_INDICATE); 
    pTxChar2902 = new BLE2902();
    pTxChar2902->setCallbacks(new TxChar2902Callbacks()); 
    pTxChar->addDescriptor(pTxChar2902);
    pTxChar->setValue(defaultValuepTxChar, 4); // Set initial value
    // Create pRxChar Device Characteristics (31)
    pRxChar = pService->createCharacteristic(STEERING_RX_CHAR_UUID, BLECharacteristic::PROPERTY_WRITE);
    pRxChar->setCallbacks(new pRxCharCallbacks());
    pRxChar->setValue(defaultValue, 4); // Set initial value
    // Start the Steering Device service (01)
    pService->start();

    // Create the Battery Service
    DEBUG_PRINTLN("Define Battery Service...");
    pBatteryService = pServer->createService(UUID16_SVC_BATTERY );
    pBatteryChar = pBatteryService->createCharacteristic(UUID16_CHR_BATTERY_LEVEL, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pBatteryChar2902 = new BLE2902();
    pBatteryChar2902->setCallbacks(new BatteryChar2902Callbacks()); 
    pBatteryChar->addDescriptor(pBatteryChar2902);
    pBatteryChar->setValue(&defaultBatteryLevel, 1); // Default 100% battery
    pBatteryService->start();

    // Setup Advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(STEERING_DEVICE_UUID);
    BLEDevice::startAdvertising();
    DEBUG_PRINTLN("Start Advertising...");  
}

// Uppercase alternative to Address <string>
std::string BLESteeringServer::toString(uint8_t macAddress[6]) {
  char stringMAC[18]; // 6*2 bytes + 5 colons + 1 null terminator = 18
  // Undo Little Endian machine-representation
  snprintf(stringMAC, sizeof(stringMAC), "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[5], macAddress[4], \
           macAddress[3], macAddress[2], macAddress[1], macAddress[0]); // byte by byte in HEX --> UPPERCASE
  return std::string(stringMAC);
}

float BLESteeringServer::constrainSteerAngle(float angle) {
    if (fabs(angle) > MAX_STEER_ANGLE) { 
      if (angle > 0) { angle = MAX_STEER_ANGLE; }
      else { angle = - MAX_STEER_ANGLE; }
    }
    if (fabs(angle) < STEERANGLETHRESHOLD) { angle= 0.0; }
    return angle;
}

bool BLESteeringServer::updateSteeringValue(float angle) {
    if (pSteeringChar_Notify_Enabled) {
        float steerAngle = constrainSteerAngle(angle);
        pSteeringChar->setValue((uint8_t*)&steerAngle, 4); // Float stored in array of 4 bytes
        pSteeringChar->notify();
        return true;
    }
    return false;
}
bool BLESteeringServer::updateBatteryPercentage(uint8_t batteryPercentage) {
    if (pBatteryChar_Notify_Enabled) {
        pBatteryChar->setValue(&batteryPercentage, 1); // uint8_t stored in array of 1 byte
        pBatteryChar->notify(); 
        DEBUG_PRINTF("Updated Battery Level: %3d%%\n", batteryPercentage);
        return true;
    }
    return false;
}

bool BLESteeringServer::sendResponse(uint8_t *data, size_t size) {
    if (pTxChar_Indicate_Enabled) {
        pTxChar->setValue(data, size);
        pTxChar->indicate(); 
        return true;
    }
    return false;
}


