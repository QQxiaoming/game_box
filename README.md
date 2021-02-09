# nes game simulator based on Qt

本项目基于Qt、InfoNES实现NES游戏模拟器,可在windous\mac\linux等多平台使用。

针对常见的InfoNES移植进行了一些问题修复：

- 图像颜色输出正确（部分其他项目存在图像输出颜色发红）
- 画面纹理输出正常不错位（部分其他项目在不同编译器下存在该问题，可以使用本项目中CatAndMouse.nes文件测试）

目前还存在的一些待解决的问题：

- 音频输出音质不高，且存在一些错误
