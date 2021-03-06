#ifndef SDP3X_H
#define SDP3X_H

// Utility functions for accessing the Sensirion Differential Pressure Sensor SDP3x
// Datasheet here: https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/8_Differential_Pressure/Sensirion_Differential_Pressure_Sensors_SDP3x_Digital_Datasheet_V0.8.pdf

// Currently configured for the SDP31 sensor

typedef enum {
  SDP_SUCCESS = 0,
  SDP_ERROR_NO_RESPONSE = 1,
  SDP_ERROR_WRONG_RESPONSE_LENGTH = 2,
  SDP_ERROR_BAD_CRC = 3
} SDP3xResponse;

class SDP3x
{
  private:
  int I2cAddress;
  
  public:
    void setup(int i2cAddress);
    SDP3xResponse readSensor(float &temperature, float &pressure);

  private:
    float convertTemperature(uint16_t ticks);
    float convertPressure(uint16_t ticks);
    bool checkCrc(const uint8_t* data, uint8_t size, uint8_t checksum);
    inline uint16_t convertBytes(uint8_t* data); // convert two 8 bit values to one word
};
#endif


