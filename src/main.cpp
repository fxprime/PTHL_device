#include "config.h"
#include "sensor.h"
#include "display.h"


void setup(void) {
  Serial.begin(115200);
  display_init();
  bme280_init();

}

void loop() {


  bme280_update();
  MAX44009_update();
  process_data();
  
  


  if(_processed_ss.pressure_updated) {
    int32_t data = _processed_ss.diff_pressure;
    static int32_t old_data = -50;
    draw_data(x_data, y_data, 0, TFT_CYAN, "DPressure", -50, +50, -30, -5, " Pa", data, old_data, 1 );
    old_data = data;
    _processed_ss.pressure_updated = 0;
  }
  if(_processed_ss.humidity_updated){
    int32_t data = _processed_ss.humidity;
    static int32_t old_data = 0;
    draw_data(x_data, y_data, 75, TFT_CYAN, "Humidity", 0, 100, 40, 60, " %", data, old_data ,0);
    old_data = data;
    _processed_ss.humidity_updated = 0;
  }
    
  if(_processed_ss.temperature_updated) {
    int32_t data = _processed_ss.temperature_deci;
    static int32_t old_data = 16;
    draw_data(x_data, y_data, 150, TFT_CYAN, "Temperature", 16, 45, 20, 25, " C", data, old_data ,1);
    old_data = data;
    _processed_ss.temperature_updated = 0;
  }
    
  if(_processed_ss.light_updated) {
    int32_t data = _processed_ss.light;
    static int32_t old_data = 0;
    draw_data(x_data, y_data, 225, TFT_CYAN, "Light", 0, 2000, 50, 750, " lux", data, old_data ,0);
    old_data = data;
    _processed_ss.light_updated = 0;
  }
    


}

