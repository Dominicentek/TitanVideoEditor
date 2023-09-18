# TitanVideoEditor
A cross platform video editing tool written in C++ designed to feel as close to Sony Vegas as possible 

## Dependencies
* SDL2
* OpenGL
* FFmpeg

## Build Instructions
### Windows
1. Install [MSYS2](https://github.com/msys2/msys2-installer/releases/download/2023-07-18/msys2-x86_64-20230718.exe)
2. Run the **MINGW** shell. You can find it in the start menu. **Do not run UCRT!**
3. Install dependencies<br><br>
   ```
   pacman -S git make pkg-config mingw-w64-x86_64-gcc mingw-w64-x86_64-glew mingw-w64-x86_64-SDL2
   ```
4. Clone the repository<br><br>
   ```
   git clone https://github.com/Dominicentek/TitanVideoEditor
   cd TitanVideoEditor
   ```
5. Compile<br><br>
   ```
   make
   ```
6. The executable should be located at `build/titan.exe`
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
