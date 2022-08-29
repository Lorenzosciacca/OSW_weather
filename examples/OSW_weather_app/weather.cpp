
#include "./apps/_experiments/weather.h"
#include "./services/OswServiceTaskWiFi.h"
#include <services/OswServiceTasks.h>
#include <HTTPClient.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include "./apps/examples/OpenWeatherParser.h"
#include "./fonts/DS_DIGI12pt7b.h"

#define OPENWEATHERMAP_APIKEY "5643586bde5db6443716d934ced6c66a"
#define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/forecast?"
#define OPENWEATHERMAP_CITY "alessandria"
#define OPENWEATHERMAP_COUNTRY "IT"
/*
    TODO:   multiple city support
            Weather icons class
            enhance decoder ( sync frame )
            
*/


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

void OswAppWeather::drawWeatherIcon(){
    switch (this->forecast3days[this->updt_selector].weather)
    {
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
    //draw time of current weather updatea
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);
    this->hal->gfx()->setTextSize(1);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->setTextCursor(200 , 119);
    strftime(this->time_updt,sizeof(this->time_updt),"%H:%M",localtime(&updt_time));
    this->hal->gfx()->print(this->time_updt);
    if(this->main_selector==1){
        //this->hal->gfx()->drawHLine(170,132,58,rgb888(255,255,255));
        this->hal->gfx()->drawThickLine(170,132,228,132,2,rgb565(164, 35, 52));
    }
    //time struct to get the day associated to the incremented timestamp updt_time
    tm* tm_1;

    tm* tm_2;
    tm_1 = localtime(&updt_time);
    time_t time = this->init_timestamp;
    time = time + 86400;
    tm_2 = localtime(&time);


    // if(tm_1->tm_yday==this->tm_init->tm_yday){
    //     this->hal->gfx()->fillCircle(4,90,3,rgb888(255,255 , 255));
    // }

    // if(tm_1->tm_yday==tm_2->tm_mday){
    //     this->hal->gfx()->fillCircle(4,120,3,rgb888(255,255 , 255));
    // }
    time = time + 86400;
    tm_2 = localtime(&time);
    // if(tm_1->tm_yday==tm_2->tm_mday){
    //   //  Serial.println(tm_2->tm_mday);
    //     this->hal->gfx()->fillCircle(4,148,3,rgb888(255,255 , 255));
    // }else{
    //     if(tm_1->tm_mday!=tm_2->tm_mday){
    //       //  Serial.println(tm_2->tm_mday);
    //     }
    // }

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

void OswAppWeather::drawRefreshIcon(uint16_t color){
    this->hal->gfx()->fillCircle(185,152,12, color);
    this->hal->gfx()->fillCircle(185,152,8,rgb565(0,0,0));
    this->hal->gfx()->drawThickLine(185,157,200,157,2,rgb565(0,0,0));
    this->drawTriangleThick(194,154, 199,154, 199,149,2, color);
}


void OswAppWeather::drawLayout(){
    this->hal->gfx()->setFont(nullptr);
    this->hal->gfx()->setTextCursor(120 , 215);
    this->hal->gfx()->setTextMiddleAligned();
    this->hal->gfx()->setTextCenterAligned();
    this->hal->gfx()->print("last sync:");
    if(this->main_selector!=2){
         //up
        this->drawTriangleThick(180,50, 190,40, 200,50,4,rgb888(255,255,255));
        //down
        this->drawTriangleThick(180,190, 190,200, 200,190,4,rgb888(255,255,255));
    }
    //<-
    this->drawTriangleThick(37,190, 45,198, 45,182,4,rgb565(100,100,100));
    //->
    this->drawTriangleThick(68,190, 60,198, 60,182,4,rgb888(255,255,255));
    //separator
    this->hal->gfx()->drawThickLine(78, 60,78,168,2,rgb565(164, 35, 52));
    if(this->main_selector!=2){
        this->drawRefreshIcon(rgb565(255,255,255));
    }else{
        this->drawRefreshIcon(rgb565(164, 35, 52));

    }
}

void OswAppWeather::drawTriangleThick(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t radius,uint16_t color){
    this->hal->gfx()->drawThickLine(x0, y0, x1, y1, radius, color);
    this->hal->gfx()->drawThickLine(x1, y1, x2, y2, radius, color);
    this->hal->gfx()->drawThickLine(x2, y2, x0, y0, radius, color);
}


void OswAppWeather::printLastUpdate(){
    this->hal->gfx()->setFont(nullptr);  
    this->hal->gfx()->setTextCursor(120 , 225);
    this->hal->gfx()->print(init_time_dd_mm_yyyy);
}

void OswAppWeather::weatherRequest(){
    WiFiClientSecure *client = new WiFiClientSecure ;
    client->setCertificate(this->rootCACertificate);
    HTTPClient http;
    http.begin("https://api.openweathermap.org/data/2.5/forecast?lat=44.91837743102328&lon=8.596110056689&appid=5643586bde5db6443716d934ced6c66a&cnt=24");
    int code = http.GET();
    int size = http.getSize();
    float temp;
    http.end();
    delete client;
    OswServiceAllTasks::wifi.disconnectWiFi();
    if (code > 0){
        DynamicJsonDocument doc(16432);
        deserializeJson(doc,http.getStream());
        temp = doc["list"][0]["main"]["temp"];
        codingOpt_t options;
        options.delta=3;
        options.encode_humidity = true;
        options.encode_pressure = true;
        options.encode_temp = true;
        options.encode_weather = true;
        options.encode_pressure_long = true;
        options.encode_temp_long = true;
        OpenWeatherParser pars(options);
        string encoded = pars.encodeWeather(doc);
        OswConfig::getInstance()->enableWrite();
        OswConfigAllKeys::weather.set(encoded.c_str());
        OswConfig::getInstance()->disableWrite();
        // wEncoder.setUpdate(updt_);
        // Serial.println("Updated");
        // string encoded = wEncoder.getEncoded();
        // Serial.println("get encoded");
        // OswConfigAllKeys::weather.set(encoded.c_str());
        // Serial.println("Weather updated");

        
    }
}



  
void OswAppWeather::getDayList(int n_updates){
    time_t timestamp = this->init_timestamp;
    tm* time_current;
    int mday_prev;
    char date_buff[6];
    time_current = localtime(&this->init_timestamp);
    mday_prev = time_current->tm_mday;
    strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&this->init_timestamp));
    this->day_first_updt.push_back(0);
    for(int i=1; i<24; i++){
        timestamp = timestamp + 10800;//TODO: generalize, not only 3h between updates
        mday_prev = time_current->tm_mday;
        time_current = localtime(&timestamp);
        Serial.println(time_current->tm_mday);
        if (time_current->tm_mday != mday_prev){
            strftime(date_buff, sizeof(date_buff), "%d/%m", localtime(&timestamp));
            day_first_updt.push_back(i);
            Serial.printf("%s\n",date_buff);
        }
    }
}


void OswAppWeather::printDate(){
    // //TODO: multiple days-> scrolling menu  
    time_t time;
    char date_buf[6];
    this->hal->gfx()->fillCircle(4,120,3,rgb888(255,255 , 255));
    if(this->day_first_updt.size()!=0){
        if(this->updt_selector < this->day_first_updt[1]){
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40 , 91);
            this->hal->gfx()->print("---");
        }else{
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            this->hal->gfx()->setTextCursor(40 , 91);
            time = this->updt_time - 86400;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
        
        if(this->updt_selector >= this->day_first_updt[this->day_first_updt.size()-1]){
            this->hal->gfx()->setFont(nullptr);
            this->hal->gfx()->setTextCursor(40 , 149);
            this->hal->gfx()->print("---");
        }else{
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            time = this->updt_time + 86400;
            this->hal->gfx()->setTextCursor(40 , 149);
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
        if(this->day_first_updt.size()!=0){
            this->hal->gfx()->setFont(&DS_DIGI12pt7b); 
            this->hal->gfx()->setTextCursor(40 , 120);
            time = this->updt_time;
            strftime(date_buf, sizeof(date_buf), "%d/%m", localtime(&time));
            this->hal->gfx()->print(date_buf);
        }
    }else{
        this->hal->gfx()->setFont(nullptr); 
        this->hal->gfx()->setTextCursor(40 , 120);
        this->hal->gfx()->print("no data");
    }
    
    this->hal->gfx()->setFont(&DS_DIGI12pt7b);  


    // this->hal->gfx()->setTextCursor(40 , 91);
    if(this->main_selector==0){
        //this->hal->gfx()->drawHLine(10,132,58,rgb888(255,255,255));
        this->hal->gfx()->drawThickLine(10,132,68,132,2,rgb565(164, 35, 52));
    }
    
    // this->hal->gfx()->print(init_time_mm_dd);
    // this->hal->gfx()->setTextCursor(40 , 120);
    // time_t time = this->init_timestamp;
    // time = time + (86400);//TODO: it's wrong !
    // strftime(this->date_mm_dd[1], sizeof(this->date_mm_dd[1]),"%d/%m",localtime(&time));
    // this->hal->gfx()->print(date_mm_dd[1]);
    // time = time + (86400);
    // this->hal->gfx()->setTextCursor(40 , 149);
    // strftime(this->date_mm_dd[2], sizeof(this->date_mm_dd[2]),"%d/%m",localtime(&time));
    // this->hal->gfx()->print(date_mm_dd[2]);



}

bool OswAppWeather::loadData(){ 
    Serial.println("Print weather content: ");
    Serial.println(OswConfigAllKeys::weather.get());
    Serial.println("End of weather content");
    WeatherDecoder decoder(std::string(OswConfigAllKeys::weather.get().c_str()));
    headerData_t header = decoder.getHeader();
    this->forecast3days[0].weather = header.weather_init;
    this->forecast3days[0].temperature = header.temp_init - 64;
    this->forecast3days[0].humidity = header.humidity_init * 3.125;
    this->forecast3days[0].pressure = header.pressure_init + 850;
    this->init_timestamp = header.timestamp * 4;
    tm* tmx;
    this->getDayList();
    tmx = localtime(&this->init_timestamp);
    Serial.printf("day :%d\n",tmx->tm_mday);
    if(strftime(this->init_time_dd_mm_yyyy, sizeof(this->init_time_dd_mm_yyyy), "%d/%m/%Y", localtime(&this->init_timestamp))){
        Serial.println(this->init_time_dd_mm_yyyy);
    }
    if(strftime(this->init_time_mm_dd, sizeof(this->init_time_mm_dd), "%d/%m", localtime(&this->init_timestamp))){
        Serial.println(this->init_time_mm_dd);
    }
    tm_init = localtime(&this->init_timestamp);
    weatherUpdate_t update = decoder.getNext();
    //TODO: generalize, not only 24 updates with an update each 3h, but also different period between updates
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

int OswAppWeather::getNextDay(){
    for(int i=0; i<this->day_first_updt.size()-1;i++){
        if(this->day_first_updt[i] > this->updt_selector){
            return this->day_first_updt[i];
        }
    }
    return 0;
}

int OswAppWeather::getPrevDay(){
    for(int i=this->day_first_updt.size()-1; i>=0; i--){
        if(this->day_first_updt[i] < this->updt_selector){
            return this->day_first_updt[i];
        }
    }
    return 0;
}


void OswAppWeather::setup() {
    this->loadData();
}

void OswAppWeather::loop() {

    this->drawLayout();
    this->drawWeather();   
    this->printDate();
    this->printLastUpdate();
    this->hal->requestFlush(); 

    if (hal->btnHasGoneDown(BUTTON_2)) {
            if(this->main_selector==1){// next update
                if(this->updt_selector<23) {this->updt_selector++;}
            }
            if(this->main_selector==0){//next day
                if(this->updt_selector >= this->day_first_updt[this->day_first_updt.size()-1]){
                    this->updt_selector=this->updt_selector;
                }else{
                    this->updt_selector = this->getNextDay();
                }
            }

    }
    if (hal->btnHasGoneDown(BUTTON_3)) {
       
        if(this->main_selector==1){
            if(this->updt_selector>0){this->updt_selector--; }              
        }
        if(this->main_selector==0){
            if(this->updt_selector == 0){
                this->updt_selector=this->updt_selector;
            }else{
                this->updt_selector = this->getPrevDay();
            }
        }

    }
    if( hal->btnHasGoneDown(BUTTON_1)){
        if(this->main_selector!=2){
            this->main_selector++;
        }else{
            this->main_selector=0;
        }
    }

}

void OswAppWeather::stop() {

}