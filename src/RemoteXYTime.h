#ifndef RemoteXYTime_h
#define RemoteXYTime_h 

#define  REMOTEXY_MILLIS_PER_DAY 86400000
#define  REMOTEXY_MILLIS_PER_HOUR 3600000
#define  REMOTEXY_MILLIS_PER_MINUTE 60000
#define  REMOTEXY_MILLIS_PER_SECOND 1000
#define  REMOTEXY_SECONDS_PER_DAY 86400
#define  REMOTEXY_MINUTES_PER_DAY 1440
#define  REMOTEXY_HOURS_PER_DAY 24
#define  REMOTEXY_SECONDS_PER_MINUTE 60                
#define  REMOTEXY_MINUTES_PER_HOUR 60

const char REMOTEXY_TIMEFORMAT_YYYY[] PROGMEM = "YYYY"; // year, 2024 , can use yyyy
const char REMOTEXY_TIMEFORMAT_YY[] PROGMEM =   "YY";   // year, 24 , can use yy
const char REMOTEXY_TIMEFORMAT_MMMM[] PROGMEM = "MMMM"; // month, January - December
const char REMOTEXY_TIMEFORMAT_MMM[] PROGMEM =  "MMM";  // month, Jan - Dec
const char REMOTEXY_TIMEFORMAT_MM[] PROGMEM =   "MM";   // month, 01 - 12
const char REMOTEXY_TIMEFORMAT_dd[] PROGMEM =   "dd";   // day, 01 - 31
const char REMOTEXY_TIMEFORMAT_d[] PROGMEM =    "d";    // day, 1 - 31
const char REMOTEXY_TIMEFORMAT_EEEE[] PROGMEM = "EEEE"; // day of week, Sunday - Saturday
const char REMOTEXY_TIMEFORMAT_EEE[] PROGMEM =  "EEE";  // day of week, Sun - Sat
const char REMOTEXY_TIMEFORMAT_u[] PROGMEM =    "u";    // day of week, 1 - 7, 1 is Monday
const char REMOTEXY_TIMEFORMAT_HH[] PROGMEM =   "HH";   // hour, 00 - 23
const char REMOTEXY_TIMEFORMAT_hh[] PROGMEM =   "hh";   // hour, 1 - 24
const char REMOTEXY_TIMEFORMAT_h[] PROGMEM =    "h";    // hour, 1 - 12
const char REMOTEXY_TIMEFORMAT_mm[] PROGMEM =   "mm";   // minute, 00 - 59
const char REMOTEXY_TIMEFORMAT_ss[] PROGMEM =   "ss";   // second, 00 - 59
const char REMOTEXY_TIMEFORMAT_SSS[] PROGMEM =  "SSS";  // millisecond, 000 - 999
const char REMOTEXY_TIMEFORMAT_aa[] PROGMEM =   "aa";   // AM or PM

const char REMOTEXY_TIME_AM[] PROGMEM =  "AM";  
const char REMOTEXY_TIME_PM[] PROGMEM =  "PM";  


const char REMOTEXY_MONTHS_FULLNAME[][10] PROGMEM = {                                                                   
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};
const char REMOTEXY_MONTHS_SHORTNAME[][4] PROGMEM = {                                                                   
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
const char REMOTEXY_DAYSOFWEEK_FULLNAME[][10] PROGMEM = {                                                                   
  "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};
const char REMOTEXY_DAYSOFWEEK_SHORTNAME[][4] PROGMEM = {                                                                   
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

const uint8_t REMOTEXY_DAYSINMONTHS[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define  REMOTEXY_DAYS_UP_TO_2025 20089


class RemoteXYTime {
  public:
  int16_t millis;
  int8_t second;
  int8_t minute;
  int8_t hour;
  int8_t day;
  int8_t month;
  int16_t year;
  int8_t dayOfWeek;  // if 0 then dayOfWeek is undefuned
  
  RemoteXYTime () {
    set (1970, 1, 1);
  }

  RemoteXYTime (int64_t ts) { 
    set (ts);
  }
  
  RemoteXYTime (const RemoteXYTime &ts) { 
    set (ts);
  }

  RemoteXYTime (int16_t _year, int8_t _month = 1, int8_t _day = 1, int8_t _hour = 0, int8_t _minute = 0, int8_t _second = 0, int16_t _millis = 0) {
    set (_year, _month, _day, _hour, _minute, _second, _millis);
  }
  
    
  void set (int16_t _year, int8_t _month, int8_t _day, int8_t _hour = 0, int8_t _minute = 0, int8_t _second = 0, int16_t _millis = 0) {
    setDate (_year, _month, _day);
    setTime (_hour, _minute, _second, _millis);
  }
  
  void set (const RemoteXYTime &t) {
    set (t.year, t.month, t.day, t.hour, t.minute, t.second, t.millis);
  }
  
  void setDate (int16_t _year, int8_t _month = 1, int8_t _day = 1) {
    year = _year;
    month = _month;
    day = _day;
    normalizeDate ();
  }
  
  void setTime (int8_t _hour, int8_t _minute = 0, int8_t _second = 0, int16_t _millis = 0) {
    hour = _hour;
    minute = _minute;
    second = _second;
    millis = _millis;
    normalizeTime ();
  }


  
  
  static uint8_t leapYear (uint16_t _year) {
    if ((_year & 3) == 0) {
      if ((_year % 100) == 0) {
        if ((_year % 400) == 0) return 1;
      }
      else return 1;
    }
    return 0;  
  }
  
  void normalizeTime () {
    if (hour < 0) hour = 0;
    if (hour >= REMOTEXY_HOURS_PER_DAY) hour = REMOTEXY_HOURS_PER_DAY - 1;
    if (minute < 0) minute = 0;
    if (minute >= REMOTEXY_MINUTES_PER_HOUR) minute = REMOTEXY_MINUTES_PER_HOUR - 1;
    if (second < 0) second = 0;
    if (second >= REMOTEXY_SECONDS_PER_MINUTE) second = REMOTEXY_SECONDS_PER_MINUTE - 1;
    if (millis < 0) millis = 0;
    if (millis >= REMOTEXY_MILLIS_PER_SECOND) millis = REMOTEXY_MILLIS_PER_SECOND - 1;  
  }
  
  // check day and set day of week
  void normalizeDate () {
    if (month <= 0) month = 1;
    if (month > 12) month = 12;    
    uint8_t md = pgm_read_byte (REMOTEXY_DAYSINMONTHS+month-1);
    if (month == 2) md += RemoteXYTime::leapYear (year);
    if (day <= 0) day = 1;
    if (day > md) day = md;
    dayOfWeek = ((getDaysSince1970 ()+3) % 7) + 1; 
  }
  
  // compare functions 
  
  int8_t compareDate (const RemoteXYTime time) {
    if (time.year > year) return 1;
    else if (time.year < year) return -1;
    if (time.month > month) return 1;
    else if (time.month < month) return -1;
    if (time.day > day) return 1;
    else if (time.day < day) return -1;
    return 0;
  }
  
  int8_t compareTime (const RemoteXYTime time) {
    if (time.hour > hour) return 1;
    else if (time.hour < hour) return -1;
    if (time.minute > minute) return 1;
    else if (time.minute < minute) return -1;
    if (time.second > second) return 1;
    else if (time.second < second) return -1;
    if (time.millis > millis) return 1;
    else if (time.millis < millis) return -1;
    return 0;
  }
  
  int8_t compare (const RemoteXYTime time) {
    int8_t c = compareDate (time);
    if (c == 0) return compareTime (time);
    return c;
  }
 
 
  // year functions
  
  void addYears (int16_t years) {
    year += years;
    normalizeDate ();
  }
  
  void setYear (int16_t _year) {
    year = _year;
    normalizeDate ();
  }
  
  void setToYearStart () {
    day = 1;
    month = 1;
    normalizeDate ();
    setToDayStart ();
  }
  
  void setToYearEnd () {
    day = 31;
    month = 12;
    normalizeDate ();
    setToDayEnd ();
  }
 
  // month function
  void addMonths (int16_t months) {
    months += month;
    while (months > 12) {
      months -= 12;
      year++;
    }
    while (months <= 0) {
      months += 12;
      year--;
    }
    month = months;
    normalizeDate ();
  }
  
  void setMonth (int16_t _month) {
    month = _month;
    normalizeDate ();
  }
  
  void setToMonthStart () {
    day = 1;
    normalizeDate ();
    setToDayStart ();
  }
  
  void setToMonthEnd () {
    day = 31;
    normalizeDate ();
    setToDayEnd ();
  }
  
  // week functions
  void addWeeks (int16_t weeks) {
    addDays (weeks * 7);
  }
  
  // go to day of week
  void setDayOfWeek (int8_t _dayOfWeek) {
    normalizeDate ();
    if (_dayOfWeek <= 0) _dayOfWeek = 1;
    if (_dayOfWeek > 7) _dayOfWeek = 7;
    if (_dayOfWeek != dayOfWeek) addDays (_dayOfWeek - dayOfWeek);
  }
    
  
  // day functions
  void addDays (int32_t days) {
    int64_t ts = getTimeStamp ();
    ts += (int64_t)days * (int64_t)REMOTEXY_MILLIS_PER_DAY;
    set (ts);
  }    
  
  void setDay (int8_t _day) {
    day = _day;
    normalizeDate ();
  }

  void setToDayStart () {
    hour = 0;
    setToHourStart ();
  }

  void setToDayEnd () {
    hour = REMOTEXY_HOURS_PER_DAY - 1;
    setToHourEnd ();
  }
     
  uint8_t equalDate (RemoteXYTime date) {
    return compareDate (date) == 0 ? 1 : 0;
  }

  
  // time functions
  void addHours (int32_t hours) {
    int64_t ts = getTimeStamp ();
    ts += (int64_t)hours * (int64_t)REMOTEXY_MILLIS_PER_HOUR;
    set (ts);  
  }                   
  
  void setHour (int8_t _hour) {
    hour = _hour;
    normalizeTime ();
  }
  
  void setToHourStart () {
    minute = 0;
    second = 0;
    millis = 0;
  }  
  
  void setToHourEnd () {
    minute = REMOTEXY_MINUTES_PER_HOUR - 1;
    second = REMOTEXY_SECONDS_PER_MINUTE - 1;
    millis = REMOTEXY_MILLIS_PER_SECOND - 1;
  }    

  void addMinutes (int32_t minutes) {
    int64_t ts = getTimeStamp ();
    ts += (int64_t)minutes * (int64_t)REMOTEXY_MILLIS_PER_MINUTE;
    set (ts);  
  }
  
  void setMinute (int8_t _minute) {
    minute = _minute;
    normalizeTime ();
  }
 
  void addSeconds (int32_t seconds) {
    int64_t ts = getTimeStamp ();
    ts += (int64_t)seconds * (int64_t)REMOTEXY_MILLIS_PER_SECOND;
    set (ts);  
  }
  
  void setSecond (int8_t _second) {
    second = _second;
    normalizeTime ();
  }
  
  void addMillis (int32_t _millis) {
    addMillis ((int64_t)_millis);
  } 
  
  void addMillis (int64_t _millis) {
    int64_t ts = getTimeStamp ();
    ts += _millis;
    set (ts);  
  }
  
  void setMillis (int16_t _millis) {
    millis = _millis;
    normalizeTime ();
  }
  
  
  void applyTimeZone  (int16_t timeZoneMinutes) {
    addMinutes (timeZoneMinutes);
  }
  
  uint8_t isEmpty () {
    if ((year == 1970) && (month == 1) && (day == 1) && (hour == 0) && (minute == 0) && (second == 0) && (millis == 0)) {
      return 1;
    }
    return 0;
  }
    
  int32_t getMillisSinceStartOfDay () {
    int32_t _millis = (int32_t)hour * REMOTEXY_MILLIS_PER_HOUR;
    _millis += (int32_t)minute * REMOTEXY_MILLIS_PER_MINUTE;   
    _millis += (int32_t)second * REMOTEXY_MILLIS_PER_SECOND;   
    _millis += (int32_t)millis;   
    return _millis;
  }
  
  int32_t getDaysSince1970 () {
    if (year < 1970) return 0;
    uint8_t leap = 0;
    int32_t days = 0;
    int16_t y = 1970;
    
    if (year >= 1970) {
      if (year >= REMOTEXY_DAYS_UP_TO_2025) {
        y = 2025;
        days += REMOTEXY_DAYS_UP_TO_2025;
      }
      
      while (y <= year) {
        leap = RemoteXYTime::leapYear (y);
        if (y < year) days += 365 + leap;
        y++;
      }  
    }
    else {
      while (y > year) {
        y--;
        leap = RemoteXYTime::leapYear (y);
        days -= (365 + leap);
      }      
    }
    
    int8_t m = 0;
    while (m < month-1) {
      days += pgm_read_byte(REMOTEXY_DAYSINMONTHS+m);
      if (m == 1) days += leap;
      m++;
    }
    days += day-1;
    return days;
  }
  
  // since 1 Jan 1970  
  int64_t getTimeStamp () {
    int64_t timeStamp = getDaysSince1970 ();
    timeStamp *= REMOTEXY_MILLIS_PER_DAY;
    timeStamp += getMillisSinceStartOfDay();
    return timeStamp;
  }
  
  
  // calc from UNIX time 01.01.1970
  void set (const int64_t &ts) {
  
    int32_t td = ts / REMOTEXY_MILLIS_PER_DAY;
    int32_t tm = ts % REMOTEXY_MILLIS_PER_DAY;
    
    millis = tm % REMOTEXY_MILLIS_PER_SECOND;
    tm = tm / REMOTEXY_MILLIS_PER_SECOND;
    second = tm % REMOTEXY_SECONDS_PER_MINUTE;
    tm = tm / REMOTEXY_SECONDS_PER_MINUTE;
    minute = tm % REMOTEXY_MINUTES_PER_HOUR;
    tm = tm / REMOTEXY_MINUTES_PER_HOUR;
    hour = tm % REMOTEXY_HOURS_PER_DAY;
    
    dayOfWeek = ((td+3) % 7) + 1;
    uint8_t leap;
    uint16_t y;
    if (td >= REMOTEXY_DAYS_UP_TO_2025) {
      y = 2025;
      td -= REMOTEXY_DAYS_UP_TO_2025;
    }
    else y = 1970;
    uint16_t dy;
    
    while (true) {
      leap = RemoteXYTime::leapYear (y);
      dy = 365 + leap;
      if (td < dy) break;
      td -= dy;
      y++;
    }  
               
    int8_t m = 0;
    while (m < 12) {
      dy = pgm_read_byte(REMOTEXY_DAYSINMONTHS+m);
      if (m == 1) dy+= leap;
      if (td < dy) break;
      td -= dy;
      m++;
    }
    
    day = td + 1;
    month = m + 1;
    year = y;
  }
    
  
  char * format (char * str, const char * tmpl) {
    uint8_t d;
    while (*tmpl) {
      d = 0;
      if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_YYYY), 1)) {
        str = rxy_intToFixedStr (year, str, 4, '0');
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_YY), 1)) {
        str = rxy_intToFixedStr (year, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_MMMM), 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, FPSTR(REMOTEXY_MONTHS_FULLNAME[month-1]));
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_MMM), 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, FPSTR(REMOTEXY_MONTHS_SHORTNAME[month-1]));
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_MM), 0)) {
        str = rxy_intToFixedStr (month, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_dd), 0)) {
        str = rxy_intToFixedStr (day, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_d), 0)) {
        str = rxy_intToStr (day, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_u), 0)) {
        if (dayOfWeek > 0) str = rxy_intToStr (dayOfWeek, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_EEEE), 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, FPSTR(REMOTEXY_DAYSOFWEEK_FULLNAME[dayOfWeek-1]));
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_EEE), 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, FPSTR(REMOTEXY_DAYSOFWEEK_SHORTNAME[dayOfWeek-1]));
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_HH), 0)) {
        str = rxy_intToFixedStr (hour, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_hh), 0)) {
        str = rxy_intToStr (hour+1, str);
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_h), 0)) {
        uint8_t h = hour % 12;
        if (h == 0) h = 12;
        str = rxy_intToStr (h, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_mm), 0)) {
        str = rxy_intToFixedStr (minute, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_ss), 0)) {
        str = rxy_intToFixedStr (second, str, 2, '0');
        d=2;
      }     
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_SSS), 0)) {
        str = rxy_intToFixedStr (millis, str, 3, '0');
        d=3;
      }     
      else if (rxy_strCompareLeft (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_aa), 0)) {
        if (hour < 12) str = rxy_strCopy (str, FPSTR(REMOTEXY_TIME_AM)); 
        else str = rxy_strCopy (str, FPSTR(REMOTEXY_TIME_PM));
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
  
  char * format (char * str, const __FlashStringHelper * tmplf) {
    char *tmpl = (char*)malloc (rxy_strLength(tmplf) + 1);
    rxy_strCopy (tmpl, tmplf);
    char * p = format (str, tmpl);
    free (tmpl);
    return p;
  }
  
  
#if defined (ARDUINO)

  String format (const char * tmpl) {
    uint16_t len = rxy_strLength(tmpl)+1;
    len += 5 * rxy_strSearchCount (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_MMMM), 0);
    len += 5 * rxy_strSearchCount (tmpl, FPSTR(REMOTEXY_TIMEFORMAT_EEEE), 0);
    char *str = (char*)malloc (len);
    format (str, tmpl);
    String s = String (str);
    free (str);
    return s;
  }
  
  String format (const __FlashStringHelper * tmplf) {
    char *tmpl = (char*)malloc (rxy_strLength(tmplf) + 1);
    rxy_strCopy (tmpl, tmplf);
    String s = format (tmpl);
    free (tmpl);
    return s;
  }
  
  String format (String tmpl) {
    return format (tmpl.c_str());
  }
  
#endif // ARDUINO
      
  // operators
  
  RemoteXYTime operator=(const RemoteXYTime& time) {
    set (time);
    return *this;
  } 
  
  RemoteXYTime operator=(const int64_t& ts) {
    set (ts);
    return *this;
  } 
  
  RemoteXYTime operator+(const int64_t _millis) const {
    RemoteXYTime time = *this;
    time.addMillis (_millis);
    return time;
  }

  RemoteXYTime operator-(const int64_t _millis) const {
    RemoteXYTime time = *this;
    time.addMillis (-_millis);
    return time;
  }
  
  RemoteXYTime operator+=(const int64_t _millis) {
    addMillis (_millis);
    return *this;
  }  
  
  RemoteXYTime operator-=(const int64_t _millis) {
    addMillis (-_millis);
    return *this;
  }  
  
  RemoteXYTime operator++() {
    addMillis ((int64_t)1);
    return *this;
  } 
  
  RemoteXYTime operator--() {
    addMillis ((int64_t)-1);
    return *this;
  } 

  bool operator<(const RemoteXYTime& time) {
    return compare (time) > 0;
  }
  
  bool operator<(const int64_t& ts) {
    return getTimeStamp () < ts;
  }
  
  bool operator<=(const RemoteXYTime& time) {
    return compare (time) >= 0;
  }
  
  bool operator<=(const int64_t& ts) {
    return getTimeStamp () <= ts;
  }
        
  bool operator>(const RemoteXYTime& time) {
    return compare (time) < 0;
  }
  
  bool operator>(const int64_t& ts) {
    return getTimeStamp () > ts;
  }
  
  bool operator>=(const RemoteXYTime& time) {
    return compare (time) <= 0;
  }
  
  bool operator>=(const int64_t& ts) {
    return getTimeStamp () >= ts;
  }  

  bool operator==(const RemoteXYTime& time) {
    return compare (time) == 0; 
  }  
  
  bool operator==(const int64_t& ts) {
    return getTimeStamp () == ts;
  }   
  
  bool operator!=(const RemoteXYTime& time) {
    return compare (time) != 0; 
  }   
 
  bool operator!=(const int64_t& ts) {
    return getTimeStamp () != ts;
  } 
  
};



#endif //RemoteXYTime_h       