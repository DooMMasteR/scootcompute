scootcompute
============

A simple bord computer for scooters

It is arduino based and uses an LCD to display information

* air temperature: Dallas/Maxim DS18B20 1-wire themometer
* oil temperature: TI LM35 analog thermometer 
* battery current: Allegro AC709 current sensor
* voltage: on board ADC via voltage divider
* engine revs: via ICP interrupt of the ATmega

The display is ST7565 based but any display supported by U8Glib should more or less plug and play.

Pictures of the current state: http://goo.gl/qn9DZ