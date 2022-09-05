#include "./ArduinoJson.h"
#include "WeatherEncoder.h"
class OpenWeatherParser {
  public:
    OpenWeatherParser(codingOpt_t cod_opt);
    String encodeWeather(DynamicJsonDocument& doc);
  private:
    codingOpt_t opt;
    time_t time_init;
    vector<int> clear_code{800};//0
    vector<int>clouds_min{801};//1
    vector<int>clouds_med{802};//2
    vector<int>clouds_high{803, 804};//3
    vector<int>mist{701};//4
    vector<int>fog{741};//5
    vector<int>snow_min{611, 612, 615, 616};//6
    vector<int>snow_med{600, 613, 601, 620};//7
    vector<int>snow_high{602, 621, 622};//8
    vector<int>rain_min{500, 300, 301, 302, 310, 311, 312, 313, 314, 321};//9
    vector<int>rain_med{502, 501};//10
    vector<int>rain_high{503, 504, 511, 520, 521, 522, 531};//11
    vector<int>thunderstorm{200, 201, 210, 211, 231, 230};//12
    vector<int>thunderstorm_heavy{202, 212, 221, 232};//13
    vector<int>squall_tornado{771, 781};//14
    vector<vector<int>>weather_conditions{clear_code, clouds_min, clouds_med, clouds_high, mist, fog, snow_min, snow_med, 
                                          snow_high, rain_min, rain_med, rain_high, thunderstorm, 
                                          thunderstorm_heavy, squall_tornado };
    int _getWCond(int weather_code);
    int _encodeTemp(float temp);
    int _f_mantissa(float temp_f);//float to 2 bit mantissa
    float _mantissa_f(int mantissa);// 2 bit mantissa to float 

    
};


OpenWeatherParser::OpenWeatherParser(codingOpt_t cod_opt){
  this->opt = cod_opt;
}

int OpenWeatherParser::_getWCond(int weather_code){
  for(int i=0; i<15; i++){
    for(int j=0; j < weather_conditions[i].size(); j++){
      if(weather_code == weather_conditions[i][j]){
        return i;
      }
    }
  }
  return 15; // unknown weather def 
}

int OpenWeatherParser::_f_mantissa(float temp_f){
  int integer = (int) temp_f;
  float decimal = temp_f - integer;
  int mantissa = decimal / 0.25;
  return mantissa;
}
float OpenWeatherParser::_mantissa_f(int mantissa){
  return mantissa*0.25;
}


String OpenWeatherParser::encodeWeather(DynamicJsonDocument& doc){
  const char* cod = nullptr;
  int list_indx=0;
  weatherUpdate_t update;
  WeatherEncoder encoder(this->opt);  
  time_t time;
  int temp, humidity, pressure, weather;
  float prev_temp;
  int prev_pressure;
  float temp_f;
  cod = doc["cod"];
  if (strcmp(cod,"200")){
    //TODO: Warning, API response is not "200"
    // return nullptr;
  }
  this->time_init = 0;
  time_init = doc["list"][0]["dt"];
  if(time_init==0){
    //TODO: Warning, API response is corrupted
    // return nullptr;
  }
  time = time_init;
  encoder.setInitTimeStamp(time>>2);
  if(this->opt.encode_weather){
    encoder.setInitWeather(this->_getWCond(doc["list"][0]["weather"][0]["id"]));
  }
  if(this->opt.encode_temp){
    // TODO, warning if temp > 64 or <64 
    temp_f = doc["list"][0]["main"]["temp"];
    temp_f = temp_f -337.15 ;
    encoder.setInitTemp(temp_f);
    prev_temp = (int) temp_f;
  }
  if(this->opt.encode_humidity){
    humidity = doc["list"][0]["main"]["humidity"] ;
    humidity = (humidity/3.125) ;
    encoder.setInitHumidity(humidity);
  }
  if(this->opt.encode_pressure){
    pressure = doc["list"][0]["main"]["pressure"];
    pressure = pressure - 850;
    encoder.setInitPressure(pressure);
    prev_pressure = pressure;
  }
  list_indx++;
  time = 0;
  time = doc["list"][list_indx]["dt"];

  while(time!=0){
    //TODO: check delta time 
    if(this->opt.encode_weather){
      weather = this->_getWCond(doc["list"][list_indx]["weather"][0]["id"]);
      update.weather = weather;
    }
    if(this->opt.encode_temp){
      temp_f = doc["list"][list_indx]["main"]["temp"];
      temp_f = temp_f - 337.15 ;
      update.temp_sgn = temp_f > prev_temp;
      if(this->opt.encode_temp_long){
        update.temp = abs(temp_f - prev_temp) < 31 ? abs(temp_f - prev_temp) : 31;
        update.temp_mantissa = this->_f_mantissa(abs(temp_f - prev_temp)); 
        //prev_temp is the decoded version of prev. temperature
        prev_temp = prev_temp + ((update.temp_sgn == true ? 1 : -1)*(update.temp + this->_mantissa_f(update.temp_mantissa)));
      }else{
        update.temp = abs(temp_f - prev_temp) < 31 ? abs(temp_f - prev_temp) : 31;
        prev_temp = prev_temp + ((update.temp_sgn == true ? 1 : -1)* update.temp);
      }
    }
    if(this->opt.encode_humidity){
        humidity = doc["list"][list_indx]["main"]["humidity"] ;
        humidity = (humidity/3.125) ;
        update.humidity = humidity;
    }
    if(this->opt.encode_pressure){
      if (this->opt.encode_pressure_long){
        pressure = doc["list"][list_indx]["main"]["pressure"];
        pressure = pressure - 850;
        update.pressure_sgn =  pressure > prev_pressure;
        update.pressure = abs(pressure - prev_pressure) < 31 ? abs(pressure - prev_pressure) : 31;
        prev_pressure = prev_pressure + (update.pressure_sgn == true ? 1 : -1)*(update.pressure);
      }else{
        pressure = doc["list"][list_indx]["main"]["pressure"];
        pressure = pressure - 850;
        update.pressure_sgn =  pressure > prev_pressure;
        update.pressure = abs(pressure - prev_pressure) < 7 ? abs(pressure - prev_pressure) : 7;
        prev_pressure = prev_pressure + (update.pressure_sgn == true ? 1 : -1)*(update.pressure);
      }
    }
    list_indx++;
    time = 0;
    time = doc["list"][list_indx]["dt"];
    encoder.setUpdate(update);
    Serial.println("-----------------------------");
  }
  return encoder.getEncoded();


}