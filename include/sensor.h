#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)
uint8_t BME280_ADS[2] ={ 0x76, 0x77};

Adafruit_BME280 bme[2]; // I2C

#define BME280_USAGE 2

unsigned long bme280_delayTime;
void bme280_init() {
    Serial.println("Initing BME280");

    for (size_t i = 0; i < BME280_USAGE; i++)
    {
        
        if (! bme[i].begin(BME280_ADS[i], &Wire)) {
            Serial.println("Could not find a valid BME280 sensor id= " + String(i)+ ", address = " + String(BME280_ADS[i]) + " check wiring!");
            while (1);
        }

        // indoor navigation
        Serial.println("-- Indoor Navigation Scenario --");
        Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
        Serial.println("0.5ms standby period, filter 16x");
        bme[i].setSampling(Adafruit_BME280::MODE_NORMAL,
                        Adafruit_BME280::SAMPLING_X2,  // temperature
                        Adafruit_BME280::SAMPLING_X16, // pressure
                        Adafruit_BME280::SAMPLING_X1,  // humidity
                        Adafruit_BME280::FILTER_X16,
                        Adafruit_BME280::STANDBY_MS_0_5 );
    }
    
    
    
    // suggested rate is 25Hz
    // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
    // T_ovs = 2
    // P_ovs = 16
    // H_ovs = 1
    // = 40ms (25Hz)
    // with standby time that should really be 24.16913... Hz
    bme280_delayTime = 41;

    Serial.println("BME280 inited.");
}


static inline void bme280_update() {
    static uint32_t last_update = millis();
    uint32_t t_now = millis();
    if( t_now - last_update > bme280_delayTime ) {
        last_update = t_now;
        
        for (size_t i = 0; i < BME280_USAGE; i++)
        {
            _raw_ss[i].pressure = bme[i].readPressure();
            _raw_ss[i].humidity = bme[i].readHumidity();
            _raw_ss[i].temperature = bme[i].readTemperature();
            _raw_ss[i].bme280_updated = 1;
        }
    }
    // Serial.println( String(_raw_ss[0].humiditcy) + " ,,,, " + String(_raw_ss[1].humidity) );
    
    // _processed_ss.pressure = _raw_ss[1].pressure 
}




#define MAX44009_ADDR 0x4A



void MAX44009_init() {
    Serial.println("Initing MAX44009");
    Wire.beginTransmission(MAX44009_ADDR);
    Wire.write(0x02);
    Wire.write(0x40);
    Wire.endTransmission();
    Serial.println("MAX44009 inited");
}


static inline void MAX44009_update() {
    static uint32_t last_update = millis();

    uint32_t t_now = millis();
    if(t_now - last_update > 20) {
        last_update = t_now;

        unsigned int data[2];
        Wire.beginTransmission(MAX44009_ADDR);
        Wire.write(0x03);
        Wire.endTransmission();

        // Request 2 bytes of data
        Wire.requestFrom(MAX44009_ADDR, 2);

        // Read 2 bytes of data luminance msb, luminance lsb
        if (Wire.available() == 2)
        {
        data[0] = Wire.read();
        data[1] = Wire.read();
        }

        // Convert the data to lux
        int exponent = (data[0] & 0xF0) >> 4;
        int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
        float luminance = pow(2, exponent) * mantissa * 0.045;
        _raw_ss[0].lightsensor_updated = 1;
        _raw_ss[0].light = luminance;
    }
    

}












static inline void process_data() {
    if(_raw_ss[0].bme280_updated && _raw_ss[1].bme280_updated) {
        _raw_ss[0].bme280_updated = 0;
        _raw_ss[1].bme280_updated = 0;
        _processed_ss.diff_pressure_old = _processed_ss.diff_pressure;
        _processed_ss.temperature_deci_old = _processed_ss.temperature_deci;
        _processed_ss.humidity_old = _processed_ss.humidity;
        _processed_ss.diff_pressure = _raw_ss[1].pressure - _raw_ss[0].pressure;
        _processed_ss.temperature_deci = 10.0*_raw_ss[0].temperature;
        _processed_ss.humidity = _raw_ss[0].humidity;
        if( abs(_processed_ss.diff_pressure_old - _processed_ss.diff_pressure) ) _processed_ss.pressure_updated = true;
        if( abs(_processed_ss.temperature_deci_old - _processed_ss.temperature_deci) ) _processed_ss.temperature_updated = true;
        if( abs(_processed_ss.humidity_old - _processed_ss.humidity) ) _processed_ss.humidity_updated = true;
    }
    if(_raw_ss[0].lightsensor_updated) {
        _raw_ss[0].lightsensor_updated = 0;
        _processed_ss.light_old = _processed_ss.light;
        _processed_ss.light = _raw_ss[0].light;
        if( abs(_processed_ss.light_old - _processed_ss.light) ) _processed_ss.light_updated = true;
    }
}

