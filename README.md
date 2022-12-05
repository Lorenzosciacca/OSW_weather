# OSW_weather(Discontinued)
A simple weather app for Open-SmartWatch.
## Note
  This code is now part of the open smartwatch project available [here](https://open-smartwatch.github.io/firmware/apps/OswWeather/)
## Prerequiremets
  - OSW_FEATURE_WIFI : visit [this](https://open-smartwatch.github.io/resources/firmware/) link for more details.
  - a valid API [key](https://openweathermap.org/api) ( free plan )
## Installation
1. In osw_config_keys.h add  
  `extern OswConfigKeyString weather;`  
  in the namespace OswConfigAllKeys  
  `namespace OswConfigAllKeys { ... ; extern OswConfigKeyString weather;}`
2. In osw_config_keys.cpp add the following line   
  `OswConfigKeyString weather("wtr","weather","encoded weather","","X");`  
  in the namespace OswConfigAllKeys  
  `namespace OswConfigAllKeys { ... ; OswConfigKeyString weather("wtr","weather","encoded weather","","X");}`
3. In the same file add the following declaration  
  `&OswConfigAllKeys::weather`  
  in OswConfigKey* oswConfigKeys[] = {}  
  `OswConfigKey* oswConfigKeys[] = {..., &OswConfigAllKeys::weather}`
4. Add the file weather.cpp to src/apps
5. Add the file weather.h to include/apps
6. Add the file DS_DIGI12pt7b.h to include/fonts

7. Add the app to the main

## Bugs  
 * It's possible to download the updates only if the WiFi is already enabled and connected before clicking "sync"
## TODO
* Multiple locations 

## Credits
For the Graphical user interface i took inspiration from the issue [152](https://github.com/Open-Smartwatch/open-smartwatch-os/issues/152) and in particular from Kokofruits1's proposal. 
