#ifndef OSW_APP_WEATHER
#define OSW_APP_WEATHER

#include <osw_hal.h>

#include "osw_app.h"
class OswAppWeather : public OswApp {
  public:
    OswAppWeather(void) {};
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    void get_w();
    void print_w();
    void drawLayout();
    bool loadData();
    void drawDate();
    void drawWeather();
    void drawWeatherIcon();
    ~OswAppWeather() {};
    void drawRain( int x, int y );
    void drawSnow( int x, int y, int level);
    void drawCloud(int x, int y, uint32_t color=rgb888(255, 255, 255),float k=1);
    void drawSun(int x, int y, int radius = 15);
    void drawMoon(int x, int y, int radius = 15);
    void drawFog(int x, int y, int mist_fog);
    void drawThunderBolt(int x, int y);
    void _drawDroplet(int x, int y, uint32_t color = rgb888(255, 255, 255));
    bool displayBufferDisabled = false;
    bool get_finish = false ;
    //TODO: store root certificate in NVS ?
    typedef struct{
      uint8_t weather;
      float temperature;
      int humidity;
      int pressure;
    }weather_t;
    uint8_t main_selector = 0; 
    uint8_t day_selector = 0;
    uint8_t place_selector = 0;
    uint8_t updt_selector = 0;
    OswHal* hal = OswHal::getInstance();
    weather_t forecast3days[24];//one update each 3h
    time_t init_timestamp;
    char init_time_mm_dd[6];
    char init_time_dd_mm_yyyy[11];
    char time_updt[6];
    char buffer[40];
    time_t updt_time;
};

#endif
