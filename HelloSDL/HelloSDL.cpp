// HelloSDL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <SDL.h>
#include <iostream>




int main(int argc, char *argv[])
{
	SDL_Window *window = nullptr;
	SDL_Surface *windowSurface = nullptr;
	SDL_Surface *imageSurface = nullptr;
	SDL_Surface *maskSurface = nullptr;

	SDL_Surface *bgSurface = nullptr;


	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "error with video init" << SDL_GetError() << std::endl;
	}
	else
	{
		window = SDL_CreateWindow("ello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			std::cout << "error with window init" << SDL_GetError() << std::endl;
		}
		else
		{
			windowSurface = SDL_GetWindowSurface(window);
			imageSurface = SDL_LoadBMP("viviartfull_smaller.bmp");

			if (imageSurface == NULL)
			{
				std::cout << "Couldnt load image: " << SDL_GetError() << std::endl;
			}
			else
			{
				maskSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);
				SDL_FillRect(maskSurface, NULL, SDL_MapRGB(maskSurface->format, 0, 0, 0));

				bgSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);// 0, 0, 0, 0);
				SDL_FillRect(bgSurface, NULL, SDL_MapRGB(bgSurface->format, 255, 255, 0));
				SDL_BlitSurface(bgSurface, NULL, windowSurface, NULL);

				SDL_SetSurfaceBlendMode(bgSurface, SDL_BLENDMODE_BLEND);

				SDL_BlitSurface(imageSurface, NULL, bgSurface, NULL);
				SDL_BlitSurface(bgSurface, NULL, windowSurface, NULL);
				SDL_UpdateWindowSurface(window);
				////SDL_Delay(5000);

				bool isRunning = true;
				SDL_Event ev;
				while (isRunning)
				{
					while(SDL_PollEvent(&ev) != 0)
					{
						if (ev.type == SDL_QUIT)
						{
							isRunning = false;
						}
					}
					SDL_UpdateWindowSurface(window);
				}
			}
		}
	}
	SDL_FreeSurface(imageSurface);
	SDL_FreeSurface(bgSurface);
	SDL_FreeSurface(maskSurface);
	imageSurface = nullptr;
	bgSurface = nullptr;
	maskSurface = nullptr;
	SDL_DestroyWindow(window);
	window = nullptr;
	windowSurface = nullptr;
	SDL_Quit();

	return 0;
}




//int main(int argc, char *argv[])
//{
//	SDL_Window *window = nullptr;
//	SDL_Surface *windowSurface = nullptr;
//	SDL_Surface *imageSurface = nullptr;
//	SDL_Surface *maskSurface = nullptr;
//
//	SDL_Surface *bgSurface = nullptr;
//
//	
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
//	{
//		std::cout << "error with video init" << SDL_GetError() << std::endl;
//	}
//	else
//	{
//		window = SDL_CreateWindow("ello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
//		if (window == NULL)
//		{
//			std::cout << "error with window init" << SDL_GetError() << std::endl;
//		}
//		else
//		{
//			windowSurface = SDL_GetWindowSurface(window);
//			imageSurface = SDL_LoadBMP("viviartfull_smaller.bmp");
//
//			if (imageSurface == NULL)
//			{
//				std::cout << "Couldnt load image: " << SDL_GetError() << std::endl;
//			}
//			else
//			{
//				/*SDL_Surface* rgbSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0, 0, 0, 0);
//				SDL_Rect rect;
//				rect.x = 0;
//				rect.y = 0;
//				rect.w = 640;
//				rect.h = 480;
//
//				SDL_Renderer *m_window_renderer = SDL_CreateRenderer(window, -1, 0);
//				SDL_SetRenderDrawColor(m_window_renderer, 255, 255, 255, 255);
//				//SDL_RenderDrawRect(m_window_renderer, &rect);
//				SDL_RenderFillRect(m_window_renderer, &rect);
//				SDL_SetRenderDrawColor(m_window_renderer, 0, 0, 0, 255);
//				SDL_RenderPresent(m_window_renderer);*/
//
//				//SDL_BlitSurface(imageSurface, NULL, rgbSurface, NULL);
//				//SDL_BlitSurface(rgbSurface, NULL, windowSurface, NULL);
//
//				maskSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);
//				SDL_FillRect(maskSurface, NULL, SDL_MapRGB(maskSurface->format, 0, 0, 0));
//
//				bgSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);// 0, 0, 0, 0);
//				SDL_FillRect(bgSurface, NULL, SDL_MapRGB(bgSurface->format, 255, 255, 0));
//				SDL_BlitSurface(bgSurface, NULL, windowSurface, NULL);
//
//				//SDL_SetSurfaceBlendMode(imageSurface, SDL_BLENDMODE_BLEND);
//				SDL_SetSurfaceBlendMode(bgSurface, SDL_BLENDMODE_BLEND);
//
//				SDL_BlitSurface(imageSurface, NULL, bgSurface, NULL);
//				SDL_BlitSurface(bgSurface, NULL, windowSurface, NULL);
//				SDL_UpdateWindowSurface(window);
//				SDL_Delay(5000);
//			}
//		}
//	}
//	SDL_FreeSurface(imageSurface);
//	SDL_FreeSurface(bgSurface);
//	SDL_FreeSurface(maskSurface);
//	imageSurface = nullptr;
//	bgSurface = nullptr;
//	maskSurface = nullptr;
//	SDL_DestroyWindow(window);
//	window = nullptr;
//	windowSurface = nullptr;
//	SDL_Quit();
//
//	return 0;
//}


//int main(int argc, char *argv[])
//{
//	SDL_Window *window = nullptr;
//
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
//	{
//		std::cout << "error with video init" << SDL_GetError() << std::endl;
//	}
//	else
//	{
//		window = SDL_CreateWindow("ello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
//		if (window == NULL)
//		{
//			std::cout << "error with window init" << SDL_GetError() << std::endl;
//		}
//		else
//		{
//			SDL_UpdateWindowSurface(window);
//			SDL_Delay(2000);
//		}
//	}
//	SDL_DestroyWindow(window);
//	SDL_Quit();
//
//	return 0;
//}

/* This was what we used for the "hello world"
int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Quit();

	return 0;
} */

//int main()
//{
//    std::cout << "Hello World!\n"; 
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
