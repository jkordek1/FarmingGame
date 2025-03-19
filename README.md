# FarmingGame

[![Version](https://img.shields.io/github/v/release/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/releases/tag/Initial)
[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/jkordek1/Man-Don-t-Get-Angry-Board-game)](https://github.com/jkordek1/Man-Don-t-Get-Angry-Board-game/pulls)

## About
<p align="center">
  <img src="https://github.com/jkordek1/FarmingGame/blob/main/Images/erasmus.png?raw=true">
</p>

This is a farming-themed electronic board game where players strategically plant and harvest crops to earn points. Each crop requires a different number of turns to fully grow, with longer-growing crops yielding higher rewards. However, unpredictable weather conditions can slow down growth, making it crucial to plan wisely. Your goal is to develop the best strategy to maximize your harvest and score as many points as possible within a set number of turns.


## Project idea
The idea was to make a 2-4 players electronic board game that binds STEM field with agriculture. It was funded through Erasmus+ KA2 project [SEED2STEM: Planting the future of education](https://www.tvz.hr/introducing-seed2stem-planting-the-future-of-education/).

## Project status
- [x] Breadboard prototype
- [x] Schematic
- [x] PCBs
- [x] Enclosure design
- [x] 3D CAD assembly and renders
- [x] Assembly
- [x] Programming
- [ ] Final testing phase
- [ ] Adjust the game points for balance.

## Issues
- Hardware is completely functional, software could use some extra work

## Hardware
- Based on popular LOLIN32 ESP32 microcontroller board
- WS2812B-V5 addressable LEDs with no external components needed
- 4x SSD1306 0.96" OLED displays
- 1x GC9A01 IPS circular display
- powered by microUSB cable

## 3D renders
![Full1](https://github.com/user-attachments/assets/fab94822-5d9b-4d8f-8514-ddaf97034429)
![Animation](https://github.com/user-attachments/assets/696d76b8-bb1a-48e9-8c86-f5cb82f0d0d6)

## Images
![IRL](https://github.com/user-attachments/assets/e725a37f-8255-4fd5-af57-f00499a43b76)

## PCB
There is a big donut shaped PCB that contains all SMD and THD componentes.

## Enclosure
The enclosure is composed of three distinct sections: an upper orange section, and middle and lower sections, both in white. Aside from that, there are also three buttons in four different colors used for menu control. There is also a 3D-printed piece that represents four soil spots for each player.

## Project folder structure
    ├── ...
    ├── Images                  # Images of the project
    ├── 3D models               # Enclosure model parts
    ├── KiCAD Files             # Main folder
    │   ├── FullPCB           # 3d models of components
    │       ├── BoardGame           # KiCAD project for main PCB
    └── ...
