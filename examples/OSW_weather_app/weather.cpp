
#include "./apps/_experiments/weather.h"
#include "./services/OswServiceTaskWiFi.h"
#include <services/OswServiceTasks.h>
#include <HTTPClient.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include "./apps/examples/OpenWeatherParser.h"
#include "./fonts/DS_DIGI12pt7b.h"

/*
    TODO:   generalize decoding using coding options
            generalize decoding for multiple days
            select day 
            change location 
            Weather icons class
            
*/
bool red = false;

// gfx


void OswAppWeather::drawSun(int x, int y, int radius ){
  this->hal->gfx()->fillCircle(x, y, radius ,rgb888(255, 250, 1));
}


void OswAppWeather::drawMoon(int x, int y, int radius ){
    this->hal->gfx()->fillCircle(x, y, radius ,rgb888(255, 255, 255));
}
/*draw a cloud given: x coordinate and y coordinate.
  color: rgb888 color
  k: size coef. NOTE: if k is > 1 cloud may be partially out of the screen
*/
void OswAppWeather::drawCloud(int x, int y, uint32_t color, float k ){
    x = x+22;
    y=y+15;
    float radius = 15*k;
    float k1=12*k;
    float j1=5*k;
    float k2=6*k;
    float j2=1*k;
    float k3=14*k;
    //this->hal->gfx()->fillCircle(120, 45, 15 ,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x, y, radius ,color);
    //this->hal->gfx()->fillCircle(108, 50, 10 ,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x-k1, y+j1, radius-j1  ,color);
    //this->hal->gfx()->fillCircle(114, 44, 10 ,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x-k2, y-j2, radius-j1 ,color);
    //this->hal->gfx()->fillCircle(134, 50, 10 ,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+k3, y+j1, radius-j1 ,color);
}

void OswAppWeather::_drawDroplet(int x, int y, uint32_t color){
  this->hal->gfx()->drawLine(x, y, x+5, y+5, color);
  this->hal->gfx()->drawLine(x+1, y, x+6, y+5, color);
  this->hal->gfx()->drawLine(x+2, y, x+7, y+5, color);
}

void OswAppWeather::drawRain(int x, int y ){
    this->_drawDroplet(x,y);
    this->_drawDroplet(x+10,y+4);
    this->_drawDroplet(x+15, y);
}

void OswAppWeather::drawSnow(int x, int y, int level){
  switch (level)
  {
  case 1:
    this->hal->gfx()->fillCircle(x+10,y+3,3,rgb888(255,255 , 255));
    break;
  case 2:
    this->hal->gfx()->fillCircle(x,y+2,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+18,y+2,3,rgb888(255,255 , 255)); 
    break;
  case 3:
    this->hal->gfx()->fillCircle(x,y,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+10,y+5,3,rgb888(255,255 , 255));
    this->hal->gfx()->fillCircle(x+18,y,3,rgb888(255,255 , 255));
  default:
    break;
  }
}

void OswAppWeather::drawThunderBolt(int x, int y)
{
    x = x - 10;
    y = y - 3;
    this->hal->gfx()->drawLine(x, y, x + 3, y + 5, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 1, y, x + 4, y + 5, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 2, y, x + 5, y + 5, rgb888(255, 222, 40));

    this->hal->gfx()->drawLine(x + 4, y + 4, x + 7, y + 8, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 5, y + 4, x + 8, y + 9, rgb888(255, 222, 40));
    this->hal->gfx()->drawLine(x + 6, y + 4, x + 9, y + 10, rgb888(255, 222, 40));
}

void OswAppWeather::drawFog(int x, int y, int mist_fog)
{
    this->hal->gfx()->drawHLine(x, y, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 1, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 2, 22, rgb888(255, 255, 255));

    this->hal->gfx()->drawHLine(x, y + 5, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 6, 22, rgb888(255, 255, 255));
    this->hal->gfx()->drawHLine(x, y + 7, 22, rgb888(255, 255, 255));
    if (mist_fog > 1)
    {
        this->hal->gfx()->drawHLine(x, y + 10, 22, rgb888(255, 255, 255));
        this->hal->gfx()->drawHLine(x, y + 11, 22, rgb888(255, 255, 255));
        this->hal->gfx()->drawHLine(x, y + 12, 22, rgb888(255, 255, 255));
    }
}

void OswAppWeather::drawLayout(){
    this->hal->gfx()->setFont(nullptr);
    this->hal->gfx()->setTextCursor(120 , 215);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->print("last sync:");
    //arrows
    //up
    this->hal->gfx()->drawTriangle(180,50, 190,40, 200,50,rgb888(255,255,255));
    //down
    this->hal->gfx()->drawTriangle(180,190, 190,200, 200,190,rgb888(255,255,255));
    this->hal->gfx()->drawTriangle(40,190, 50,200, 50,180,rgb888(255,255,255));
    this->hal->gfx()->drawTriangle(65,190, 55,200, 55,180,rgb888(255,255,255));
    //separator
    this->hal->gfx()->drawVLine(75,60,110,rgb888(255,255,255));
}

void OswAppWeather::printLastUpdate(){
    this->hal->gfx()->setFont(nullptr);  
    this->hal->gfx()->setTextCursor(120 , 225);
    this->hal->gfx()->print(init_time_dd_mm_yyyy);
}

void OswAppWeather::printDate(){
    //TODO: multiple days-> scrolling menu
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);    
    this->hal->gfx()->setTextCursor(40 , 91);
    this->hal->gfx()->print(init_time_mm_dd);
    this->hal->gfx()->setTextCursor(40 , 120);
    time_t time = this->init_timestamp;
    time = time + (86400);
    strftime(this->date_mm_dd[1], sizeof(this->date_mm_dd[1]),"%d/%m",localtime(&time));
    this->hal->gfx()->print(date_mm_dd[1]);
    time = time + (86400);
    this->hal->gfx()->setTextCursor(40 , 149);
    strftime(this->date_mm_dd[2], sizeof(this->date_mm_dd[2]),"%d/%m",localtime(&time));
    this->hal->gfx()->print(date_mm_dd[2]);
}

bool OswAppWeather::loadData(){ 
    Serial.println("Print weather content: ");
    Serial.println(OswConfigAllKeys::weather.get());
    Serial.println("End of weather content");
    WeatherDecoder decoder(std::string(OswConfigAllKeys::weather.get().c_str()));
    headerData_t header = decoder.getHeader();
    this->forecast3days[0].weather = header.weather_init;
    this->forecast3days[0].temperature = header.temp_init -64;
    this->forecast3days[0].humidity = header.humidity_init*3.125;
    this->forecast3days[0].pressure = header.pressure_init + 850;
    this->init_timestamp = header.timestamp*4;//TODO: round 
    if(strftime(this->init_time_dd_mm_yyyy, sizeof(this->init_time_dd_mm_yyyy), "%d/%m/%Y", localtime(&this->init_timestamp))){
        Serial.println(this->init_time_dd_mm_yyyy);
    }
    if(strftime(this->init_time_mm_dd, sizeof(this->init_time_mm_dd), "%d/%m", localtime(&this->init_timestamp))){
        Serial.println(this->init_time_mm_dd);
    }
    weatherUpdate_t update = decoder.getNext();
    //TODO: generalize, not only 3 days with an update each 3h, but also different period between updates
    for(int i=1; i<24; i++){
        this->forecast3days[i].weather = update.weather;
        this->forecast3days[i].temperature = forecast3days[i-1].temperature +((update.temp + (0.25*update.temp_mantissa)) * (-1*!update.temp_sgn));
        this->forecast3days[i].humidity = update.humidity; 
        this->forecast3days[i].pressure = forecast3days[i-1].pressure + (update.pressure * (-1*!update.pressure_sgn));
        if(update.last_update){
            i=24;//TODO: handle this condition in a better way
        } else{
            update = decoder.getNext();
        }
    }
return true;
}


void OswAppWeather::drawWeatherIcon(){
    switch (this->forecast3days[this->updt_selector].weather)
    {//TODO: all the cases
    case 0:
        this->drawSun(120,45);//TODO: draw sun/moon 
        break;
    case 1:
        this->drawCloud(98,30);
        break;
    case 2:
        this->drawCloud(108-24,30,rgb888(254,254,254),1);
        this->drawCloud(98,30);
        break;
    case 3:
        this->drawCloud(108-24,30,rgb888(252,252,252),1);
        this->drawCloud(98,30,rgb888(254,254,254),1);
        break;
    case 4://mist
        this->drawFog(110,40,1);
        break;
    case 5://fog
        this->drawFog(110,40,2);
        break;
    case 6://snow min
        this->drawCloud(98,30);
        this->drawSnow(110,66,1);
        break;
    case 7://snow med
        this->drawCloud(98,30);
        this->drawSnow(110,66,2);
        break;
    case 8://snow high
        this->drawCloud(98,30);
        this->drawSnow(110,66,3);
        break;
    case 9://rain min
        this->drawCloud(98,30);
        this->drawRain(115,65);
        break;
    case 10://rain med
        this->drawCloud(108-24,30,rgb888(252,252,252),1);
        this->drawCloud(98,30,rgb888(254,254,254),1);
        this->drawRain(115,65);
        break;
    case 11://rain heavy
        this->drawCloud(108-24,30,rgb888(250,250,250),1);
        this->drawCloud(98,30,rgb888(252,252,252),1);
        this->drawRain(115,65);
        break;
    case 12://thunderstorm
        this->drawCloud(108-24,30,rgb888(252,252,252),1);
        this->drawCloud(98,30,rgb888(254,254,254),1);
        this->drawRain(115,65);
        this->drawThunderBolt(115,65);
        break;
    case 13://thunderstorm heavy 
        this->drawCloud(108-24,30,rgb888(250,250,250),1);
        this->drawCloud(98,30,rgb888(252,252,252),1);
        this->drawRain(115,65);
        this->drawThunderBolt(115,65);
        break;
    case 14://squall or tornado 
        this->hal->gfx()->setTextCursor(120,30);
        this->hal->gfx()->print("sqall/tornado!!");
    default:
        //unknown
        this->hal->gfx()->setTextCursor(120,30);
        this->hal->gfx()->print("?");
        break;
    }
}


void OswAppWeather::drawWeather(){
    updt_time = init_timestamp + (this->updt_selector * 10800 );
    this->drawWeatherIcon();
    //draw time of current weather update
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);
    this->hal->gfx()->setTextSize(1);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->setTextCursor(200 , 119);
    strftime(this->time_updt,sizeof(this->time_updt),"%H:%M",localtime(&updt_time));
    this->hal->gfx()->print(this->time_updt);
    //weather data 
    sprintf(this->buffer,"t:%3.2f",this->forecast3days[this->updt_selector].temperature);
    this->hal->gfx()->setTextCursor(120 , 90);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer,"H:%d%%",this->forecast3days[this->updt_selector].humidity);
    this->hal->gfx()->setTextCursor(120 , 119);
    this->hal->gfx()->print(buffer);

    sprintf(this->buffer, "p:%d",this->forecast3days[this->updt_selector].pressure);
    this->hal->gfx()->setTextCursor(120 , 148);
    this->hal->gfx()->print(buffer);
}



void OswAppWeather::setup() {
    this->loadData();
}

void OswAppWeather::loop() {
    if (hal->btnHasGoneDown(BUTTON_2)) {
        if(this->updt_selector<23){
            this->updt_selector++;
        }
    }
    if (hal->btnHasGoneDown(BUTTON_3)) {
        if(this->updt_selector>0){
            this->updt_selector--;
         }
    }
    this->drawLayout();
    this->drawWeather();   
    this->printDate();
    this->printLastUpdate();
    this->hal->requestFlush(); 
}

void OswAppWeather::stop() {

}
