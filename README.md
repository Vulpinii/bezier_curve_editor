<p align="center"><h1>Bezier Curve Editor</h1></p>

This program is a curve editor. It is based on Bézier curves calculated with the method of Bernstein polynomials or the recursive method of Casteljau. You can load 5 different examples and change the position of the control points or create your own curve.

## Features
- load different examples (as heart, moon, wave)
- set curve's precision (i.e. number of vertices)
- set the step of curve's construction
- hide control points
- show intermediate steps lines
- see and change directly vertices coordinates 

## Building
#### On Linux
**Prerequisite**: CMake

To build this program, download the source code using `` git clone`` or directly the zip archive.
Then run the `` launch.sh`` shell script.

You can do it manually by following these commands:
```shell script
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
./program
```

#### On Windows
[instructions coming soon]


## Gallery
#### YouTube Video
<a href="https://www.youtube.com/watch?v=zsa_Gt1qFe0
" target="_blank"><img src="https://github.com/Vulpinii/bezier_curve_editor/blob/master/images/screenshot_heart.png" 
alt="Bezier Curve Editor" width="100%" height="auto" border="10" /></a>
#### Preview
<p align="center"><img src="https://github.com/Vulpinii/bezier_curve_editor/blob/master/images/bezier_curve_editor.gif" alt="Animated gif" width="100%" /></p>
