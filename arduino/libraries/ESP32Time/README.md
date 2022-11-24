# ESP32Time

Arduino Library for ESP32 Time Adjustment

## 機能

  * ESP32内部の時計をNTPから取得した時刻に設定する．
  * 1時間ごとに自動で時計を合わせる(FreeRTOSのタスクによりバックグラウンドで実行)

## 使い方

  1. WiFiに接続する
  1. `setup()`内で`ESP32Time.begin()`を呼んで時刻合わせタスクを起動する
  1. 時刻はC言語標準関数の`time()`で取得できる

