#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library


TFT_eSPI tft = TFT_eSPI();       // Invoke custom library



uint16_t x_data=0;
uint16_t y_data=0;

void display_init() {
    tft.init();
    tft.setRotation(0);
    

    tft.fillScreen(TFT_BLACK);
}



/********************************************************
 *  
 * Input
 * 
 * 
 * Output 
 *  x_out = x position of data
 *  y_out = y position of data
 * 
 ********************************************************/
    static inline void draw_data(uint16_t& x_out, uint16_t& y_out, const uint16_t& y, const uint16_t& color, const String& text,const int16_t& min, const int16_t& max, const int16_t& min_op, const int16_t& max_op, const String& unit, const int16_t& data, const int16_t& old_data, const uint8_t& decimal) {


      const uint16_t rec_x = 10;
      const uint16_t rec_y = y + 50;
      const uint16_t rec_h = 30;
      const uint16_t rec_w = 220;
      const uint16_t text_x = rec_x;
      const uint16_t text_h = 4;
      const uint16_t text_y = 20 + y;
      const String min_str = (min > 0 ? "+"+String(min):String(min));  //ใส่เครื่องหมาย
      const String max_str = (max > 0 ? "+"+String(max):String(max));  //ใส่เครื่องหมาย
      const String min_op_str = (min_op > 0 ? "+"+String(min_op):String(min_op));  //ใส่เครื่องหมาย
      const String max_op_str = (max_op > 0 ? "+"+String(max_op):String(max_op));  //ใส่เครื่องหมาย
      float pct=0;

      x_out = 230;
      y_out = text_y;
      



  
  /**************************
   * Draw border of gauge   |
   *************************/
      tft.setTextColor(TFT_CYAN, TFT_BLACK); 
      int padding = tft.textWidth("temperature", 4); // get the width of the text in pixels
      tft.setTextPadding(padding);    
      tft.drawString(text,text_x,text_y,text_h);
      tft.drawRect(rec_x, rec_y, rec_w, rec_h, color);
      
      tft.setTextPadding(0);





  /*******************
   * Data value       |
   *******************/
      float data_f = (float)(data/powf(10,decimal));
      float old_data_f = (float)old_data;
      const uint16_t data_x = rec_x+2;
      const uint16_t data_y = rec_y+2;
      const uint16_t data_h = rec_h-4;
      const uint16_t data_w = rec_w-4;
      const String data_str = (data > 0 ? "+"+String(data_f, decimal):String(data_f, decimal)) + unit;
      static float last_data = data_f;
      

  /***************************
   * erase or write new line |
   **************************/

    pct = (float)(data_f-min)/(max-min);
    int32_t targetpx = pct*data_w;
    pct = (float)(old_data_f-min)/(max-min);
    int32_t old_targetpx = pct*data_w;

    while( targetpx != old_targetpx ) {
        if( targetpx > old_targetpx ) {
            tft.drawLine(data_x+old_targetpx, data_y, data_x+old_targetpx, data_h+data_y, TFT_GREENYELLOW);
            old_targetpx++;
        }else{
            tft.drawLine(data_x+old_targetpx, data_y, data_x+old_targetpx, data_h+data_y, TFT_BLACK);
            old_targetpx--;
        }
    }

  /*******************
   * Write new       |
   *******************/
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      padding = tft.textWidth("999999", 4); // get the width of the text in pixels
      tft.setTextPadding(padding);
      tft.drawRightString( data_str.c_str(), x_out, y_out, 4);
      tft.setTextPadding(0);
      last_data = data_f;



  /*******************
   * Min max value   |
   *******************/
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawCentreString( min_str.c_str(), rec_x, rec_y+rec_h+5, 1);
      tft.drawCentreString( max_str.c_str(), rec_x+rec_w, rec_y+rec_h+5, 1);

  /*******************
   * Min optimized   |
   *******************/
      pct = (float)(min_op - min)/(max - min);
      tft.drawLine(rec_x+  (int16_t)(pct*rec_w), rec_y-3,  rec_x+  (int16_t)(pct*rec_w), rec_y+rec_h+3, color  );
      tft.drawCentreString( min_op_str.c_str(), rec_x+  (int16_t)(pct*rec_w), rec_y+rec_h+5, 1);
      
  /*******************
   * Max optimized   |
   *******************/
      pct = (float)(max_op - min)/(max - min);
      tft.drawLine(rec_x+  (int16_t)(pct*rec_w), rec_y-3,  rec_x+  (int16_t)(pct*rec_w), rec_y+rec_h+3, color  );
      tft.drawCentreString( max_op_str.c_str(), rec_x+  (int16_t)(pct*rec_w), rec_y+rec_h+5, 1);


}

