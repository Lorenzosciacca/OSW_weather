#include <iostream>
#include <string> 
#include <vector>
using namespace std;
#include "../src/ArduinoJson.h" 
#include "../src/OpenWeatherParser.h" 
int main()
{
    //response 
    string json = "{\"cod\":\"200\",\"message\":0,\"cnt\":40,\"list\":[{\"dt\":1659020400,\"main\":{\"temp\":305.83,\"feels_like\":306.44,\"temp_min\":305.61,\"temp_max\":305.83,\"pressure\":1010,\"sea_level\":1010,\"grnd_level\":1000,\"humidity\":40,\"temp_kf\":0.22},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03d\"}],\"clouds\":{\"all\":31},\"wind\":{\"speed\":1.43,\"deg\":142,\"gust\":3.33},\"visibility\":10000,\"pop\":0.12,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2022-07-28 15:00:00\"},{\"dt\":1659031200,\"main\":{\"temp\":304.39,\"feels_like\":305.21,\"temp_min\":301.52,\"temp_max\":304.39,\"pressure\":1010,\"sea_level\":1010,\"grnd_level\":1000,\"humidity\":45,\"temp_kf\":2.87},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":42},\"wind\":{\"speed\":1.3,\"deg\":259,\"gust\":3.78},\"visibility\":10000,\"pop\":0.47,\"rain\":{\"3h\":0.21},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2022-07-28 18:00:00\"},{\"dt\":1659042000,\"main\":{\"temp\":297.46,\"feels_like\":297.93,\"temp_min\":293.28,\"temp_max\":297.46,\"pressure\":1013,\"sea_level\":1013,\"grnd_level\":1004,\"humidity\":76,\"temp_kf\":4.18},\"weather\":[{\"id\":501,\"main\":\"Rain\",\"description\":\"moderate rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":48},\"wind\":{\"speed\":3.02,\"deg\":31,\"gust\":6.96},\"visibility\":10000,\"pop\":0.78,\"rain\":{\"3h\":4.54},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2022-07-28 21:00:00\"},{\"dt\":1659052800,\"main\":{\"temp\":291.87,\"feels_like\":292.33,\"temp_min\":291.87,\"temp_max\":291.87,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1005,\"humidity\":97,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10n\"}],\"clouds\":{\"all\":42},\"wind\":{\"speed\":2.13,\"deg\":312,\"gust\":3.07},\"visibility\":10000,\"pop\":0.81,\"rain\":{\"3h\":2.03},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2022-07-29 00:00:00\"},{\"dt\":1659063600,\"main\":{\"temp\":291.79,\"feels_like\":292.22,\"temp_min\":291.79,\"temp_max\":291.79,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1004,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04n\"}],\"clouds\":{\"all\":66},\"wind\":{\"speed\":1.02,\"deg\":280,\"gust\":1.22},\"visibility\":10000,\"pop\":0.27,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2022-07-29 03:00:00\"},{\"dt\":1659074400,\"main\":{\"temp\":293.32,\"feels_like\":293.74,\"temp_min\":293.32,\"temp_max\":293.32,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1005,\"humidity\":90,\"temp_kf\":0},\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":{\"all\":60},\"wind\":{\"speed\":0.53,\"deg\":302,\"gust\":0.73},\"visibility\":10000,\"pop\":0.28,\"rain\":{\"3h\":0.37},\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2022-07-29 06:00:00\"},{\"dt\":1659085200,\"main\":{\"temp\":298.05,\"feels_like\":298.37,\"temp_min\":298.05,\"temp_max\":298.05,\"pressure\":1015,\"sea_level\":1015,\"grnd_level\":1005,\"humidity\":68,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":56},\"wind\":{\"speed\":0.21,\"deg\":122,\"gust\":0.54},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2022-07-29 09:00:00\"},{\"dt\":1659096000,\"main\":{\"temp\":303.09,\"feels_like\":303.96,\"temp_min\":303.09,\"temp_max\":303.09,\"pressure\":1012,\"sea_level\":1012,\"grnd_level\":1002,\"humidity\":49,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04d\"}],\"clouds\":{\"all\":55},\"wind\":{\"speed\":0.26,\"deg\":113,\"gust\":2.04},\"visibility\":10000,\"pop\":0.02,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2022-07-29 12:00:00\"}]}";

    DynamicJsonDocument doc(40024);
    deserializeJson(doc,json);
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
    std::cout << "..." << endl;

    WeatherDecoder decoder(encoded);
    headerData_t h;
    weatherUpdate_t up;
    h = decoder.getHeader();
    up = decoder.getNext();

    while(up.last_update != 1){
    up = decoder.getNext();
    std::cout << up.weather << endl;
    }
    std::cout << ".";
}