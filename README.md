# Qt Image Processing Demo
### qt-simple-image-proc

<img width="1620" height="940" alt="image" src="https://github.com/user-attachments/assets/652e8272-e00d-4154-a0ee-2c3efd69d612" />

<p>A lightweight, modern image processing application built with Qt (Widgets + QML) that demonstrates real-time GPU-based effects and a clean editing workflow.</p>

### Overview
This project is a simplified image editor inspired by tools like ON1 Photo RAW. It focuses on:

<ul>
  <li>Real-time preview using GPU shaders </li>
  <li>Adjustable effect parameters </li>
  <li>Clean separation between UI, rendering, and processing logic</li>
</ul>

<p>The goal is to demonstrate a production-style architecture for image editing applications while remaining easy to understand and extend.</p>

### Features
<ul>
  <li>Real-Time Preview (GPU)</li>
  <li>QML-based rendering using QQuickWidget</li>
  <li>Fragment shader pipeline for effects</li>
  <li>Instant visual feedback when adjusting sliders</li>
  <li>Effect Controls</li>
  <li>Blur intensity</li>
  <li>Opacity control</li>
  <li>Enable/disable toggle</li>
  <li>Reset to defaults</li>
</ul>

### UI Structure
<ul>
  <li>Qt Widgets main window</li>
  <li>QML preview surface</li>
  <li>Panel-based controls</li>
  <li>Toolbar + menu actions</li>
</ul>

  
### Architecture

The app is split into clear layers:

<ol>
<li>1. UI Layer (Qt Widgets)
MainWindow
Menus, toolbar, sliders, buttons
Handles user input and state </li>
<li>2. Preview Layer (QML + GPU)
PreviewView.qml
Fragment + vertex shaders
Displays real-time processed image</li>
<li>3. Controller Layer
PreviewController
Bridges UI and rendering
Updates shader uniforms / parameters</li>
</ol>

### Rendering Pipeline
<ol>
<li>Image is loaded into the application</li>
<li>Passed to the QML preview</li>
<li>Fragment shader applies effects: Blur, Opacity blending</li>
<li>UI controls update shader uniforms in real time</li>
</ol>

### Key Concepts
```text
Texel vs Pixel
Pixel: screen-space unit
Texel: texture-space unit (used in shaders)
Texel Step
```

Used to sample neighboring pixels in shaders:

```
vec2 texelStep = 1.0 / textureSize;
Uniform Buffer
```

All UI-controlled parameters are passed into shaders via uniforms:
```
Blur radius
Opacity
Enable flag
```

This keeps GPU logic clean and efficient.

### Shader Bindings
```
binding = 0: typically input texture
binding = 1: uniform buffer
```

### Project Structure
```text
ImageDemo/
├── src/
│   ├── MainWindow.cpp / .h
│   ├── PreviewController.cpp / .h
│
├── qml/
│   ├── PreviewView.qml
│
├── shaders/
│   ├── effect.vert
│   ├── effect.frag
│
├── resources.qrc
├── CMakeLists.txt
```

