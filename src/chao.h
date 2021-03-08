#ifndef CHAO_H
#define CHAO_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "array.h"

class Chao {

public:

	enum ScalingMode {
		SCALING_MODE_NONE = 0,
		SCALING_MODE_STRETCH,
		SCALING_MODE_KEEP_RATIO,
		SCALING_MODE_EXTEND,
		SCALING_MODE_END
	};

	struct Image {
		char* key = NULL;
		SDL_Surface* surface = NULL;
		int width;
		int heigth;
	};

	Chao(int width, int height, ScalingMode scalingMode);
	~Chao();

	void runGameLoop();
	void loadImage(char* key, char* imagePath);
	Image getImage(char* key);
	void drawImage(char* key, SDL_Surface* targetSurface);
	void drawImage(Image image, SDL_Surface* targetSurface);
	void log(char* message);


private:

	SDL_Window* sdlWindow = NULL;
	SDL_Surface* screenSurface = NULL;
	Array<Image> images;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

Chao::Chao(int width, int height, ScalingMode scalingMode) {
	log("Initializing chao!");
	SDL_Init(SDL_INIT_VIDEO);
	sdlWindow = SDL_CreateWindow("chao.exe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	screenSurface = SDL_GetWindowSurface(sdlWindow);
}

Chao::~Chao() {
	SDL_FreeSurface(images[0].surface);

	SDL_DestroyWindow(sdlWindow);
	sdlWindow = NULL;
	SDL_Quit();
}

void Chao::runGameLoop() {
	bool quit = false;
	SDL_Event e;

	// le game loop
	while(!quit) {

		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
			else if( e.type == SDL_KEYDOWN )
			{
				printf("\n%d", e.key.keysym.scancode);
			}


		}

		SDL_FillRect(screenSurface, NULL, 0xFFFFFF);

		drawImage("sticker", screenSurface);

		SDL_UpdateWindowSurface( sdlWindow );
	}
}

void Chao::loadImage(char* key, char* imagePath) {
	SDL_Surface* newSurface = IMG_Load(imagePath);

	Image newImage = { key, newSurface, 0, 0 };
	images.add(newImage);
}

Chao::Image Chao::getImage(char* key) {
	for (int i = 0; i < images.getSize(); ++i) {
		if (strcmp(key, images[i].key) == 0) {
			return images[i];
		}
	}
}

void Chao::drawImage(char* key, SDL_Surface* targetSurface) {
	drawImage(getImage(key), targetSurface);
}

void Chao::drawImage(Image image, SDL_Surface* targetSurface) {
	SDL_BlitSurface( image.surface, NULL, targetSurface, NULL );
}


void Chao::log(char* message) {
	printf("%s\n", message);
}

#endif // CHAO_H