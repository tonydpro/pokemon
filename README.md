# pokemon

How to compile :

- editeur :
g++ main.c -o tileset -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w

g++ $(pkg-config --cflags gtk+-2.0) -c main.c -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w
g++ main.o $(pkg-config --libs gtk+-2.0) -o tileset -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w

- pokemon :
g++ main.c -o editeur -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w

- tileset :
g++ main.c -o tileset -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w

g++ $(pkg-config --cflags gtk+-2.0) -c main.c -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w
g++ main.o $(pkg-config --libs gtk+-2.0) -o tileset -O2 -lSDL -lSDLmain -lSDL_image -lSDL_mixer -lSDL_ttf -fpermissive -w

How to install SDL 1.2 on linux :

1) If you don't have yet the g++ compiler on your computer, enter this command in the terminal :

sudo apt install g++

2) This command will install the SDL 1.2 library :

sudo apt-get install libsdl1.2-dev

3) This command will install other libraries usefull for developpement with SDL 1.2 :

sudo apt-get install libsdl1.2debian libsdl-gfx1.2-5 libsdl-gfx1.2-dev libsdl-gfx1.2-doc libsdl-image1.2 libsdl-image1.2-dbg libsdl-image1.2-dev libsdl-mixer1.2 libsdl-mixer1.2-dbg libsdl-mixer1.2-dev libsdl-net1.2 libsdl-net1.2-dbg libsdl-net1.2-dev libsdl-sound1.2 libsdl-sound1.2-dev libsdl-ttf2.0-0 libsdl-ttf2.0-dev

4) If when you compile, you get an error message linked to SDL_mixer, go on the SDL_mixer 1.2 download page : https://www.libsdl.org/projects/SDL_mixer/release-1.2.html

Then, in "Binary / Linux", download the file SDL_mixer-devel-1.2.12-1.i386.rpm if your running a 32 bits OS or  SDL_mixer-devel-1.2.12-1.x86_64.rpm if your on a 64 bits OS

Then, open a new terminal and enter :

sudo pcmanfm

This will open a file browser with permissions to install the lib manually

Now, go to the file you've downloded and unpack the archive

Open the usr directory and go in lib

Copy SDL_mixer.a and libSDL_mixer.a to /usr/local/lib/

In the unpacked archive, go back to the usr directory, go in include

Copy SDL_mixer.h to usr/include/SDL/
