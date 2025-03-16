#ifndef RemoteXYTime_h
#define RemoteXYTime_h 

// not use uint64_t

#define  REMOTEXY_MILLIS_PER_DAY 86400000
#define  REMOTEXY_MILLIS_PER_HOUR 3600000
#define  REMOTEXY_MILLIS_PER_MINUTE 60000

#define  REMOTEXY_MILLIS_END_DAY 86399999

const char * REMOTEXY_DATEFORMAT_YYYY = "YYYY"; // year, 2024 , can use yyyy
const char * REMOTEXY_DATEFORMAT_YY =   "yy";   // year, 24 , can use yy
const char * REMOTEXY_DATEFORMAT_MMMM = "MMMM"; // month, January - December
const char * REMOTEXY_DATEFORMAT_MMM =  "MMM";  // month, Jan - Dec
const char * REMOTEXY_DATEFORMAT_MM =   "MM";   // month, 01 - 12
const char * REMOTEXY_DATEFORMAT_dd =   "dd";   // day, 01 - 31
const char * REMOTEXY_DATEFORMAT_d =    "d";    // day, 1 - 31
const char * REMOTEXY_DATEFORMAT_EEEE = "EEEE"; // day of week, Sunday - Saturday
const char * REMOTEXY_DATEFORMAT_EEE =  "EEE";  // day of week, Sun - Sat
const char * REMOTEXY_DATEFORMAT_u =    "u";    // day of week, 1 - 7, 1 is Monday
const char * REMOTEXY_DATEFORMAT_HH =   "HH";   // hour, 00 - 23
const char * REMOTEXY_DATEFORMAT_h =    "h";    // hour, 1 - 12
const char * REMOTEXY_DATEFORMAT_mm =   "mm";   // minute, 00 - 59
const char * REMOTEXY_DATEFORMAT_ss =   "ss";   // second, 00 - 59
const char * REMOTEXY_DATEFORMAT_SSS =  "SSS";  // millisecond, 000 - 999
const char * REMOTEXY_DATEFORMAT_aa =   "aa";   // AM or PM

const char * REMOTEXY_DATE_AM =  "AM";  
const char * REMOTEXY_DATE_PM =  "PM";  


const char REMOTEXY_MONTHS_FULLNAME[][10] = {                                                                   
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};
const char REMOTEXY_MONTHS_SHORTNAME[][4] = {                                                                   
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
const char REMOTEXY_DAYSOFWEEK_FULLNAME[][10] = {                                                                   
  "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};
const char REMOTEXY_DAYSOFWEEK_SHORTNAME[][4] = {                                                                   
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

class RemoteXYDate {
  public:
  uint16_t millis;
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t dayOfWeek;
  
  char * format (char * str, const char * tmpl) {

    while (*tmpl) {
      uint8_t d = 0;
      if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_YYYY, 1)) {
        str = rxy_intToFixedStr (year, str, 4, '0');
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_YY, 1)) {
        str = rxy_intToFixedStr (year, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_MMMM, 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, REMOTEXY_MONTHS_FULLNAME[month-1]);
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_MMM, 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, REMOTEXY_MONTHS_SHORTNAME[month-1]);
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_MM, 0)) {
        str = rxy_intToFixedStr (month, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_dd, 0)) {
        str = rxy_intToFixedStr (day, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_d, 0)) {
        str = rxy_intToStr (day, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_u, 0)) {
        str = rxy_intToStr (dayOfWeek, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_EEEE, 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, REMOTEXY_DAYSOFWEEK_FULLNAME[dayOfWeek-1]);
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_EEE, 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, REMOTEXY_DAYSOFWEEK_SHORTNAME[dayOfWeek-1]);
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_HH, 0)) {
        str = rxy_intToFixedStr (hour, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_h, 0)) {
        uint8_t h = hour % 12;
        if (h == 0) h = 12;
        str = rxy_intToStr (h, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_mm, 0)) {
        str = rxy_intToFixedStr (minute, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_ss, 0)) {
        str = rxy_intToFixedStr (second, str, 2, '0');
        d=2;
      }     
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_SSS, 0)) {
        str = rxy_intToFixedStr (millis, str, 3, '0');
        d=3;
      }     
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_DATEFORMAT_aa, 0)) {
        if (hour < 12) str = rxy_strCopy (str, REMOTEXY_DATE_AM); 
        else str = rxy_strCopy (str, REMOTEXY_DATE_PM);
        d=2;
      }
      else if (*tmpl == 0x27) {  // character escaping
        tmpl++; 
        uint8_t f = 0;
        char ch;
        while (true) {
          ch = *tmpl;
          if (ch == 0) break;
          if ((ch == 0x27) && (f == 0)) {
            tmpl++;
            break;
          }
          else if (ch == 0x5C) {
            f = 1;
            tmpl++;
            continue;
          }
          *str++ = ch; 
          tmpl++;
          f = 0;
        }
      }
      else {
        *str++ = *tmpl;
        d=1;
      }
      tmpl+=d;
    }
    *str = 0;
    return str;
  }
};


class RemoteXYTime {
  
  //two numbers must be of the same sign
  private:
  int32_t days; 
  int32_t millis;    
  
  public:
  RemoteXYTime () {
    days = millis = 0;
  }
  
  RemoteXYTime (int32_t _days, int32_t _millis) {
    days = _days;
    millis = _millis;
    normalize ();
  } 
  
    
  private:
  void normalize () { 
    while (millis >= REMOTEXY_MILLIS_PER_DAY) {
      days++;
      millis -= REMOTEXY_MILLIS_PER_DAY; 
    }
    while (millis <= -REMOTEXY_MILLIS_PER_DAY) {
      days--;
      millis += REMOTEXY_MILLIS_PER_DAY; 
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
  void add (const RemoteXYTime& time2) {
    days += time2.days;
    millis += time2.millis;
    normalize ();
  }
  
  public:
  void add (const int32_t ms) {
    millis += ms;
    normalize ();
  }
  
  public:
  void sub (const RemoteXYTime& time2) {
    days -= time2.days;
    millis -= time2.millis;
    normalize ();
  }
  
  public:
  void sub (const int32_t ms) {
    millis -= ms;
    normalize ();
  }

  
  public:
  RemoteXYTime operator+(const RemoteXYTime& time2) const {
    RemoteXYTime time (days + time2.days, millis + time2.millis);
    return time;
  }
  
  public:
  RemoteXYTime operator+(const int32_t ms) const {
    RemoteXYTime time (days, millis + ms);
    return time;
  }
  
  public:
  RemoteXYTime operator-(const RemoteXYTime& time2) const {
    RemoteXYTime time (days - time2.days, millis - time2.millis);
    return time;
  }
  
  public:
  RemoteXYTime operator-(const int32_t ms) const {
    RemoteXYTime time (days, millis - ms);
    return time;
  }
  
  public:
  RemoteXYTime operator+=(const RemoteXYTime& time2) {
    *this = *this + time2;
    return *this;
  }  
 
  public:
  RemoteXYTime operator+=(const int32_t ms) {
    *this = *this + ms;
    return *this;
  }  
  
  public:
  RemoteXYTime operator-=(const RemoteXYTime& time2) {
    *this = *this - time2;
    return *this;
  }   

  public:
  RemoteXYTime operator-=(const int32_t ms) {
    *this = *this - ms;
    return *this;
  } 
  
  RemoteXYTime operator=(const RemoteXYTime& time) {
    days = time.days;
    millis = time.millis;
    return *this;
  } 
  
  RemoteXYTime operator=(const int32_t ms) {
    millis = ms;
    normalize ();
    return *this;
  } 
  
  bool operator<(const RemoteXYTime& time2) {
    if (days == time2.days) return millis < time2.millis;
    return days < time2.days;
  }
        
  bool operator>(const RemoteXYTime& time2) {
    if (days == time2.days) return millis > time2.millis;
    return days > time2.days;
  }
  
  bool operator==(const RemoteXYTime& time2) {
    return ((days == time2.days) && (millis == time2.millis)); 
  }   
  
  bool operator==(const int32_t ms) {
    RemoteXYTime time (0, ms);  
    return time == *this; 
  }   
  
  bool operator!=(const RemoteXYTime& time2) {
    return ((days != time2.days) || (millis != time2.millis)); 
  }  
  
  bool operator!=(const int32_t ms) {
    RemoteXYTime time (0, ms); 
    return time != *this; 
  }  
  
  
  void set (int32_t _days, int32_t _millis) {
    days = _days;
    millis = _millis;
    normalize ();
  } 
  
  void set (int32_t _millis) {
    days = 0;
    millis = _millis;
    normalize ();
  }
  
  void setDays (int32_t _days) {
    days = _days;
    normalize ();
  }
  
  void setMillis (int32_t _millis) {
    millis = _millis; 
    normalize ();   
  }
  
  void setSeconds (int16_t sec) {
    millis = (int32_t)sec * 1000; 
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
    if ((days ==0) && (millis ==0)) return 1;
    return 0;
  }
  
  
  int32_t getDays () {
    return days;
  }
  
  int32_t getMillis () {
    return millis;
  }
  
  // calc from UNIX time 01.01.1970
  RemoteXYDate getDate () {
    RemoteXYDate date;
    int32_t d;
    
    d = millis;
    date.millis = d % 1000;
    d = d / 1000;
    date.second = d % 60;
    d = d / 60;
    date.minute = d % 60;
    d = d / 60;
    date.hour = d % 24;
    
    d = days;
    date.dayOfWeek = ((d+3) % 7) + 1;
    uint16_t year = 1970;
    uint8_t leap;
    uint16_t dpy;
    uint8_t dpm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t m;
    
    while (true) {
      leap = 0;
      if ((year & 3) == 0) {
        if ((year % 100) == 0) {
          if ((year % 400) == 0) leap = 1;
        }
        else leap = 1;
      }
      dpy = 365 + leap;
      if (d < dpy) break;
      d -= dpy;
      year++;
    }  
                
    dpm[1] = (leap == 0) ? 28 : 29;
    for (m=0; m<12; m++) {
      if (d < dpm[m]) break;
      d -= dpm[m];
    }
    date.day = d + 1;
    date.month = m + 1;
    date.year = year;
    return date;
  }
  
  
  /*
  void setDate (RemoteXYDate date) {
    millis = (int32_t)date.hour * REMOTEXY_MILLIS_PER_HOUR;
    millis += (int32_t)date.minutes * REMOTEXY_MILLIS_PER_MINUTE;   
    millis += (int32_t)date.seconds * 1000;   
    millis += millis;   
    
  }
  */
    
  int64_t getUnixMillis () {
    return (int64_t)days * REMOTEXY_MILLIS_PER_DAY + (int64_t)millis;
  }
  
  void setUnixMillis (int64_t ms) {
    days = ms / REMOTEXY_MILLIS_PER_DAY;
    millis = ms % REMOTEXY_MILLIS_PER_DAY;
  }
  
};



#endif //RemoteXYTime_h