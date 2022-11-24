/**
  @file     ESP32Time.ino
  @author   kerikun11
  @date     2017.08.29
*/

#include <WiFi.h>
#include <ESP32Time.h>

const char* ssid = "";
const char* psk = "";

void setup() {
  Serial.begin(115200);

  log_i("connecting to %s ...", ssid);
  WiFi.begin(ssid, psk);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    log_e("WiFi connection failed:(");
    while (1) delay(1000);
  }

  ESP32Time.begin(); //< adjusted the time
}

void loop() {
  time_t t = time(NULL);
  struct tm *t_st;
  t_st = localtime(&t);
  log_d("year: %d", 1900 + t_st->tm_year);
  log_d("month: %d", 1 + t_st->tm_mon);
  log_d("month day: %d", t_st->tm_mday);
  log_d("week day: %c%c", "SMTWTFS"[t_st->tm_wday], "uouehra"[t_st->tm_wday]);
  log_d("year day: %d", 1 + t_st->tm_yday);
  log_d("hour: %d", t_st->tm_hour);
  log_d("minute: %d", t_st->tm_min);
  log_d("second: %d", t_st->tm_sec);
  log_d("ctime: %s", ctime(&t));
  delay(1000);
}

