## SWORD II: ADVENTURE OF FROG
A small game made entirely on live stream over about 15 hours.
I intend to add more documentation and clarify some of the code and assets over the next few days.

### links
 - [windows build](https://github.com/NoelFB/tiny_link/releases/tag/v1.0.0)
 - [stream archive](https://twitch.tv/noelfb)
 
### building
  - You need C++17 and CMake 3.12+
  - Make sure you have SDL2 headers & libraries, and make sure CMake can find them
  - All the art was made in [Aseprite](https://www.aseprite.org/). To modify or add new sprites, you'll need to use aseprite.

### building on macOS Big Sur
  - preinstall latest Xcode and the command line tools
  - brew install cmake        // make sure CMake is 3.12+
  - brew install sdl2         // make sure sdl2 is 2.0.12+
  - git clone https://github.com/NoelFB/tiny_link.git 
  - cd tiny_link
  - git submodule init
  - git submodule update
  - cmake -G Xcode .
  - open Xcode project file, select 'game' target, build and run on Xcode

![Screenshot](https://github.com/noelfb/tiny_link/raw/main/screenshot.png "Screenshot")

### box art
![box art](https://github.com/noelfb/tiny_link/raw/main/boxart.jpg "Box art by Grayson")

*Box Art by [Grayson](https://twitter.com/soft_rumpus/status/1345934041527144459/photo/1)*

*Mosquito enemy sprite by [Randy](https://twitter.com/RandyPGaul))*

