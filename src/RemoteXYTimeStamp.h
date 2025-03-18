#ifndef RemoteXYTimeStamp_h
#define RemoteXYTimeStamp_h 

// not use uint64_t

#define  REMOTEXY_MILLIS_PER_DAY 86400000
#define  REMOTEXY_MILLIS_PER_HOUR 3600000
#define  REMOTEXY_MILLIS_PER_MINUTE 60000
#define  REMOTEXY_MILLIS_PER_SECOND 1000
#define  REMOTEXY_SECONDS_PER_DAY 86400
#define  REMOTEXY_MINUTES_PER_DAY 1440
#define  REMOTEXY_HOURS_PER_DAY 24
#define  REMOTEXY_SECONDS_PER_MINUTE 60
#define  REMOTEXY_MINUTES_PER_HOUR 60

#define  REMOTEXY_MILLIS_PER_23DAYS (uint32_t)1987200000

//typedef int64_t RemoteXYTimeStamp;


class RemoteXYTimeStamp {
  
  //big int variation
  
  private:
  int32_t days;    //  any days
  int32_t millis;  // -86399999 ... 86399999, two numbers must be of the same sign 
  
  public:
  RemoteXYTimeStamp () {
    days = millis = 0;
  }
  
  RemoteXYTimeStamp (int32_t _days, int32_t _millis) {
    set (_days, _millis);
  } 
  
  RemoteXYTimeStamp (int32_t ms) {
    set (0, ms);
  } 
  
  RemoteXYTimeStamp (int ms) {
    set (0, ms);
  } 
  
  RemoteXYTimeStamp (uint32_t ms) {
    if (ms >= REMOTEXY_MILLIS_PER_23DAYS) set (23, ms - REMOTEXY_MILLIS_PER_23DAYS); 
    else set (0, ms);
  } 
  
  RemoteXYTimeStamp (int64_t ms) {
    setInt64 (ms);
  } 
        
  private:
  void normalize () {  
    
    //if ((millis >= REMOTEXY_MILLIS_PER_DAY) || (millis <= -REMOTEXY_MILLIS_PER_DAY)) {
    //  days += millis / REMOTEXY_MILLIS_PER_DAY;
    //  millis = millis % REMOTEXY_MILLIS_PER_DAY;      
    //}  
       
    while (millis >= REMOTEXY_MILLIS_PER_DAY) {
      millis -= REMOTEXY_MILLIS_PER_DAY;
      days++;
    }
    while (millis <= -REMOTEXY_MILLIS_PER_DAY) {
      millis += REMOTEXY_MILLIS_PER_DAY;
      days--;
    }
    if ((days > 0) && (millis < 0)) {
      days--;
      millis += REMOTEXY_MILLIS_PER_DAY;
    }
    else if ((days < 0) && (millis > 0)) {
      days++;
      millis -= REMOTEXY_MILLIS_PER_DAY;
    }    
  }

  public:
  void add (const RemoteXYTimeStamp &ts) {
    days += ts.days;
    millis += ts.millis;
    normalize ();
  }
  
  public:
  void add (int32_t ms) {
    millis += ms;
    normalize ();
  }
  
  public:
  void add (uint32_t ms) {
    if (ms > REMOTEXY_MILLIS_PER_23DAYS) {
      days += 23;
      ms -= REMOTEXY_MILLIS_PER_23DAYS;
    }
    millis += ms;
    normalize ();
  }
  
  public:
  void sub (const RemoteXYTimeStamp &ts) {
    days -= ts.days;
    millis -= ts.millis;
    normalize ();
  }
  
  public:
  void sub (int32_t ms) {
    millis -= ms;
    normalize ();
  }

  public:
  void sub (uint32_t ms) {
    if (ms > REMOTEXY_MILLIS_PER_23DAYS) {
      days -= 23;
      ms -= REMOTEXY_MILLIS_PER_23DAYS;
    }  
    millis -= ms;
    normalize ();
  }  

      
  void set (int32_t _days, int32_t _millis) {
    days = _days;
    millis = _millis;
    normalize ();
  } 
  
 

  
  // timeZone - minutes
  void applyTimeZone (int16_t timeZone) {
    add ((int32_t)timeZone * REMOTEXY_MILLIS_PER_MINUTE);
  }
  
  void setNull () {
    days = millis = 0;
  }
  
  uint8_t isNull () {
    if ((days == 0) && (millis == 0)) return 1;
    return 0;
  }
  
  
  int32_t getDays () {
    return days;
  }
  
  int32_t getMillis () {
    return millis;
  }
  
      
  int64_t getInt64 () {
    return (int64_t)days * REMOTEXY_MILLIS_PER_DAY + (int64_t)millis;
  }
  
  void setInt64 (int64_t ms) {
    days = ms / REMOTEXY_MILLIS_PER_DAY;
    millis = ms % REMOTEXY_MILLIS_PER_DAY;
  }
  
  
  // operators
  
  RemoteXYTimeStamp operator=(const RemoteXYTimeStamp &ts) {
    days = ts.days;
    millis = ts.millis;
    return *this;
  } 
  
  RemoteXYTimeStamp operator=(const int ms) {
    days = 0;
    millis = ms;
    normalize ();
    return *this;
  } 
  
  RemoteXYTimeStamp operator=(const int32_t ms) {
    days = 0;
    millis = ms;
    normalize ();
    return *this;
  } 
  
  
  RemoteXYTimeStamp operator=(const int64_t ms) {
    setInt64 (ms);
    return *this;
  } 
  
  operator int64_t() {
    return getInt64 ();
  }   
  
  public:
  RemoteXYTimeStamp operator+(const RemoteXYTimeStamp &ts2) const {
    RemoteXYTimeStamp ts (days + ts2.days, millis + ts2.millis);
    return ts;
  }
  
  public:
  RemoteXYTimeStamp operator+(const int32_t ms) const {
    RemoteXYTimeStamp ts (days, millis + ms);
    return ts;
  }  
  
  public:
  RemoteXYTimeStamp operator+(const uint32_t ms) const {
    RemoteXYTimeStamp ts (*this);
    ts.add (ms);
    return ts;
  }     
  
  public:
  RemoteXYTimeStamp operator+(const int64_t ms) const {
    RemoteXYTimeStamp ts (ms);
    return *this + ts;
  }
  
  public:
  RemoteXYTimeStamp operator-(const RemoteXYTimeStamp &ts2) const {
    RemoteXYTimeStamp ts (days - ts2.days, millis - ts2.millis);
    return ts;
  }
  
  public:
  RemoteXYTimeStamp operator-(const int32_t ms) const {
    RemoteXYTimeStamp ts (days, millis - ms);
    return ts;
  }
  
  public:
  RemoteXYTimeStamp operator-(const uint32_t ms) const {
    RemoteXYTimeStamp ts (*this);
    ts.sub (ms);
    return ts;
  }   
  
  public:
  RemoteXYTimeStamp operator-(const int64_t ms) const {
    RemoteXYTimeStamp ts (ms);
    return *this - ts;
  }
  
  public:
  RemoteXYTimeStamp operator+=(const RemoteXYTimeStamp &ts) {
    *this = *this + ts;
    return *this;
  }  
 
  public:
  RemoteXYTimeStamp operator+=(const int32_t ms) {
    *this = *this + ms;
    return *this;
  }   
  
  public:
  RemoteXYTimeStamp operator+=(const uint32_t ms) {
    *this = *this + ms;
    return *this;
  }   
  
  public:
  RemoteXYTimeStamp operator+=(const int64_t ms) {
    *this = *this + ms;
    return *this;
  }  
  
  public:
  RemoteXYTimeStamp operator-=(const RemoteXYTimeStamp &ts) {
    *this = *this - ts;
    return *this;
  }   

  public:
  RemoteXYTimeStamp operator-=(const int32_t ms) {
    *this = *this - ms;
    return *this;
  } 
  
  public:
  RemoteXYTimeStamp operator-=(const uint32_t ms) {
    *this = *this - ms;
    return *this;
  }  
  
  public:
  RemoteXYTimeStamp operator-=(const int64_t ms) {
    *this = *this - ms;
    return *this;
  }  
  
  bool operator<(const RemoteXYTimeStamp &ts) {
    if (days == ts.days) return millis < ts.millis;
    return days < ts.days;
  }
  
  bool operator<(const int32_t ms) {
    return *this < RemoteXYTimeStamp (0, ms);
  }
  
  bool operator<(const int64_t ms) {
    return *this < RemoteXYTimeStamp (ms);
  }

  bool operator<=(const RemoteXYTimeStamp &ts) {
    if (days == ts.days) return millis <= ts.millis;
    return days <= ts.days;
  }
  
  bool operator<=(const int32_t ms) {
    return *this <= RemoteXYTimeStamp (0, ms);
  }
  
  bool operator<=(const int64_t ms) {
    return *this <= RemoteXYTimeStamp (ms);
  }
          
  bool operator>(const RemoteXYTimeStamp &ts) {
    if (days == ts.days) return millis > ts.millis;
    return days > ts.days;
  }
  
  bool operator>(const int32_t ms) {
    return *this > RemoteXYTimeStamp (0, ms);
  }
  
  bool operator>(const int64_t ms) {
    return *this > RemoteXYTimeStamp (ms);
  }  

  bool operator>=(const RemoteXYTimeStamp &ts) {
    if (days == ts.days) return millis >= ts.millis;
    return days >= ts.days;
  }

  bool operator>=(const int32_t ms) {
    return *this >= RemoteXYTimeStamp (0, ms);
  }
  
  bool operator>=(const int64_t ms) {
    return *this >= RemoteXYTimeStamp (ms);
  }
      
  bool operator==(const RemoteXYTimeStamp &ts) {
    return ((days == ts.days) && (millis == ts.millis)); 
  }   
  
  bool operator==(const int ms) {
    return *this == RemoteXYTimeStamp (0, ms); 
  } 
  
  bool operator==(const int32_t ms) {
    return *this == RemoteXYTimeStamp (0, ms); 
  }   
  
  bool operator==(const int64_t ms) {
    return *this == RemoteXYTimeStamp (ms); 
  }   
  
  bool operator!=(const RemoteXYTimeStamp &ts) {
    return ((days != ts.days) || (millis != ts.millis)); 
  }  
  
  bool operator!=(const int ms) {
    return *this != RemoteXYTimeStamp (0, ms); 
  } 
  
  bool operator!=(const int32_t ms) {
    return *this != RemoteXYTimeStamp (0, ms); 
  }  
  
  bool operator!=(const int64_t ms) {
    return *this != RemoteXYTimeStamp (ms); 
  }  

};             



#endif //RemoteXYTimeStamp_h       