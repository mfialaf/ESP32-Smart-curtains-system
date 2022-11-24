/**
  @file     ESP32Time.cpp
  @author   kerikun11

  @section  LICENSE

  MIT License

  Copyright (c) 2017 Ryotaro Onuki

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "ESP32Time.h"

#include <WiFiUdp.h>
#include <ctime>
#include "esp32-hal-log.h"

#define NTP_SERVER_IPADDRESS  IPAddress(133, 243, 238, 164)
#define NTP_LOCAL_PORT        2390
#define NTP_PACKET_SIZE       48

class ESP32Time ESP32Time;

ESP32Time::ESP32Time(char const *time_zone): time_zone(time_zone), handle(NULL) {}

ESP32Time::~ESP32Time() {
  if (handle != NULL) vTaskDelete(handle);
}

void ESP32Time::begin(int stack_size, int priority) {
  xTaskCreate([](void* arg) {
    static_cast<ESP32Time*>(arg)->periodicTimeAdjustmentTask();
  }, "ESP32Time", stack_size, this, priority, &handle);
  if (WiFi.status() == WL_CONNECTED) set_time();
}

void ESP32Time::set_time() {
  struct timeval tv;
  if (!getNtpTime(&tv)) {
    log_w("NTP Failed:(");
    return;
  }
  struct timezone tz;
  setenv("TZ", time_zone, 1);
  tzset();
  tz.tz_minuteswest = 0;
  tz.tz_dsttime = 0;
  settimeofday(&tv, &tz);
  time_t t = time(NULL);
  log_d("ctime: %s", ctime(&t));
}

void ESP32Time::periodicTimeAdjustmentTask() {
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  while (1) {
    vTaskDelayUntil(&xLastWakeTime, 24 * 60 * 60 * 1000 / portTICK_RATE_MS);
    if (WiFi.status() == WL_CONNECTED) set_time();
  }
}

bool ESP32Time::getNtpTime(struct timeval * tvp) {
  WiFiUDP udp;
  udp.begin(NTP_LOCAL_PORT);
  byte packetBuffer[NTP_PACKET_SIZE];
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(NTP_SERVER_IPADDRESS, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
  for (int i = 0; udp.parsePacket() < NTP_PACKET_SIZE; i++) {
    if (i > 1000) {
      log_e("No NTP Response :-(");
      return false;
    }
    delay(1);
  }
  udp.read(packetBuffer, NTP_PACKET_SIZE);
  unsigned long secsSince1900, afterTheDecimalPoint;
  secsSince1900  =  (unsigned long)packetBuffer[40] << 24;
  secsSince1900 |=  (unsigned long)packetBuffer[41] << 16;
  secsSince1900 |=  (unsigned long)packetBuffer[42] <<  8;
  secsSince1900 |=  (unsigned long)packetBuffer[43] <<  0;
  afterTheDecimalPoint  =  (unsigned long)packetBuffer[44] << 24;
  afterTheDecimalPoint |=  (unsigned long)packetBuffer[45] << 16;
  afterTheDecimalPoint |=  (unsigned long)packetBuffer[46] <<  8;
  afterTheDecimalPoint |=  (unsigned long)packetBuffer[47] <<  0;
  tvp->tv_sec = secsSince1900 - 2208988800UL;
  tvp->tv_usec = (float)afterTheDecimalPoint * 1000000 / (uint64_t)0x100000000;
  return true;
}

