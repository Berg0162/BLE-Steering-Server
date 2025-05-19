#ifndef BLE STEERING_SERVER_H
#define BLE STEERING_SERVER_H

#include <arduino.h>
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>

class BLESteeringServer : public BLEServerCallbacks, public BLECharacteristicCallbacks, public BLEDescriptorCallbacks {
public:
    static BLESteeringServer& getInstance();

    void begin(void);
    bool updateSteeringValue(float angle);
    bool updateBatteryPercentage(uint8_t batteryPercentage);
    bool sendResponse(uint8_t *data, size_t size);
    bool pSteeringChar_Notify_Enabled = false;
    bool pTxChar_Indicate_Enabled = false; 
    bool pBatteryChar_Notify_Enabled = false; 
    bool isConnected = false;

    std::string toString(uint8_t macAddress[6]);

private:
    // Singleton
    BLESteeringServer() = default;
    BLESteeringServer(const BLESteeringServer&) = delete;
    BLESteeringServer& operator=(const BLESteeringServer&) = delete;

    // BLE internal objects
    BLEServer *pServer = nullptr;
    BLEService *pService = nullptr;
    BLECharacteristic *pSteeringChar = nullptr;
    BLE2902 *pSteeringChar2902 = nullptr;
    BLECharacteristic *pRxChar = nullptr;
    BLECharacteristic *pTxChar = nullptr;
    BLE2902 *pTxChar2902 = nullptr;
    BLEService *pBatteryService = nullptr;
    BLECharacteristic *pBatteryChar = nullptr;
    BLE2902 *pBatteryChar2902 = nullptr;

    float constrainSteerAngle(float angle);

    //  Presets 
    uint8_t defaultValue[4] = {0x0,0x0,0x0,0x0};                    // Default 4 byte Value zero
    uint8_t defaultValuepTxChar[4] = {0x03,0x10,0x12,0x34};         // Default 4 byte value
    uint8_t defaultBatteryLevel = 100;

};

#endif // BLE STEERING_SERVER_H