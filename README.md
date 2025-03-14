# FarmingGame

[![Version](https://img.shields.io/github/v/release/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/releases/tag/Initial)
[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/pulls)

## About

Mensch Ă¤rgere Dich nicht (English: Man, Don't Get Angry) is a German board game developed by Josef Friedrich Schmidt in 1907/1908.
The name derives from the fact that a peg is sent back to the "out" field when another peg lands on it, similar to the later game Sorry!.

More information about the game can be found at the [wiki page](https://en.wikipedia.org/wiki/Mensch_%C3%A4rgere_Dich_nicht).

## Project idea
Idea of the project is to create an electronic version of the game to help promoting STEM education (and to have some fun of course).

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

## Images
<p align="center">
  <img width="1000" src="https://raw.githubusercontent.com/jkordek1/FarmingGame/refs/heads/main/Images/Full2.png?token=GHSAT0AAAAAADAQO5WRSFD4EZ2NXNY4VVBMZ6UGODQ">
</p>

## 3D renders
<img src="https://raw.githubusercontent.com/jkordek1/CNLJS/main/Images/BoardGameV1.png" width="1000"/>

## PCBs
There are 3 different PCBs:

**BoardGame** - main 150x150mm PCB that contains LEDs and most of the electronic components (1 needed)

<img src="https://raw.githubusercontent.com/jkordek1/Man-Don-t-Get-Angry-Board-game/main/Images/BoardGame-front%20and%20back.png" width="1000"/>

**Dice** - small PCB used as a touchpad (1 needed)

<img src="https://raw.githubusercontent.com/jkordek1/Man-Don-t-Get-Angry-Board-game/main/Images/Dice-front%20and%20back.png"  width="600"/>

**Touchpad** - PCB with 3 touchpads used as controls for the game (4 needed)

<img src="https://raw.githubusercontent.com/jkordek1/Man-Don-t-Get-Angry-Board-game/main/Images/Touchpad-front%20and%20back.png"  width="600"/>


## Enclosure
There are few 3D models files needed for enclosure:
1. Case - main part of the enclosure
2. FieldBoard - Upper part of the enclosure printed with black and white filaments

## Project folder structure
