/**
  @file     ESP32Time.h
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

#pragma once

#include <WiFi.h>
#include "freertos/task.h"

class ESP32Time;
extern class ESP32Time ESP32Time;

class ESP32Time {
  public:
    ESP32Time(char const *time_zone = "GMT-2");
    ~ESP32Time();

    void begin(int stack_size = 1024, int priority = 0);
    void set_time();

  private:
    const char* time_zone;
    xTaskHandle handle;

    void periodicTimeAdjustmentTask();
    bool getNtpTime(struct timeval * tvp);
};

