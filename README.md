# TitanVideoEditor
A cross platform video editing tool written in C++ designed to feel as close to Sony Vegas as possible 

## Dependencies
* SDL2
* OpenGL
* FFmpeg

## Build Instructions
### Windows
1. Download [MSYS2](https://www.msys2.org/)
2. Install it
3. Run the MSYS64 shell (or MSYS32 on 32-bit machines)
4. Install dependencies:<br><br>
   ```
   pacman -S make git mingw-w64-i686-gcc mingw-w64-x86_64-gcc mingw-w64-i686-glew mingw-w64-x86_64-glew mingw-w64-i686-SDL2 mingw-w64-i686-SDL mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL
   ```
5. Clone the repository<br><br>
   ```
   git clone https://github.com/Dominicentek/TitanVideoEditor
   cd TitanVideoEditor
   ```
6. Compile<br><br>
   ```
   make
   ```
7. When it finishes building, you can find the executable at `build/titan.exe`
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
