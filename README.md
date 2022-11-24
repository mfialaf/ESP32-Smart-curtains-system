# Systém chytrých okenních závěsů založený na ESP32

Projekt obsahuje dokumenty potřebné k implementaci systému chytrých okenních závěsů. Projekt byl vyvíjen v Arduino IDE a prototyp obsahuje vytvořenou PCB desku.
![Alt text](/pictures/prototype.jpg "Final prototype")

## O projektu
Systém dokáže reagovat na vnější podněty z okolí pomocí světělného čidla, nastavovat polohu závěsů podle zvoleného času a dne v týdny či upravovat pozici závěsu kdekoli v rozmezí 0% - otevřeno až 100% - zavřeno.

Posouvání závěsů je realizováno pomocí krokového motoru. Výpočetní jednotkou je ESP32, které konrtoluje všechny připojené periferie a komunikuje se serverem pomocí MQTT protokolu. Při konstruování prototypu byla využita aplikace Home Assistant, která sloužila jako server s uživatelským rozhraním.

## About smart window curtains
The system can react to external light with a light sensor, change the position of the curtains according to the selected time and day of the week or adjust the position anywhere from 0% - open to 100% - closed.

A stepper motor realizes the curtains movement. The ESP32 microcontroller controls all connected peripherals and communicates with the server using the MQTT protocol. When designing a prototype, the Home Assistant application was chosen as the server with a friendly user interface.
[English article link](https://medium.com/@mafialka/smart-curtains-system-150fcbf853d2)

## Komponenty/components
- [ESP32](https://www.espressif.com/en/products/devkits/esp32-devkitc)
- [krokový motor / stepper motor NEMA17](https://www.gme.cz/krokovy-motor-nema-17)
- [ovladač motou / motor driver TMC2209](https://www.majkl3d.cz/p/tmc2209-driver-ovladac-motoru)
- [světelný senzor / light sensor 1750](https://botland.cz/svetelne-a-barevne-senzory/2024-senzor-intenzity-svetla-bh1750-5904422373283.html)
- [Zdroj / Power supply](https://www.gme.cz/napajeci-adapter-sitovy-12v-1000ma-5-5-2-1mm-vigan)
- [Thermistor](https://www.gme.cz/termistor-ntc-ntc-640-10k)
- [Reed switch](https://www.gme.cz/jazyckovy-magneticky-senzor-meder-ksk1a66-1020)
- [DC konektor](https://www.gme.cz/napajeci-souosy-konektor-wealthmetal-ds-214b)
