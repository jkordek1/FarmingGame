# FarmingGame

[![Version](https://img.shields.io/github/v/release/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/releases/tag/Initial)
[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/pulls)

## About

<p align="center">
  <img src="https://github.com/jkordek1/FarmingGame/blob/main/Images/erasmus.png?raw=true">
</p>

This is a farming themed electronic board game. You plant crops on fields and earn points by harvesting them when they are fully grown. The weather isn't always optimal for the crops and they don't grow as much. Your job is to develop the strategy for earning as many points as you can in a number of turns. Every crop needs a different number of turns to be fully grown and therefore it awards you with different number of points.


## Project idea
The idea was to make a 2-4 players electronic board game that binds STEM field with agriculture. It was funded throught Erasmus+ KA2 project [SEED2STEM: Planting the future of education](https://www.tvz.hr/introducing-seed2stem-planting-the-future-of-education/).

## Project status
- [x] Breadboard prototype
- [x] Schematic
- [x] PCBs
- [x] Enclosure design
- [x] 3D CAD assembly and renders
- [x] Assembly
- [x] Programming

## Issues
- Hardware is completely functional but software could use some extra work

## Main features
- Based on popular LOLIN32 ESP32 microcontroller board
- WS2812B-V5 addressable LEDs with no external components needed
- 4x SSD1306 0.96" OLED displays
- 1x GC9A01 IPS circular display

## 3D renders
<p align="center">
  <img width="1000" src="https://raw.githubusercontent.com/jkordek1/FarmingGame/refs/heads/main/Images/Full2.png?token=GHSAT0AAAAAADAQO5WR6OSTDIB2TXIP34XKZ6UGYQA">
</p>
<p align="center">
 <img width="1000" src="https://raw.githubusercontent.com/jkordek1/FarmingGame/refs/heads/main/Images/Animation.gif?token=GHSAT0AAAAAADAQO5WQXRZWHWDRE5YH4S3AZ6UGQCQ">
</p>

## Images


## PCBs
There are 3 different PCBs:

**BoardGame** - main 150x150mm PCB that contains LEDs and most of the electronic components (1 needed)



**Dice** - small PCB used as a touchpad (1 needed)



**Touchpad** - PCB with 3 touchpads used as controls for the game (4 needed)




## Enclosure
There are few 3D models files needed for enclosure:
1. Case - main part of the enclosure
2. FieldBoard - Upper part of the enclosure printed with black and white filaments

## Project folder structure
