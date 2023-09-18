# TitanVideoEditor
A cross platform video editing tool written in C++ designed to feel as close to Sony Vegas as possible 

## Dependencies
* SDL2
* OpenGL
* FFmpeg

## Build Instructions
### Windows
* Currently no build instructions. Pull requests are welcome!
### Linux (Tested with Ubuntu 23.04)
1. Install dependencies
   * For Debian based:<br><br>
     ```
     sudo apt install git build-essential libglew-dev libsdl2-dev
     ```
   * For Arch based:<br><br>
     ```
     sudo pacman -S git make gcc glew sdl2
     ```
2. Clone the repository<br><br>
   ```
   git clone https://github.com/Dominicentek/TitanVideoEditor
   cd TitanVideoEditor
   ```
3. Compile<br><br>
   ```
   make
   ```
4. When it finishes building, you can find the executable at `build/titan`
### Mac OS X
* Currently no build instructions. Pull requests are welcome!
