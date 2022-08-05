#include <iostream>
#include <string>

#include <vector>
using namespace std;

/*TODO: sync()
        header mantissa
        overloading setUpdate( weatherUpdate_t* update, int n_updates)
        safety
*/ 


// Weather encoder 
#include <bitset>
typedef struct  {
    bool encode_weather = true;
    bool encode_temp;
    bool encode_humidity;
    bool encode_pressure;
    bool encode_temp_long;
    bool encode_pressure_long;
    int delta = 3;
} codingOpt_t;

typedef struct {
  int weather;
  int temp; bool temp_sgn;
  int temp_mantissa;
  int humidity; 
  int pressure; bool pressure_sgn;
  bool last_update;// used only by the decoder
}weatherUpdate_t;

typedef struct
  {
    time_t timestamp;
    int delta_time;
    int to_next_sync;
    bool encode_weather, encode_temp, encode_humidity, encode_pressure, encode_temp_long, encode_pressure_long;
    int weather_init;
    int temp_init;
    int temp_init_mantissa;
    int humidity_init;
    int pressure_init;
  } headerData_t;



class WeatherEncoder{

  public:
    WeatherEncoder(codingOpt_t opt);
    void setInitTimeStamp(time_t time);
    void setInitWeather(int init_w);
    void setInitTemp(int temp, int temp_mantissa = 0);
    void setInitHumidity(int humidity);
    void setInitPressure(int pressure);
    void setUpdate( weatherUpdate_t update);
    void sync();// to be implemted
    string getEncoded();

  private:
  headerData_t last_header;
  int last_header_indx; //  index in outData where to put header
  vector<uint8_t> outData{};
  vector<uint8_t>::iterator oPtr = outData.begin();
  char buffer8bit[8];
  int buffer_indx = 0;
  int char_cnt = 0;// # of chars in outData 
  int update_cnt = 0;
  void _putInBuffer(string in_data);
  unsigned int _byteToUint(char *bits);
  unsigned int _byteToUint(string bits);
  void _writeHeader(string header_bin);
  void _emptyBuffer();

};

WeatherEncoder::WeatherEncoder(codingOpt_t opt){
  //set last_header
  this->last_header.encode_weather = opt.encode_weather;
  this->last_header.encode_temp = opt.encode_temp;
  this->last_header.encode_humidity = opt.encode_humidity;
  this->last_header.encode_pressure = opt.encode_pressure;
  if (this->last_header.encode_temp){
    this->last_header.encode_temp_long = opt.encode_temp_long;
  }
  if (this->last_header.encode_pressure){
    this->last_header.encode_pressure_long = opt.encode_pressure_long;
  }
  this->last_header.delta_time = opt.delta;
  this->last_header_indx = 0;
}

void WeatherEncoder::setInitTimeStamp( time_t time ){
  this->last_header.timestamp = time ;
}

void WeatherEncoder::setInitWeather(int init_w ){
  if ( !this->last_header.encode_weather){
    //TODO, warning
  }
  if ( init_w > 15 ){
    //TODO, warning
  }
  this->last_header.weather_init = init_w;
}

void WeatherEncoder::setInitTemp(int temp, int temp_mantissa){
  if ( ! this->last_header.encode_temp){
    //TODO, warning
  }
  if ( temp > 127){
    //TODO, warning
  }
  this->last_header.temp_init = temp;
  if ( this->last_header.encode_temp_long ){
    this->last_header.temp_init_mantissa = temp_mantissa;
  }
}
 
void WeatherEncoder::setInitHumidity(int humidity){
  if (! this->last_header.encode_humidity){
    //TODO, warning
  }
  if ( humidity > 31 ){
    //TODO, warning
  }
  this->last_header.humidity_init = humidity;
}

void WeatherEncoder::setInitPressure(int pressure){
  if ( !this->last_header.encode_pressure ){
    //TODO, warning
  }
  this->last_header.pressure_init = pressure;
}

unsigned int WeatherEncoder::_byteToUint(char* bits){
  unsigned int res = 0;
  int j = 0;
  bitset<8> b = bitset<8>(string(bits));
  res = b.to_ulong();
  return res;
}
unsigned int WeatherEncoder::_byteToUint(string bits){
  unsigned int res = 0;
  int j = 0;
  bitset<8> b = bitset<8>(string(bits));
  res = b.to_ulong();
  return res;
}


void WeatherEncoder::_putInBuffer(string in_data){
  unsigned int c;
  for( int i = 0; i < in_data.length(); i++){
    this->buffer8bit[buffer_indx] = in_data[i];
    /* empty the buffer */ 
    if ( buffer_indx == 7){
      c = this->_byteToUint(buffer8bit);
      char_cnt++;
      this->outData.push_back(c);
      buffer_indx = 0;      
    }else{ 
      buffer_indx++;
    }
  }
  
}

void WeatherEncoder::sync(){
/* to be implemented */
  
}



template <typename... Args>
unsigned long long getULL(Args... as)
{
  using unused = int[];
  unsigned long long ret{0ULL};
  (void)unused{0, (ret <<= 1, ret |= (as ? 1ULL : 0ULL), 0)...};
  return ret;
}

template<typename T>
T adder(T v) {
  return v;
}

template<typename T, typename... Args>
T adder(T first, Args... args) {
  return first + adder(args...);
}

void WeatherEncoder::_writeHeader(string header_bin){
  string str;
  unsigned int c = 0;
  string cs;
  vector<uint8_t>::iterator it; 
  size_t n = 1;
  it = outData.begin();
  for(int i = 8; i>= 0; i--){
    str = header_bin.substr(i*8,8);
    c = this->_byteToUint(str);
    outData.insert(it,c);
    it = outData.begin();
  }
}

void WeatherEncoder::_emptyBuffer(){
  unsigned int c =0;
  if(this->buffer_indx != 0){
    for (int i= buffer_indx; i <= 7; i++){
      buffer8bit[i] = '0';
    }
    c = this->_byteToUint(buffer8bit);
    this->outData.push_back(c);
    char_cnt++;
    buffer_indx = 0;
  } 
}

string WeatherEncoder::getEncoded()
{
  // write header
  string header_bin;
  bitset<30> bTime = this->last_header.timestamp;
  bitset<5> bDelta = this->last_header.delta_time;
  bitset<7> bToNxtSync = 0; // NOTE: sync() not implemented
  bitset<6> bC{getULL(this->last_header.encode_weather, this->last_header.encode_temp, this->last_header.encode_humidity,
                      this->last_header.encode_pressure, this->last_header.encode_temp_long,
                      this->last_header.encode_pressure_long)}; // code opt 6 bit
  bitset<4> bW = this->last_header.weather_init;
  bitset<7> bT = this->last_header.temp_init;
  bitset<5> bH = this->last_header.humidity_init;
  bitset<8> bPx = this->last_header.pressure_init;
  header_bin = adder(bTime.to_string(), bDelta.to_string(), bToNxtSync.to_string(), bC.to_string(), bW.to_string(),bT.to_string(), bH.to_string(), bPx.to_string());
  this->_writeHeader(header_bin);
  //empty the buffser 
  this->_emptyBuffer();
  string out_s;
  uint8_t ranz;
  for (int i=0; i<outData.size();i++){
    out_s.push_back(outData[i]);
  }
  return out_s;
}

void WeatherEncoder::setUpdate(weatherUpdate_t update){
  char k;
  string j;
  if (this->last_header.encode_weather){
    bitset<4> bW = update.weather;
    this->_putInBuffer(bW.to_string());
  }

  if (this->last_header.encode_temp){
    /* temp_sgn */
      k = 48 + update.temp_sgn;
      j.push_back(k);
      this->_putInBuffer( j );
      j.clear();  
    if (this->last_header.encode_temp_long){
      bitset<5> bT = update.temp; //  temp long
      this->_putInBuffer(bT.to_string());
      bitset<2> bTman = update.temp_mantissa; // temp_mantissa 
      this->_putInBuffer(bTman.to_string());
    }else{
      bitset<3> bT = update.temp; //  temp short
      this->_putInBuffer(bT.to_string());
    }
  }
  if (this->last_header.encode_humidity){
      bitset<5> bH = update.humidity; // humidity
      this->_putInBuffer(bH.to_string());
  }
  if (this->last_header.encode_pressure){
      /* p_sgn */
      k = 48 + update.pressure_sgn;
      j.push_back(k);
      this->_putInBuffer( j );
      j.clear();  
      if (this->last_header.encode_pressure_long){
        bitset<5> bP = update.pressure; // pressure
        this->_putInBuffer(bP.to_string());
      }else{
        bitset<3> bP = update.pressure; // pressure
        this->_putInBuffer(bP.to_string());
      }
  }
  update_cnt++;
}

/* Weather decoder */
/* NOTE: temp_init_mantissa not yet implemented */ 
class WeatherDecoder
{
public:
  WeatherDecoder(string data);
  // headerData_t last_header;
  headerData_t getHeader();// get header struct 
  weatherUpdate_t getNext();// get next update 
  weatherUpdate_t getPrev();// TODO, to be implemented 
private:
  headerData_t _header;
  string _header_str;
  string in_data;
  void _buildHeader();
  void _decodeHeader();
  string _readBuffer(int n_bits);
  bool data_is_valid; // just check size 
  bool header_is_valid;
  uint8_t update_size; // size of a single update [bit]
  void _getUpdateSize();
  string _buffer;
  bool buffer_is_empty = true;
  int in_data_indx;
  int buffer_indx;
  int bit_cnt = 0;
  int remaining_updt;
  weatherUpdate_t _readUpdate();
};

// size of a single update [bit]
void WeatherDecoder::_getUpdateSize(){
  update_size = 0;//[bit]
  update_size = (4*_header.encode_weather) + (_header.encode_temp *(4 + (4*_header.encode_temp_long))) 
                + (_header.encode_humidity * 5)
                 + (_header.encode_pressure * (4 +(2*_header.encode_pressure_long)));
}


WeatherDecoder::WeatherDecoder(string data){
  uint8_t header_uint8[9];
  if (data.size()*8<76){
    this->header_is_valid = false;
  }
  else{
    header_is_valid = true;
    for(int i=0; i<9;i++){
      header_uint8[i] = uint8_t(data[i]);
    }
    for(int i=0;i<9;i++){
      _header_str.append(bitset<8>(header_uint8[i]).to_string());
    }
    this->_decodeHeader();
    this->_getUpdateSize();
    if(data.size() < 9 + ((update_size)) / 8){
      this->data_is_valid = false;
    }
    this->in_data_indx = 9;
    in_data = data;
    remaining_updt= ((in_data.size()-9)*8)/update_size;
  }
}
void WeatherDecoder::_decodeHeader(){
  bitset<30> bTime = bitset<30>(string(_header_str.substr(0,30)));
  this->_header.timestamp = bTime.to_ulong();
  bitset<5> bDelta = bitset<5>(string(_header_str.substr(30,5)));
  this->_header.delta_time = bDelta.to_ulong();
  bitset<7> bSync = bitset<7>(string(_header_str.substr(35,7)));
  this->_header.to_next_sync = bSync.to_ulong();

  this->_header.encode_weather = (_header_str[42]=='1' ? true : false);
  this->_header.encode_temp = (_header_str[43]=='1' ? true : false);
  this->_header.encode_humidity = (_header_str[44]=='1' ? true : false);
  this->_header.encode_pressure = (_header_str[45]=='1' ? true : false);
  this->_header.encode_temp_long = (_header_str[46]=='1' ? true : false);
  this->_header.encode_pressure_long = (_header_str[47]=='1' ? true : false);

  bitset<4> bW = bitset<4>(string(_header_str.substr(48,4)));
  this->_header.weather_init = bW.to_ulong();
  bitset<7> bT = bitset<7>(string(_header_str.substr(52,7)));
  this->_header.temp_init = bT.to_ulong();
  bitset<5> bH = bitset<5>(string(_header_str.substr(59,5)));
  this->_header.humidity_init = bH.to_ulong();
  bitset<8> bP = bitset<8>(string(_header_str.substr(64,8)));
  this->_header.pressure_init = bP.to_ulong();
  bit_cnt = 72;
}


headerData_t WeatherDecoder::getHeader(){
  return _header;
}

string WeatherDecoder::_readBuffer(int n_bits){
  string out_str = "";
  for ( int i=0; i<n_bits; i++){
    if(this->buffer_is_empty){
      if(in_data_indx==in_data.size()){
        return "";
      }
      this->_buffer = bitset<8>(in_data[this->in_data_indx]).to_string();
      this->in_data_indx++;
      this->buffer_is_empty = false;
      this->buffer_indx = 0;
    }
    out_str.push_back(_buffer[buffer_indx]);
    bit_cnt++;
    this->buffer_indx++;
    if(buffer_indx == 8){
      this->buffer_is_empty = true;
    }
  }
  return out_str;

}

weatherUpdate_t WeatherDecoder::_readUpdate(){
  weatherUpdate_t updt;
  if(this->_header.encode_weather){
    bitset<4> bW = bitset<4>(this->_readBuffer(4));
    updt.weather = bW.to_ulong();
  }
  if(this->_header.encode_temp){
    updt.temp_sgn = this->_readBuffer(1)=="1" ? true : false;
    if(this->_header.encode_temp_long){
      bitset<5> bT = bitset<5>(this->_readBuffer(5));
      updt.temp = bT.to_ulong();
      bitset<2>  bMantissa = bitset<2>(this->_readBuffer(2));
      updt.temp_mantissa = bMantissa.to_ullong();
    }else{
      bitset<3> bT = bitset<3>(this->_readBuffer(3));
      updt.temp = bT.to_ulong();
    }
  }
  if(this->_header.encode_humidity){
      bitset<5> bH = bitset<5>(this->_readBuffer(5));
      updt.humidity = bH.to_ulong();
  }
  if(this->_header.encode_pressure){
    updt.pressure_sgn = this->_readBuffer(1)=="1" ? true : false;
    if(this->_header.encode_pressure_long){
      bitset<5> bP = bitset<5>(this->_readBuffer(5));
      updt.pressure = bP.to_ulong();
    }else{
      bitset<3> bP = bitset<3>(this->_readBuffer(3));
      updt.temp = bP.to_ulong();
    }
  }
  return updt;
}

/* get next update */ 
weatherUpdate_t WeatherDecoder::getNext(){
  //
  if(data_is_valid && header_is_valid){
    weatherUpdate_t update;
    update = this->_readUpdate();
    this->remaining_updt--;
    if(remaining_updt == 0){
      update.last_update = true;
    }else{
      update.last_update = false;
    }
    return update;
  }else {
    weatherUpdate_t broken;
    broken.last_update = true;
    return broken;
  }
}
