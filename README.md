[![Windows ci](https://img.shields.io/github/actions/workflow/status/qqxiaoming/game_box/windows.yml?branch=main&logo=data:image/svg+xml;base64,PHN2ZyByb2xlPSJpbWciIHZpZXdCb3g9IjAgMCAyNCAyNCIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj48dGl0bGU+V2luZG93czwvdGl0bGU+PHBhdGggZD0iTTAsMEgxMS4zNzdWMTEuMzcySDBaTTEyLjYyMywwSDI0VjExLjM3MkgxMi42MjNaTTAsMTIuNjIzSDExLjM3N1YyNEgwWm0xMi42MjMsMEgyNFYyNEgxMi42MjMiIGZpbGw9IiNmZmZmZmYiLz48L3N2Zz4=)](https://github.com/QQxiaoming/game_box/actions/workflows/windows.yml)
[![Linux ci](https://img.shields.io/github/actions/workflow/status/qqxiaoming/game_box/linux.yml?branch=main&logo=linux&logoColor=white)](https://github.com/QQxiaoming/game_box/actions/workflows/linux.yml)
[![Macos ci](https://img.shields.io/github/actions/workflow/status/qqxiaoming/game_box/macos.yml?branch=main&logo=apple)](https://github.com/QQxiaoming/game_box/actions/workflows/macos.yml)
[![CodeFactor](https://img.shields.io/codefactor/grade/github/qqxiaoming/game_box.svg?logo=codefactor)](https://www.codefactor.io/repository/github/qqxiaoming/game_box)
[![License](https://img.shields.io/github/license/qqxiaoming/game_box.svg?colorB=f48041&logo=gnu)](https://github.com/QQxiaoming/game_box)
[![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/QQxiaoming/game_box.svg?logo=git)](https://github.com/QQxiaoming/game_box/releases)
[![GitHub All Releases](https://img.shields.io/github/downloads/QQxiaoming/game_box/total.svg?logo=pinboard)](https://github.com/QQxiaoming/game_box/releases)
[![GitHub stars](https://img.shields.io/github/stars/QQxiaoming/game_box.svg?logo=github)](https://github.com/QQxiaoming/game_box)
[![GitHub forks](https://img.shields.io/github/forks/QQxiaoming/game_box.svg?logo=github)](https://github.com/QQxiaoming/game_box)
[![Gitee stars](https://gitee.com/QQxiaoming/game_box/badge/star.svg?theme=dark)](https://gitee.com/QQxiaoming/game_box)
[![Gitee forks](https://gitee.com/QQxiaoming/game_box/badge/fork.svg?theme=dark)](https://gitee.com/QQxiaoming/game_box)

# game box

English | [简体中文](./README_zh_CN.md)

GameBox is a retro console game emulator. This project only relies on the Qt framework and can be used on multiple platforms such as windows, macos, linux and even embedded linux. Since this project is developed based on a variety of open source codes, it is strongly reminded to pay attention to the license instructions.

## Platform support

- Implementation of NES game emulator based on InfoNES (basically completed, the advantage of InfoNES is less performance overhead, but currently it seems to have poor compatibility and poor audio processing, there are plans to consider refactoring and use fceux implementation)
- Implement MD game emulator based on dgen-sdl (completed)
- Implementation of GBA game simulator based on VBA-M (planned)

## Feature

- Mute (completed)
- Button configuration (in progress)
- Global mechanism for archive reading (planned)
- Screenshots (planned)

## nes

Basic porting is complete, with some bug fixes for common InfoNES porting:

- Image color output is correct (some other items exist image output color is red)
- The picture texture output is normal and good (some other projects have this problem under different compilers, you can use the SuperMario.nes file in this project to test)

There are still some issues to be resolved:

- The audio output is not high quality and has some bugs

## md

Based on the dgen-sdl-1.33 porting, the effect is good, and no problems have been found so far. The archiving function has not been transplanted yet. At present, it is considered to implement an overall archiving mechanism instead of implementing a specific platform archiving mechanism.

## gba

planned
