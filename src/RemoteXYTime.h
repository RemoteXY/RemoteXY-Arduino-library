#ifndef RemoteXYTime_h
#define RemoteXYTime_h 

#include "RemoteXYTimeStamp.h"

const char * REMOTEXY_TIMEFORMAT_YYYY = "YYYY"; // year, 2024 , can use yyyy
const char * REMOTEXY_TIMEFORMAT_YY =   "yy";   // year, 24 , can use yy
const char * REMOTEXY_TIMEFORMAT_MMMM = "MMMM"; // month, January - December
const char * REMOTEXY_TIMEFORMAT_MMM =  "MMM";  // month, Jan - Dec
const char * REMOTEXY_TIMEFORMAT_MM =   "MM";   // month, 01 - 12
const char * REMOTEXY_TIMEFORMAT_dd =   "dd";   // day, 01 - 31
const char * REMOTEXY_TIMEFORMAT_d =    "d";    // day, 1 - 31
const char * REMOTEXY_TIMEFORMAT_EEEE = "EEEE"; // day of week, Sunday - Saturday
const char * REMOTEXY_TIMEFORMAT_EEE =  "EEE";  // day of week, Sun - Sat
const char * REMOTEXY_TIMEFORMAT_u =    "u";    // day of week, 1 - 7, 1 is Monday
const char * REMOTEXY_TIMEFORMAT_HH =   "HH";   // hour, 00 - 23
const char * REMOTEXY_TIMEFORMAT_h =    "h";    // hour, 1 - 12
const char * REMOTEXY_TIMEFORMAT_mm =   "mm";   // minute, 00 - 59
const char * REMOTEXY_TIMEFORMAT_ss =   "ss";   // second, 00 - 59
const char * REMOTEXY_TIMEFORMAT_SSS =  "SSS";  // millisecond, 000 - 999
const char * REMOTEXY_TIMEFORMAT_aa =   "aa";   // AM or PM

const char * REMOTEXY_TIME_AM =  "AM";  
const char * REMOTEXY_TIME_PM =  "PM";  


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

const uint8_t REMOTEXY_DAYSINMONTHS[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define  REMOTEXY_DAYS_UP_TO_2025 20089


class RemoteXYTime {
  public:
  uint16_t millis;
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  uint8_t dayOfWeek;  // if 0 then dayOfWeek is undefuned
  
  RemoteXYTime () {}

  RemoteXYTime (RemoteXYTimeStamp ts) { 
    set (ts);
  }

  RemoteXYTime (uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second, uint16_t _millis) {
    set (_year, _month, _day, _hour, _minute, _second, _millis);
  }
  
  RemoteXYTime (uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second) {
    set (_year, _month, _day, _hour, _minute, _second);
  }
  
  RemoteXYTime (uint16_t _year, uint8_t _month, uint8_t _day) {
    set (_year, _month, _day);
  }
  
    
  void set (uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second, uint16_t _millis) {
    year = _year;
    month = _month;
    day = _day;
    hour = _hour;
    minute = _minute;
    second = _second;
    millis = _millis;
    if (hour >= REMOTEXY_HOURS_PER_DAY) hour = REMOTEXY_HOURS_PER_DAY - 1;
    if (minute >= REMOTEXY_MINUTES_PER_HOUR) minute = REMOTEXY_MINUTES_PER_HOUR - 1;
    if (second >= REMOTEXY_SECONDS_PER_MINUTE) second = REMOTEXY_SECONDS_PER_MINUTE - 1;
    if (millis >= REMOTEXY_MILLIS_PER_SECOND) millis = REMOTEXY_MILLIS_PER_SECOND - 1;
    normalizeDate ();
  }
  
  void set (uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second) {
    set (_year, _month, _day, _hour, _minute, _second, 0);
  }

  void set (uint16_t _year, uint8_t _month, uint8_t _day) {
    set (_year, _month, _day, 0, 0, 0, 0);
  }
  
  static uint16_t leapYear (uint16_t _year) {
    if ((_year & 3) == 0) {
      if ((_year % 100) == 0) {
        if ((_year % 400) == 0) return 1;
      }
      else return 1;
    }
    return 0;  
  }
  
  // check day and set day of week
  void normalizeDate () {
    if (month == 0) month = 1;
    if (month > 12) month = 12;    
    uint8_t md = REMOTEXY_DAYSINMONTHS[month-1];
    if (month == 2) md += RemoteXYTime::leapYear (year);
    if (day == 0) day = 1;
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
  
  void addYear (int16_t years) {
    years += year;
    normalizeDate ();
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
  
  void startOfMonth () {
    day = 1;
    normalizeDate ();
    setStartOfDay ();
  }
  
  void endOfMonth () {
    day = 31;
    normalizeDate ();
    setEndOfDay ();
  }
  
  // week functions
  void addWeeks (int16_t week) {
    addDays (week * 7);
  }
  
  // go to day of week
  void setDayOfWeek (int8_t _dayOfWeek) {
    normalizeDate ();
    if (_dayOfWeek != dayOfWeek) addDays (_dayOfWeek - dayOfWeek);
  }
    
  
  // day functions
  void addDays (int16_t days) {
    RemoteXYTimeStamp ts = getTimeStamp ();
    ts.add (RemoteXYTimeStamp (days, 0));
    set (ts);
  }

  void setStartOfDay () {
    hour = 0;
    minute = 0;
    second = 0;
    millis = 0;
  }

  void setEndOfDay () {
    hour = REMOTEXY_HOURS_PER_DAY - 1;
    minute = REMOTEXY_MINUTES_PER_HOUR - 1;
    second = REMOTEXY_SECONDS_PER_MINUTE - 1;
    millis = REMOTEXY_MILLIS_PER_SECOND - 1;
  }
     
  uint8_t equalDate (RemoteXYTime date) {
    return compareDate (date) == 0;
  }

  
  // time functions
  void addHours (int32_t hours) {
    RemoteXYTimeStamp ts = getTimeStamp ();
    int32_t days = hours / REMOTEXY_HOURS_PER_DAY;
    hours = hours % REMOTEXY_HOURS_PER_DAY;
    ts.add (RemoteXYTimeStamp (days, hours * REMOTEXY_MILLIS_PER_HOUR));
    set (ts);  
  }

  void addMinutes (int32_t minutes) {
    RemoteXYTimeStamp ts = getTimeStamp ();
    int32_t days = minutes / REMOTEXY_MINUTES_PER_DAY;
    minutes = minutes % REMOTEXY_MINUTES_PER_DAY;
    ts.add (RemoteXYTimeStamp (days, minutes * REMOTEXY_MILLIS_PER_MINUTE));
    set (ts);  
  }
 
  void addSeconds (int32_t seconds) {
    RemoteXYTimeStamp ts = getTimeStamp ();
    int32_t days = seconds / REMOTEXY_SECONDS_PER_DAY;
    seconds = seconds % REMOTEXY_SECONDS_PER_DAY;
    ts.add (RemoteXYTimeStamp (days, seconds * REMOTEXY_MILLIS_PER_SECOND));
    set (ts);  
  }
  
  // time functions
  void addMillis (int32_t _millis) {
    RemoteXYTimeStamp ts = getTimeStamp ();
    ts.add (RemoteXYTimeStamp (0, _millis));
    set (ts);  
  }
  
    
  int32_t getMillisWithinDay () {
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
    uint16_t y = 1970;
    
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
    
    uint8_t m = 0;
    while (m < month-1) {
      days += REMOTEXY_DAYSINMONTHS[m];
      if (m == 1) days += leap;
      m++;
    }
    days += day-1;
    return days;
  }
  
  // since 1 Jan 1970  
  RemoteXYTimeStamp getTimeStamp () {
    return RemoteXYTimeStamp (getDaysSince1970 (), getMillisWithinDay());
  }
  
  
  // calc from UNIX time 01.01.1970
  void set (RemoteXYTimeStamp ts) {
    int32_t d = ts.getMillis ();
    millis = d % REMOTEXY_MILLIS_PER_SECOND;
    d = d / REMOTEXY_MILLIS_PER_SECOND;
    second = d % REMOTEXY_SECONDS_PER_MINUTE;
    d = d / REMOTEXY_SECONDS_PER_MINUTE;
    minute = d % REMOTEXY_MINUTES_PER_HOUR;
    d = d / REMOTEXY_MINUTES_PER_HOUR;
    hour = d % REMOTEXY_HOURS_PER_DAY;
    
    d = ts.getDays ();
    dayOfWeek = ((d+3) % 7) + 1;
    uint8_t leap;
    uint16_t y;
    if (d >= REMOTEXY_DAYS_UP_TO_2025) {
      y = 2025;
      d -= REMOTEXY_DAYS_UP_TO_2025;
    }
    else y = 1970;
    uint16_t dy;
    
    while (true) {
      leap = RemoteXYTime::leapYear (y);
      dy = 365 + leap;
      if (d < dy) break;
      d -= dy;
      y++;
    }  
               
    uint8_t m = 0;
    while (m < 12) {
      dy = REMOTEXY_DAYSINMONTHS[m];
      if (m == 1) dy+= leap;
      if (d < dy) break;
      d -= dy;
      m++;
    }
    
    day = d + 1;
    month = m + 1;
    year = y;
  }
    
  
  char * format (char * str, const char * tmpl) {

    while (*tmpl) {
      uint8_t d = 0;
      if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_YYYY, 1)) {
        str = rxy_intToFixedStr (year, str, 4, '0');
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_YY, 1)) {
        str = rxy_intToFixedStr (year, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_MMMM, 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, REMOTEXY_MONTHS_FULLNAME[month-1]);
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_MMM, 0)) {
        if ((month>0) && (month<=12)) {
          str = rxy_strCopy (str, REMOTEXY_MONTHS_SHORTNAME[month-1]);
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_MM, 0)) {
        str = rxy_intToFixedStr (month, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_dd, 0)) {
        str = rxy_intToFixedStr (day, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_d, 0)) {
        str = rxy_intToStr (day, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_u, 0)) {
        if (dayOfWeek > 0) str = rxy_intToStr (dayOfWeek, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_EEEE, 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, REMOTEXY_DAYSOFWEEK_FULLNAME[dayOfWeek-1]);
        }
        d=4;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_EEE, 0)) {
        if ((dayOfWeek>0) && (dayOfWeek<=7)) {
          str = rxy_strCopy (str, REMOTEXY_DAYSOFWEEK_SHORTNAME[dayOfWeek-1]);
        }
        d=3;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_HH, 0)) {
        str = rxy_intToFixedStr (hour, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_h, 0)) {
        uint8_t h = hour % 12;
        if (h == 0) h = 12;
        str = rxy_intToStr (h, str);
        d=1;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_mm, 0)) {
        str = rxy_intToFixedStr (minute, str, 2, '0');
        d=2;
      }
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_ss, 0)) {
        str = rxy_intToFixedStr (second, str, 2, '0');
        d=2;
      }     
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_SSS, 0)) {
        str = rxy_intToFixedStr (millis, str, 3, '0');
        d=3;
      }     
      else if (rxy_strCompareLeft (tmpl, REMOTEXY_TIMEFORMAT_aa, 0)) {
        if (hour < 12) str = rxy_strCopy (str, REMOTEXY_TIME_AM); 
        else str = rxy_strCopy (str, REMOTEXY_TIME_PM);
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
  
  // operators
  
  RemoteXYTime operator=(const RemoteXYTime& time) {
    year = time.year;
    month = time.month;
    day = time.day;
    hour = time.hour;
    minute = time.minute;
    second = time.second;
    millis = time.millis;
    dayOfWeek = time.dayOfWeek;
    return *this;
  } 
  
  RemoteXYTime operator+(const int32_t _millis) const {
    RemoteXYTime time = *this;
    time.addMillis (_millis);
    return time;
  }

  RemoteXYTime operator-(const int32_t _millis) const {
    RemoteXYTime time = *this;
    time.addMillis (-_millis);
    return time;
  }
  
  RemoteXYTime operator+=(const int32_t _millis) {
    addMillis (_millis);
    return *this;
  }  
  
  RemoteXYTime operator-=(const int32_t _millis) {
    addMillis (-_millis);
    return *this;
  }  
  
  RemoteXYTime operator++() {
    addMillis (1);
    return *this;
  } 
  
  RemoteXYTime operator--() {
    addMillis (1);
    return *this;
  } 

  bool operator<(const RemoteXYTime& time) {
    return compare (time) > 0;
  }
  
  bool operator<=(const RemoteXYTime& time) {
    return compare (time) >= 0;
  }
        
  bool operator>(const RemoteXYTime& time) {
    return compare (time) < 0;
  }
  
  bool operator>=(const RemoteXYTime& time) {
    return compare (time) <= 0;
  }

  bool operator==(const RemoteXYTime& time) {
    return compare (time) == 0; 
  }   
  
  bool operator!=(const RemoteXYTime& time) {
    return compare (time) != 0; 
  }   
 

};



#endif //RemoteXYTime_h       