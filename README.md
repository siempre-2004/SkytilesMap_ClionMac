# 🧱 SkyTiles — CLion Version

A real-time **tile rendering and camera control system** developed in **C++ (CLion)** using **Raylib**.  
This version demonstrates smooth camera switching, multi-map rendering, and frustum culling for optimized visualization.

---

## 🌍 Overview
This project simulates a **sky tile map system** with three switchable maps:

- 🟩 **Small map:** 32×32 tiles  
- 🟦 **Large map:** 1024×1024 tiles  
- 🟨 **Medium map:** 128×128 tiles  

Players can seamlessly switch between these maps and toggle multiple visualization options in real time.

---

## 🎮 Controls

| Key | Function |
|-----|-----------|
| **Tab** | Switch between the small and large maps |
| **R** | Switch to the 128×128 medium map |
| **F** | Toggle frustum culling (visual optimization) |
| **G** | Enable or disable map visual culling |
| **C** | Lock / unlock the camera within a defined boundary |
| **B** | Change background color |
| **Space** | Randomly redistribute tile positions |

The camera initially starts **off-center**, requiring player navigation to explore the map.  
After pressing **C**, pressing **Tab** still locks until **C** is pressed again.

---

## 🧩 Features
- Real-time **tile map rendering**  
- **Three different map scales** (32², 128², 1024²)  
- **Frustum culling** for performance optimization  
- **Camera locking and free movement**  
- **Dynamic map switching and reinitialization**  
- Built and tested in **CLion + Raylib**

---

## 🛠️ Build
```bash
g++ main.cpp -o skytile -lraylib -lm -lpthread
./skytile
