////#pragma warning(disable:4996)



//special thanks:
//
//https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
//https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/
//https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
//https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html
//https://github.com/W-J-Dev-School/CHIP-8-Emulator/blob/master/src/chip8/cpu.rs
//https://blog.wjdevschool.com/blog/video-game-console-emulator/
//https://stackoverflow.com/questions/22129349/reading-binary-file-to-unsigned-char-array-and-write-it-to-another?noredirect=1&lq=1
//https://www.tutorialspoint.com/c_standard_library/c_function_fread.htm
//
//
//

#include "pch.h"
#include <SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <SDL_mixer.h>
#include<time.h>



struct SDL_ElementHolder 
{
	SDL_Window *window = nullptr;
	SDL_Surface *windowSurface = nullptr;
	SDL_Surface *imageSurface = nullptr;
	SDL_Surface *bufferSurface1 = nullptr;
	SDL_Surface *maskSurface = nullptr;

	SDL_Surface *bgSurface = nullptr;
	const unsigned char *sdlKeyState = nullptr;

	Mix_Music *bgm = nullptr;
	Mix_Chunk *sfx = nullptr;
};

int DeInitSdlForEmu(SDL_ElementHolder *sdlHolder);
int InitSdlForEmu(SDL_ElementHolder *sdlHolder);
int ClearEmuScreen(unsigned char screenGfx[]);
unsigned char SetPixel(int x, int y, unsigned char screenGfx[]);
int DrawEmuScreen(SDL_ElementHolder *sdlHolder, unsigned char screenGfx[]);
int LoadGameProgram(unsigned char mem[], const char path[]);
int LoadEmuOS(unsigned char mem[]);
int PrintSpriteFromMem(unsigned short spriteStart, unsigned short spriteHeight, unsigned char mem[]);
int PrintState(unsigned short opCode, unsigned char vreg[], unsigned short iReg, unsigned short pc);
char GetHexCodeChar(unsigned char code);


const int EMUSCREENWIDTH = 64;
const int EMUSCREENHEIGHT = 32;


int main(int argc, char *argv[])
{
	SDL_ElementHolder sdlHolder;

	unsigned short currentOpCode;

	unsigned char mem[4096];
	unsigned char vReg[16];
	unsigned short iReg = 0;
	unsigned short PROGRAM_COUNTER = 0x200;
	unsigned char screenGfx[EMUSCREENWIDTH * EMUSCREENHEIGHT];// [64 * 32];
	unsigned char delayTimer = 0;
	unsigned char soundTimer = 0;
	unsigned short stack[16];
	unsigned short stackIndex = 0;
	unsigned char KeypadState[16];

	for (int i = 0; i <= 15; i++)
	{
		vReg[i] = 0;
		KeypadState[i] = 0;
	}
	for (int i = 0; i < EMUSCREENWIDTH * EMUSCREENHEIGHT; i++)
	{
		screenGfx[i] = 0;
	}
	for (int i = 0; i < 4096; i++)
	{
		mem[i] = 0;
	}

	srand(time(0));

	unsigned char tempByte1;
	unsigned char tempByte2;
	unsigned char tempByte3;
	int spriteWidth = 8;

	long processorSpeed = 12;// 100;

	bool drawFlag = false;

	if (InitSdlForEmu(&sdlHolder) == 0)
	{
		LoadEmuOS(mem);
		LoadGameProgram(mem, "BLINKY"); //"MERLIN");// "BLINKY"); // Space Invaders [David Winter].ch8 // Maze (alt) [David Winter, 199x].ch8

		//set chip8 sound
		//using for simple demo purposes https://freesound.org/people/braqoon/sounds/161098/
		//thank you braqoon, esp for the CC0 license
		sdlHolder.sfx = Mix_LoadWAV("161098__braqoon__arrow-damage_CC.wav");

		bool isRunning = true;
		SDL_Event ev;
		////ClearEmuScreen(screenGfx);
		while (isRunning)
		{
			//todo: try to set this at 60 fps

			for (int xc = 0; xc < processorSpeed; xc++)
			{
				currentOpCode = mem[PROGRAM_COUNTER] << 8 | mem[PROGRAM_COUNTER + 1];

				unsigned char x = (currentOpCode & 0x0F00) >> 8;
				unsigned char y = (currentOpCode & 0x00F0) >> 4;
				long regsum;

				//std::cout << (currentOpCode & 0xF000) << "   "  << currentOpCode << std::endl;
				if (false)
				{
					PrintState(currentOpCode, vReg, iReg, PROGRAM_COUNTER);
				}

				//opcode info taken from wikipedia https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
				switch (currentOpCode & 0xF000)
				{
				case 0x1000://1NNN 	Flow 	goto NNN; 	Jumps to address NNN.
					PROGRAM_COUNTER = currentOpCode & 0x0FFF;
					////????PROGRAM_COUNTER += 2;
					break;
				case 0x2000://2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN. 
					stack[stackIndex] = PROGRAM_COUNTER + 2;////???? on the +2
					stackIndex += 1;
					PROGRAM_COUNTER = currentOpCode & 0x0FFF;
					////????PROGRAM_COUNTER += 2;
					break;

				case 0x3000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					if (vReg[x] == (unsigned char)(currentOpCode & 0x00FF))
					{
						PROGRAM_COUNTER += 2;
					}
					PROGRAM_COUNTER += 2;
					break;

				case 0x4000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					if (vReg[x] != (unsigned char)(currentOpCode & 0x00FF))
					{
						PROGRAM_COUNTER += 2;
					}
					PROGRAM_COUNTER += 2;
					break;

				case 0x5000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					if (vReg[x] == vReg[y])
					{
						PROGRAM_COUNTER += 2;
					}
					PROGRAM_COUNTER += 2;
					break;

				case 0x6000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					vReg[x] = (currentOpCode & 0x00FF);
					PROGRAM_COUNTER += 2;
					break;

				case 0x7000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					vReg[x] += (currentOpCode & 0x00FF);
					PROGRAM_COUNTER += 2;
					break;

				case 0xA000:
					iReg = (currentOpCode & 0x0FFF);
					PROGRAM_COUNTER += 2;
					break;

				case 0xB000:
					PROGRAM_COUNTER = (currentOpCode & 0x0FFF) + vReg[0];
					////????PROGRAM_COUNTER += 2;
					break;

				case 0xC000:
					vReg[x] = (rand() & (0x00FF & currentOpCode));
					PROGRAM_COUNTER += 2;
					break;

				case 0xD000:
					{
						//sprite height
						unsigned char spriteHeight = currentOpCode & 0x000F;

						std::cout << std::endl;
						std::cout << "x: " << (int)vReg[x] << "y:" << (int)vReg[y] << std::endl;
						std::cout << std::endl;
						PrintSpriteFromMem(iReg, spriteHeight, mem);
						vReg[0x000F] = 0;
						for (unsigned short hrow = 0; hrow < spriteHeight; hrow++)
						{
							unsigned char spriteRow = mem[iReg + hrow];
							for (unsigned short b = 0; b < spriteWidth; b++)
							{
								if (((spriteRow << b) & 0x80) > 0)
								{
									if (SetPixel(vReg[x] + b, vReg[y] + hrow, screenGfx))
									{
										vReg[15] = 1;
									}
								}
							}
						}
						drawFlag = true;
						PROGRAM_COUNTER += 2;
						break;
					}
					break;

				case 0xE000:
					switch(currentOpCode & 0x00FF)
					{
					case 0x009E:
						if (KeypadState[vReg[x]])
						{
							PROGRAM_COUNTER += 2;
						}
						PROGRAM_COUNTER += 2;
						break;
					case 0x00A1:
						if (!KeypadState[vReg[x]])
						{
							PROGRAM_COUNTER += 2;
						}
						PROGRAM_COUNTER += 2;
						break;

					default:
						std::cout << "do not have opcode" << currentOpCode << std::endl;
						break;
					}
					break;

				case 0xF000:
					switch (currentOpCode & 0x00FF)
					{
					case 0x0007:
						vReg[x] = delayTimer;
						PROGRAM_COUNTER += 2;
						break;
					case 0x000A:
						if (false)//wait for keypress
						{
							PROGRAM_COUNTER += 2;
						}
						break;
					case 0x0015:
						delayTimer = vReg[x];
						PROGRAM_COUNTER += 2;
						break;
					case 0x0018:
						soundTimer = vReg[x];
						PROGRAM_COUNTER += 2;
						break;
					case 0x001E:
						iReg = iReg + vReg[x];
						PROGRAM_COUNTER += 2;
						break;
					case 0x0029:
						iReg = vReg[x] * 5;
						PROGRAM_COUNTER += 2;
						break;
					case 0x0033:
						mem[iReg] = (vReg[x] / 100);
						mem[iReg + 1] = (vReg[x] % 100) / 10;
						mem[iReg + 2] = vReg[x] % 10;
						PROGRAM_COUNTER += 2;
						break;
					case 0x0055:
						for (unsigned short c = 0; c <= x; c++)
						{
							//mem[iReg] = vReg[c];
							mem[iReg + c] = vReg[c];
							//iReg += 1;
						}
						PROGRAM_COUNTER += 2;
						break;
					case 0x0065:
						for (unsigned short c = 0; c <= x; c++)
						{
							//vReg[c] = mem[iReg];
							vReg[c] = mem[iReg + c];
							//iReg += 1;
						}
						PROGRAM_COUNTER += 2;
						break;

					default:
						std::cout << "do not have opcode" << currentOpCode << std::endl;
						break;
					}
					break;

				case 0x8000:
					switch (currentOpCode & 0x000F)
					{
					case 0x0000://8XY0 	Assig 	Vx = Vy 	Sets VX to the value of VY. 
						vReg[x] = vReg[y];
						PROGRAM_COUNTER += 2;
						break;
					case 0x0001:
						vReg[x] = (vReg[x] | vReg[y]);
						PROGRAM_COUNTER += 2;
						break;
					case 0x0002:
						vReg[x] = (vReg[x] & vReg[y]);
						PROGRAM_COUNTER += 2;
						break;
					case 0x0003:
						vReg[x] = (vReg[x] ^ vReg[y]);
						PROGRAM_COUNTER += 2;
						break;
					case 0x0004:
						regsum = (vReg[x] + vReg[y]);
						if (regsum > 0x00FF)
						{
							vReg[0x000F] = 1;
						}
						else
						{
							vReg[0x000F] = 0;
						}
						vReg[x] = vReg[x] + vReg[y];
						PROGRAM_COUNTER += 2;
						break;
					case 0x0005:
						if (vReg[y] > vReg[x])
						{
							vReg[0x000F] = 0;
						}
						else
						{
							vReg[0x000F] = 1;
						}
						vReg[x] = vReg[x] - vReg[y];
						PROGRAM_COUNTER += 2;
						break;
					case 0x0006://8XY6[a] 	BitOp 	Vx >>= 1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1. VC = V[15] aka v[0xf]
						vReg[15] = (vReg[x] & 0x0001);////???? 0x00F;
						vReg[x] = (vReg[x] >> 1);
						PROGRAM_COUNTER += 2;
						break;
					case 0x0007:
						if (vReg[x] > vReg[y])
						{
							vReg[0x000F] = 0;
						}
						else
						{
							vReg[0x000F] = 1;
						}
						vReg[x] = vReg[y] - vReg[x];
						PROGRAM_COUNTER += 2;
						break;
					case 0x000E://8XY6[a] 	BitOp 	Vx >>= 1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1. VC = V[15] aka v[0xf]
						vReg[15] = (vReg[x] & 0x80);
						vReg[x] = vReg[x] << 1;
						PROGRAM_COUNTER += 2;
						break;

					default:
						std::cout << "do not have opcode" << currentOpCode << std::endl;
						break;
					}
					break;

				case 0x9000://3XNN 	Cond 	if (Vx == NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block); 
					if (vReg[x] != vReg[y])
					{
						PROGRAM_COUNTER += 2;
					}
					PROGRAM_COUNTER += 2;
					break;

				case 0x0000:
					switch (currentOpCode & 0x000F)
					{
					case 0x0000:
						if (currentOpCode == 0x00E0)
						{
							ClearEmuScreen(screenGfx);
							PROGRAM_COUNTER += 2;
						}
						break;
					case 0x000E:
						if (currentOpCode == 0x00EE)
						{
							//PROGRAM_COUNTER = stack[stackIndex];
							stackIndex -= 1;
							PROGRAM_COUNTER = stack[stackIndex];
							////????PROGRAM_COUNTER += 2;
						}
						break;

					default:
						std::cout << "do not have opcode" << currentOpCode << std::endl;
						break;
					}

					break;

				default:
					std::cout << "do not have opcode" << currentOpCode << std::endl;
					break;
				}

			}

			if (delayTimer > 0)
			{
				delayTimer -= 1;
			}
			if (soundTimer > 0)
			{
				soundTimer -= 1;
				if (!Mix_Playing(0))
				{
					Mix_PlayChannel(0, sdlHolder.sfx, 0);
				}
				//Mix_HaltChannel(0);
			}
			else {
				if (Mix_Playing(0))
				{
					////Mix_HaltChannel(0);
				}
			}

			//
			//todo: handle sound
			//if soundTimer greater than 0, play, else stop/dont play 
			//


			if (drawFlag == true)
			{
				DrawEmuScreen(&sdlHolder, screenGfx);
				drawFlag = false;
			}
			else
			{
				//iReg += 1;
			}

			/*if (currentOpCode >> 12 == 0x0A) {
				iReg = currentOpCode & 0x0FFF;
				PROGRAM_COUNTER += 2;
			}*/

			/*SetPixel(1, 1, screenGfx);
			SetPixel(2, 2, screenGfx);
			DrawEmuScreen(&sdlHolder, screenGfx);*/

			SDL_UpdateWindowSurface(sdlHolder.window);


			while (SDL_PollEvent(&ev) != 0)
			{
				if (ev.type == SDL_QUIT)
				{
					isRunning = false;
				}
				else if(ev.type == SDL_KEYDOWN)
				{
					//keypresses
				}
				//else if (ev.type == SDL_KEYUP)
				//{
				//	//keypresses
				//	switch (ev.key.keysym.sym)
				//	{
				//	case SDLK_q:
				//		break;
				//		default:
				//			break;
				//	}
				//}
			}

			sdlHolder.sdlKeyState = SDL_GetKeyboardState(NULL);
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_1]) { KeypadState[0] = 0x1; } else { KeypadState[0] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_2]) { KeypadState[1] = 0x1; } else { KeypadState[1] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_3]) { KeypadState[2] = 0x1; } else { KeypadState[2] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_4]) { KeypadState[3] = 0x1; } else { KeypadState[3] = 0x0; }

			if (sdlHolder.sdlKeyState[SDL_SCANCODE_Q]) { KeypadState[4] = 0x1; } else { KeypadState[4] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_W]) { KeypadState[5] = 0x1; } else { KeypadState[5] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_E]) { KeypadState[6] = 0x1; } else { KeypadState[6] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_R]) { KeypadState[7] = 0x1; } else { KeypadState[7] = 0x0; }

			if (sdlHolder.sdlKeyState[SDL_SCANCODE_A]) { KeypadState[8] = 0x1; } else { KeypadState[8] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_S]) { KeypadState[9] = 0x1; } else { KeypadState[9] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_D]) { KeypadState[10] = 0x1; } else { KeypadState[10] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_F]) { KeypadState[11] = 0x1; } else { KeypadState[11] = 0x0; }

			if (sdlHolder.sdlKeyState[SDL_SCANCODE_Z]) { KeypadState[12] = 0x1; } else { KeypadState[12] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_X]) { KeypadState[13] = 0x1; } else { KeypadState[13] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_C]) { KeypadState[14] = 0x1; } else { KeypadState[14] = 0x0; }
			if (sdlHolder.sdlKeyState[SDL_SCANCODE_V]) { KeypadState[15] = 0x1; } else { KeypadState[15] = 0x0; }

			std::this_thread::sleep_for(std::chrono::microseconds(1666));
		}
	}
	
	DeInitSdlForEmu(&sdlHolder);

	return 0;
}

int LoadGameProgram(unsigned char mem[], const char path[])
{
	//thank you for the basis https://stackoverflow.com/questions/22129349/reading-binary-file-to-unsigned-char-array-and-write-it-to-another?noredirect=1&lq=1
	FILE * file;
	unsigned char temparray[4096];
	fopen_s(&file, path, "r+");//"input.txt", "r+");////
	if (file == NULL) return 2;
	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	fclose(file);
	// Reading data to array of unsigned chars

	//and thank you for telling me the rb option https://stackoverflow.com/questions/42183768/how-to-read-a-binary-file-with-fread
	fopen_s(&file, path, "rb");
	int bytes_read = fread(temparray, sizeof(unsigned char), size, file);
	fclose(file);
	for (unsigned short i = 0; i < (unsigned short)size; i++)
	{
		mem[512 + i] = temparray[i];
	}


	return 0;
}

unsigned char SetPixel(int x, int y, unsigned char screenGfx[])
{
	if (x > EMUSCREENWIDTH) {
		x = x % EMUSCREENWIDTH;
	}
	else if (x < 0) {
		unsigned long tempx = x + (x * EMUSCREENWIDTH * -1);
		x = tempx % EMUSCREENWIDTH;
	}

	if (y > EMUSCREENHEIGHT) {
		y = y % EMUSCREENHEIGHT;
	}
	else if (x < 0) {
		unsigned long tempy = y + (y * EMUSCREENHEIGHT * -1);
		y = tempy % EMUSCREENHEIGHT;
	}

	int pixelIndex = x + (y * EMUSCREENWIDTH);
	screenGfx[pixelIndex] ^= 1;

	return !screenGfx[pixelIndex];
}

int CheckOffAllUsed(unsigned short opcode, bool usedOpCodes[])
{
	if (opcode & 0xF000 == 0x000)
	{

	}

	return 0;
}

int PrintState(unsigned short opCode, unsigned char vreg[], unsigned short iReg, unsigned short pc)
{
	char hexcode[5];
	hexcode[4] = '/0';
	hexcode[3] = GetHexCodeChar(opCode % 16);
	hexcode[2] = GetHexCodeChar((opCode% 256) / 16);
	hexcode[1] = GetHexCodeChar((opCode % 4096) / 256);
	hexcode[0] = GetHexCodeChar(opCode / 4096);
	std::cout << std::endl;
	std::cout << "0x" << hexcode[0] << hexcode[1] << hexcode[2] << hexcode[3] << std::endl;
	std::cout << "program counter: " << pc << std::endl;
	std::cout << "iReg: " << iReg << std::endl;

	std::cout << "vReg0: " << (int)vreg[0] << std::endl;
	std::cout << "vReg1: " << (int)vreg[1] << std::endl;
	std::cout << "vReg2: " << (int)vreg[2] << std::endl;
	std::cout << "vReg3: " << (int)vreg[3] << std::endl;
	std::cout << "vReg4: " << (int)vreg[4] << std::endl;
	std::cout << "vReg5: " << (int)vreg[5] << std::endl;
	std::cout << "vReg6: " << (int)vreg[6] << std::endl;
	std::cout << "vReg7: " << (int)vreg[7] << std::endl;
	std::cout << "vReg8: " << (int)vreg[8] << std::endl;
	std::cout << "vReg9: " << (int)vreg[9] << std::endl;
	std::cout << "vReg10: " << (int)vreg[10] << std::endl;
	std::cout << "vReg11: " << (int)vreg[11] << std::endl;
	std::cout << "vReg12: " << (int)vreg[12] << std::endl;
	std::cout << "vReg13: " << (int)vreg[13] << std::endl;
	std::cout << "vReg14: " << (int)vreg[14] << std::endl;
	std::cout << "vReg15: " << (int)vreg[15] << std::endl;

	return 0;
}

char GetHexCodeChar(unsigned char code)
{
	switch (code)
	{
		case 0: return '0';
		case 1: return '1';
		case 2: return '2';
		case 3: return '3';
		case 4: return '4';
		case 5: return '5';
		case 6: return '6';
		case 7: return '7';
		case 8: return '8';
		case 9: return '9';
		case 10: return 'A';
		case 11: return 'B';
		case 12: return 'C';
		case 13: return 'D';
		case 14: return 'E';
		case 15: return 'F';
		default: return '?';
	}
}


int PrintSpriteFromMem(unsigned short spriteStart, unsigned short spriteHeight, unsigned char mem[])
{
	//unsigned char spriteHeight = currentOpCode & 0x000F;
	char imageString[9];
	std::cout << std::endl;

	for (unsigned char hrow = 0; hrow < spriteHeight; hrow++)
	{
		unsigned char spriteRow = mem[spriteStart + hrow];
		for (unsigned char b = 0; b < 8; b++)//spriteWidth
		{
			if (((spriteRow << b) & 0x80) > 0)
			{
				imageString[b] = 'X';				
			}
			else
			{
				imageString[b] = ' ';
			}
		}
		imageString[8] = '/0';
		std::cout << imageString[0] << imageString[1] << imageString[2] << imageString[3] << imageString[4] << imageString[5] << imageString[6] << imageString[7] << std::endl;
	}
	std::cout << std::endl;

	return 0;
}

int ClearEmuScreen(unsigned char screenGfx[])
{
	for (int i = 0; i < EMUSCREENWIDTH * EMUSCREENHEIGHT; i++)
	{
		screenGfx[i] = 0;
	}
	return 0;
}

int DrawEmuScreen(SDL_ElementHolder *sdlHolder, unsigned char screenGfx[])
{
	const int pixelWidth = 10;

	(*sdlHolder).bgSurface = SDL_CreateRGBSurface(0, 640, 320, 32, 0xff, 0xff00, 0xff0000, 0x00000000);
	SDL_FillRect((*sdlHolder).bgSurface, NULL, SDL_MapRGB((*sdlHolder).bgSurface->format, 0, 0, 0));
	SDL_BlitSurface((*sdlHolder).bgSurface, NULL, (*sdlHolder).windowSurface, NULL);

	(*sdlHolder).bufferSurface1 = SDL_CreateRGBSurface(0, pixelWidth, pixelWidth, 32, 0xff, 0xff00, 0xff0000, 0x00000000);
	SDL_FillRect((*sdlHolder).bufferSurface1, NULL, SDL_MapRGB((*sdlHolder).bufferSurface1->format, 255, 255, 255));

	SDL_Rect pixelRect;

	pixelRect.x = 0;
	pixelRect.y = 0;
	pixelRect.w = 10;
	pixelRect.h = 10;
	for (int i = 0; i < EMUSCREENWIDTH * EMUSCREENHEIGHT; i++)
	{
		if (screenGfx[i] > 0)
		{
			pixelRect.x = (i % EMUSCREENWIDTH) * pixelWidth;
			pixelRect.y = (i / EMUSCREENWIDTH) * pixelWidth;
			SDL_BlitSurface((*sdlHolder).bufferSurface1, NULL, (*sdlHolder).windowSurface, &pixelRect);
		}
		//screenGfx[i] = 0;
	}

	return 0;
}


int LoadEmuOS(unsigned char mem[])
{
	//stolen from https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
	//thank you
	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	for (int i = 0; i < 80; i++)
	{
		mem[i] = chip8_fontset[i];
	}

	return 0;
}

int DeInitSdlForEmu(SDL_ElementHolder *sdlHolder)
{
	SDL_FreeSurface((*sdlHolder).bufferSurface1);
	SDL_FreeSurface((*sdlHolder).imageSurface);
	SDL_FreeSurface((*sdlHolder).bgSurface);
	SDL_FreeSurface((*sdlHolder).maskSurface);
	(*sdlHolder).bufferSurface1 = nullptr;
	(*sdlHolder).imageSurface = nullptr;
	(*sdlHolder).bgSurface = nullptr;
	(*sdlHolder).maskSurface = nullptr;
	SDL_DestroyWindow((*sdlHolder).window);
	(*sdlHolder).window = nullptr;
	(*sdlHolder).windowSurface = nullptr;
	Mix_FreeChunk((*sdlHolder).sfx);
	Mix_FreeMusic((*sdlHolder).bgm);
	(*sdlHolder).sfx = nullptr;
	(*sdlHolder).bgm = nullptr;
	Mix_Quit();
	SDL_Quit();

	return 0;
}

int InitSdlForEmu(SDL_ElementHolder *sdlHolder)
{
	int errorReturned = 1;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		std::cout << "error with video or audio init" << SDL_GetError() << std::endl;
	}
	else
	{
		(*sdlHolder).window = SDL_CreateWindow("ello SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
		if ((*sdlHolder).window == NULL)
		{
			std::cout << "error with window init" << SDL_GetError() << std::endl;
		}
		else
		{
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				std::cout << "SDL Mixer error: " << Mix_GetError() << std::endl;
			}

			(*sdlHolder).windowSurface = SDL_GetWindowSurface((*sdlHolder).window);
			errorReturned = 0;			
		}
	}

	return errorReturned;
}


//
//int main(int argc, char *argv[])
//{
//	SDL_ElementHolder sdlHolder;
//
//	if (InitSdlForEmu(&sdlHolder) == 0)
//	{
//		sdlHolder.windowSurface = SDL_GetWindowSurface(sdlHolder.window);
//		sdlHolder.imageSurface = SDL_LoadBMP("viviartfull_smaller.bmp");
//
//		sdlHolder.maskSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);
//		SDL_FillRect(sdlHolder.maskSurface, NULL, SDL_MapRGB(sdlHolder.maskSurface->format, 0, 0, 0));
//
//		sdlHolder.bgSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xff, 0xff00, 0xff0000, 0x00000000);// 0, 0, 0, 0);
//		SDL_FillRect(sdlHolder.bgSurface, NULL, SDL_MapRGB(sdlHolder.bgSurface->format, 255, 255, 0));
//		SDL_BlitSurface(sdlHolder.bgSurface, NULL, sdlHolder.windowSurface, NULL);
//
//		SDL_SetSurfaceBlendMode(sdlHolder.bgSurface, SDL_BLENDMODE_BLEND);
//
//		SDL_BlitSurface(sdlHolder.imageSurface, NULL, sdlHolder.bgSurface, NULL);
//		SDL_BlitSurface(sdlHolder.bgSurface, NULL, sdlHolder.windowSurface, NULL);
//		SDL_UpdateWindowSurface(sdlHolder.window);
//
//		bool isRunning = true;
//		SDL_Event ev;
//		while (isRunning)
//		{
//			while (SDL_PollEvent(&ev) != 0)
//			{
//				if (ev.type == SDL_QUIT)
//				{
//					isRunning = false;
//				}
//			}
//			SDL_UpdateWindowSurface(sdlHolder.window);
//		}
//	}
//
//	DeInitSdlForEmu(&sdlHolder);
//
//	return 0;
//}