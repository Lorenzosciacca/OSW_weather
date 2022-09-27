# OSW_weather
A simple weather app for Open-SmartWatch.

## Prerequiremets
  - OSW_FEATURE_WIFI : visit [this](https://open-smartwatch.github.io/resources/firmware/) link for more details.
  - a valid API [key](https://openweathermap.org/api) ( free plan )
## Installation
1. In osw_config_keys.cpp add the following line   
  `OswConfigKeyString weather("wtr","weather","encoded weather","","X");`  
  in the namespace OswConfigAllKeys  
  `namespace OswConfigAllKeys { ... ; OswConfigKeyString weather("wtr","weather","encoded weather","","X");}`
2. In the same file add the following declaration  
  `&OswConfigAllKeys::weather`  
  in OswConfigKey* oswConfigKeys[] = {}  
  `OswConfigKey* oswConfigKeys[] = {..., &OswConfigAllKeys::weather}`
3. Add the file weather.cpp to src/apps
4. Add the file weather.h to include/apps
5. Add the file DS_DIGI12pt7b.h to include/fonts
6. Insert the location and the API key in weather.cpp  
```
#define OPENWEATHERMAP_APIKEY "INSERT_HERE_API_KEY"
#define OPENWEATHERMAP_CITY "turin"    
#define OPENWEATHERMAP_STATE_CODE "IT"    
```
7. Add the app to the main

  
