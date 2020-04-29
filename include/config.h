typedef struct {
    float pressure;
    float humidity;
    float temperature;
    float light;
    bool  bme280_updated;
    bool  lightsensor_updated;
}sensor_data_t;



typedef struct {
    int     diff_pressure;
    int     diff_pressure_old;
    int     humidity;
    int     humidity_old;
    int     temperature_deci;
    int     temperature_deci_old;
    int     light;
    int     light_old;
    bool    pressure_updated;
    bool    humidity_updated;
    bool    temperature_updated;
    bool    light_updated;
    bool    reserved; 
    
}proc_data_t;

sensor_data_t _raw_ss[2];
proc_data_t _processed_ss;
