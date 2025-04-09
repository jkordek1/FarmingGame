# FarmingGame

[![Version](https://img.shields.io/github/v/release/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/releases/tag/Initial)
[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/pulls)

## About
<p align="center">
  <img src="https://github.com/jkordek1/FarmingGame/blob/main/Images/erasmus.png?raw=true">
</p>

This is an electronic board game about farming in which players strategically plant and harvest crops to earn points. Each plant requires a different number of turns to grow fully, with longer growing plants bringing a higher reward. However, unpredictable weather conditions can slow down growth, so it's crucial to plan wisely. Your goal is to develop the best strategy to maximize your harvest and score as many points as possible within a given number of turns.


## Project idea
The idea was to develop an electronic board game for 2-4 players that combines STEM with agriculture. It was funded by the Erasmus+ KA2 project [SEED2STEM: Planting the future of education] (https://www.tvz.hr/introducing-seed2stem-planting-the-future-of-education/).

## FarmingGame rules

At the start of the game, you can choose whether you want to play with 2 or 4 players. As soon as at least 2 players have been selected, the first player begins his turn. Each turn consists of three phases.

In the first phase, the weather is randomly generated, with the possible weather conditions being sunny, rainy or dry. Depending on the weather conditions, the crops either grow or remain standing.

In the second phase (harvest), players can harvest the fully grown crops.

In the final phase (planting), players can plant new crops on empty fields and end their turn. When a crop is planted, the field lights up in the color of the respective crop. The circular main display shows the remaining turns until the crop is fully planted.

Players can grow the following crops/plants:

| Crop/Plant | Color  | Turns | Points |
|------------|--------|-------|--------|
| Corn       | Yellow | 1     | 1      |
| Carrot     | Orange | 2     | 3      |
| Tomato     | Red    | 2     | 4      |
| Lettuce    | Green  | 3     | 5      |
| Blueberry  | Blue   | 4     | 7      |

The number of turns required for the plants to fully grow and the points awarded for harvesting them need to be revised to balance the game properly. It is almost there, but not yet optimal. But the points you get should increase exponentially compared to the number of rounds.

The player who has scored the most points after a certain number of rounds wins.

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
- Due to ESP32 Neopixel library bug, the first WS2812B LED sometimes lights up green. This was fixed by adding a dummy LED which is not used

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
There is a donut shaped PCB that contains microcontroller, all SMD and THD componentes.

## Enclosure
The enclosure consists of three different sections: an upper orange section, a middle section and a lower section, both made of white PLA material. There are also three buttons in four different colors that are used for the menu controls. There is also a 3D printed part that represents four ground points for each player.

## Project folder structure
    ├── ...
    ├── Images                  # Images of the project
    ├── 3D models               # Enclosure model parts
    ├── KiCAD Files             # Main folder
    │   ├── FullPCB             # 3d models of components
    │       ├── BoardGame       # KiCAD project for main PCB
    └── ...
