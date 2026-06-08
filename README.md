# SimpleFPSTemplate
Simple C++ FPS Template for Unreal Engine [More Info](https://www.tomlooman.com/fps-template/)

*Last updated for 5.2*

# Why create a "simplified" FPS C++ Template?
The provided C++ template that ships with the engine has several features that someone new to C++ may not be interested in like VR controls and Touch input. This code "bloats" the template to look a lot more complex than neccessary if you are simply looking to start with C++ in Unreal Engine. I'm using this project to create tutorials and you can use it in your own way!

![alt text](https://www.tomlooman.com/wp-content/uploads/2017/09/Thumb_FPSTemplate2.jpg)

This project is licensed under the Unreal Engine EULA.

# SimpleFPSTemplate - 我的 UE5 练习项目

基于开源 FPS 模板的修改，用于学习 UE5 C++ 游戏开发。

## 已实现的功能

- **霰弹枪散射系统**：支持自定义弹丸数量、散射角度，可在蓝图中实时调整参数
- **弹药与换弹系统**：弹夹/备弹管理、换弹计时、自动换弹触发
- **HUD 弹药显示**：实时显示当前弹药和换弹状态
- 修复多子弹同时生成时的碰撞冲突问题

## 控制方式

- 鼠标左键：开火
- R 键：换弹

## 技术栈

- Unreal Engine 5
- C++ / Enhanced Input / UMG
