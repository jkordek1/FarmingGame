# FarmingGame

[![Version](https://img.shields.io/github/v/release/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/releases/tag/Initial)
[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![GitHub Issues](https://img.shields.io/github/issues/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/jkordek1/FarmingGame)](https://github.com/jkordek1/FarmingGame/pulls)

## About
<p align="center">
  <img src="https://github.com/jkordek1/FarmingGame/blob/main/Images/erasmus.png?raw=true">
</p>

This is an electronic board game about farming where 2-4 players compete to grow crops and score the most points. Players strategically plant crops with different growing times and point values, while dealing with unpredictable weather that affects crop growth. The game features three difficulty modes, each with unique weather mechanics and strategic depth.

**Key Features:**
- **Multiple difficulty modes**: Simple (beginner-friendly), Medium (balanced), and Advanced (strategic with crop-specific weather effects)
- **Dynamic weather system**: Weather drawn from a "bag" of 11 cards (4 Sunny, 3 Rain, 4 Drought) that refills when empty
- **Strategic depth**: Advanced mode includes stale crops (25% points if not harvested quickly) and planting limits (2 crops per turn)
- **Physical feedback**: LED ring shows field status, OLED displays for each player, central TFT screen with crop images
- **Sound and visual effects**: Weather animations, harvest celebrations, victory fanfare


## Project idea
The idea was to develop an electronic board game for 2-4 players that combines STEM with agriculture. It was funded by the Erasmus+ KA2 project [SEED2STEM: Planting the future of education](https://www.tvz.hr/introducing-seed2stem-planting-the-future-of-education/).

## How to Play

### Game Setup
1. **Power on** the device - "FARM GAME" logo appears
2. **Player 1** presses CONFIRM to start player selection (or hold BACK for settings)
3. **All players** press CONFIRM to join (2-4 players, 10-second window)
4. **Game begins** automatically when time expires or all slots filled

### Turn Structure
Each player's turn has three phases:

1. **Weather Phase**: Press CONFIRM to reveal weather → Check effects on your crops → Press CONFIRM to continue
2. **Field Management**: Navigate between fields (F1-F4) to harvest ready crops or plant new ones
3. **End Turn**: Navigate to "End Turn" and press CONFIRM twice (or hold FORWARD for quick skip)

### Controls
- **BACK/FORWARD**: Navigate left/right, decrease/increase values
- **CONFIRM**: Select, confirm actions
- **Hold FORWARD**: Quick jump to "End Turn" (1-second hold)

## Crops & Strategy

### Crop Information
| Crop | Turns to Grow | Simple Mode | Medium Mode | Advanced Mode |
|------|---------------|-------------|-------------|---------------|
| **Corn** 🌽 | 2 | 1 point | 2 points | 3 points |
| **Carrot** 🥕 | 3 | 4 points | 5 points | 6 points |
| **Tomato** 🍅 | 4 | 8 points | 9 points | 12 points |
| **Lettuce** 🥬 | 5 | 15 points | 16 points | 20 points |
| **Blueberry** 🫐 | 6 | 20 points | 25 points | 30 points |

### Game Modes

#### Simple Mode (Green) - Beginner Friendly
- **Weather effects**: Drought = no growth, Rain = 50% chance growth, Sun = normal growth
- **Strategy**: Focus on learning crop timing, weather is mostly unpredictable

#### Medium Mode (Yellow) - Balanced Challenge
- **Weather effects**: Drought = no growth, Rain = double growth, Sun = normal growth
- **Strategy**: Time plantings before rain for maximum benefit

#### Advanced Mode (Red) - Strategic Mastery
- **Crop-specific weather effects**: Each crop reacts differently to weather
  - **Drought**: Only corn survives (80% chance), others die
  - **Sun**: Corn & Tomato thrive (double growth), others normal
  - **Rain**: Carrot, Lettuce & Blueberry thrive (double growth), others normal
- **Stale crops**: Unharvested ready crops spoil and give only 25% points
- **Planting limit**: Maximum 2 crops per turn
- **Strategy**: Match crops to expected weather, harvest immediately when ready

### Weather System
Weather is drawn from a "bag" of 11 cards:
- **4 Sunny days** (36.4%)
- **3 Rainy days** (27.3%) 
- **4 Drought days** (36.4%)

When all cards are used, the bag refills and reshuffles. Players can track drawn weather to predict what's coming next!

## Project Status
- [x] Breadboard prototype
- [x] Schematic design
- [x] PCB design and manufacturing
- [x] Enclosure design and 3D printing
- [x] 3D CAD assembly and renders
- [x] Hardware assembly
- [x] Core game programming
- [x] **Game balancing completed** - All three difficulty modes fully implemented and balanced
- [x] **Weather system** - Sophisticated bag-based weather with crop-specific effects
- [x] **Advanced features** - Stale crops, planting limits, long-press shortcuts
- [x] **Audio/Visual polish** - Sound effects, animations, victory celebrations
- [x] **Documentation** - Complete rules, assembly instructions, cheat sheets
- [x] Final testing and optimization

## Current Game State
The game is **feature-complete and fully playable** with three balanced difficulty modes:
- **Simple Mode**: Perfect for beginners and casual play
- **Medium Mode**: Balanced challenge with predictable weather effects  
- **Advanced Mode**: Strategic depth with crop-weather matching, spoilage mechanics, and resource management

**Recent Major Updates:**
- Implemented crop-specific weather effects in Advanced mode
- Added stale crop mechanics (25% points penalty)
- Introduced per-turn planting limits (2 crops max in Advanced mode)
- Balanced weather bag system (4 Sun, 3 Rain, 4 Drought)
- Enhanced user interface with long-press shortcuts and improved text positioning
- Complete audio/visual feedback system

## Known Issues
- ~~Hardware is completely functional, software could use some extra work~~ ✅ **Resolved**
- ~~Due to ESP32 Neopixel library bug, the first WS2812B LED sometimes lights up green~~ ✅ **Fixed
- ~~Game balance needed adjustment~~ ✅ **Completed - all modes balanced**

**Current Status**: No major issues remaining. Game is stable and fully functional.

## Hardware & Display System

### Main Components
- **ESP32-S3 microcontroller** (Adafruit Feather ESP32-S3)
- **Central TFT display**: GC9A01 240x240 circular display showing crop images and game status
- **Player displays**: 4x SSD1306 128x64 OLED screens for individual player information
- **Status indicators**: WS2812B LED ring (16 LEDs, 4 per player) showing field status
- **Input system**: PCF8575 I/O expander with tactile buttons for each player
- **Audio feedback**: Buzzer for sound effects and music
- **Power management**: Optional MAX17048 battery fuel gauge
- **Connectivity**: Powered via USB-C

### Display System
- **Central TFT**: Shows active player (colored border), crop images, weather animations, victory screens
- **Individual OLEDs**: Personal scores, field information, menus, weather effects for each player
- **LED Ring**: Gray (empty field), colored (growing crop matching crop color), dark (player not active)
- **Audio**: Navigation beeps, weather sounds, harvest celebrations, victory fanfare

## 3D renders
![Full1](https://github.com/user-attachments/assets/fab94822-5d9b-4d8f-8514-ddaf97034429)
![Animation](https://github.com/user-attachments/assets/696d76b8-bb1a-48e9-8c86-f5cb82f0d0d6)

## Images
![IRL](https://github.com/user-attachments/assets/e725a37f-8255-4fd5-af57-f00499a43b76)

## PCB
There is a donut-shaped circuit board containing a microcontroller, all SMD and THD components.

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
