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
	void drawImage(char* key, SDL_Surface* targetSurface, float x, float y,
		float alpha = 1.0f, float scaleX = 1.0f, float scaleY = 1.0f, float angle = 0.0f,
		float rotationOffsetX = 0.5f, float rotationOffsetY = 0.5f);
	void drawImage(Image image, SDL_Surface* targetSurface, float x, float y,
		float alpha, float scaleX, float scaleY, float angle,
		float rotationOffsetX = 0.5f, float rotationOffsetY = 0.5f);
	
	void log(char* message);


private:

	const static int KEY_CODES_COUNT = 128;

	SDL_Window* sdlWindow = NULL;
	SDL_Surface* screenSurface = NULL;
	Array<Image> images;

	bool pressed[KEY_CODES_COUNT];
	bool justPressed[KEY_CODES_COUNT];
	bool justReleased[KEY_CODES_COUNT];

	void resetInput();
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

Chao::Chao(int width, int height, ScalingMode scalingMode) {
	log("Initializing chao!");
	SDL_Init(SDL_INIT_VIDEO);
	sdlWindow = SDL_CreateWindow("chao.exe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	screenSurface = SDL_GetWindowSurface(sdlWindow);

	resetInput();
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
			switch (e.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					printf("\n%d", e.key.keysym.scancode);
					break;
				case SDL_KEYUP:
					//
					break;
			}

		}

		SDL_FillRect(screenSurface, NULL, 0xFFFFFF);

		drawImage("sticker", screenSurface, 0, 0);

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

void Chao::drawImage(char* key, SDL_Surface* targetSurface, float x, float y, float alpha, float scaleX, float scaleY, float angle, float rotationOffsetX, float rotationOffsetY) {
	drawImage(getImage(key), targetSurface, x, y, alpha, scaleX, scaleY, angle, rotationOffsetX, rotationOffsetY);
}

void Chao::drawImage(Image image, SDL_Surface* targetSurface, float x, float y, float alpha, float scaleX, float scaleY, float angle, float rotationOffsetX, float rotationOffsetY) {
	SDL_BlitSurface(image.surface, NULL, targetSurface, NULL);
}

void Chao::log(char* message) {
	printf("%s\n", message);
}

void Chao::resetInput() {
	for (int i = 0; i < KEY_CODES_COUNT; ++i) {
		pressed[i] = false;
		justPressed[i] = false;
		justReleased[i] = false;
	}
}

#endif // CHAO_H