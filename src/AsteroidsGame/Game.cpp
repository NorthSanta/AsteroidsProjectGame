#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <conio.h>
#include <SDL.h>
#include <SDL_image.h>
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include <vector>
#include <time.h>
#include <SDL_ttf.h>
#include <list>

using namespace std;
using namespace rapidxml;

int counter = 0;

const int fps = 30;
const int WIDTH = 720, HEIGHT = 576;
string lives;
string speed;
string enemies;
string pluse;
//asteroids vectors
vector<SDL_Rect> bigAs;
vector<SDL_Rect> nomAs;
vector<SDL_Rect> smlAs;
//bullets vector
vector<SDL_Rect> bullets;

//life vector
vector<SDL_Rect> cors;




int iLives;
float iSpeed;
int iEnemies;
int iPlus;

bool alive = true;
int x;
int y;
int lvl = 0;

int score = 0;

struct PlayerData
{
	string name;
	int score;
};





bool compFirst(const PlayerData &a, const PlayerData &b) {
	return a.score > b.score;
}
void die() {
	
	TTF_Init();
	cout << "Enter Your Name" << endl;
	string name;
	cin >> name;
	//read & update ranking
	list<PlayerData> ranking;
	string textureText[10];
	std::ifstream loadfile("Ranking.bin", std::ios::binary);
	if (loadfile.good()) {
		PlayerData playerData[10];
		for (int i = 0; i < 10; i++) {
			int aux = loadfile.tellg();
			if (i != 0) aux += 1;
			loadfile.seekg(aux);
			std::getline(loadfile, playerData[i].name, '\0'); // Get player name (only if null ternimated in binary)
			loadfile.read(reinterpret_cast<char*>(&playerData[i].score), sizeof(playerData[i].score)); // Read int bytes
			if (playerData[i].name == "") playerData[i].score = 0;
			else {
				ranking.emplace_front(playerData[i]);
			}
		}
		loadfile.close();
	}
	loadfile.close();
	//acctualizar lista
	PlayerData playerData = { name, score };
	
	ranking.emplace_front(playerData);
	
	ranking.sort(compFirst);
	cout << "sorted List" << endl;
	for (list<PlayerData>::iterator it = ranking.begin(); it != ranking.end(); ++it)
	{
		if (it->score > 0) {
			cout << it->name << ":" << it->score << endl;
		}
	}
	std::ofstream savefile("Ranking.bin", std::ios::binary);
	int i = 0;
	if (savefile.good()) {
		for (list<PlayerData>::iterator it = ranking.begin(); it != ranking.end(); ++it)
		{	
			if (it->name != "" && score > 0 && i <= 10) {
				savefile.write(it->name.c_str(), it->name.size()); // Write string to binary file
				savefile.write("\0", sizeof(char));
				savefile.write(reinterpret_cast<char*>(&it->score), sizeof(it->score));
				savefile.write("\0", sizeof(char));// Write int to binary file
			}
			i++;
		}
		savefile.close();
	}
	
	
	SDL_Window* gameOver = SDL_CreateWindow("GameOver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer3 = SDL_CreateRenderer(gameOver, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Color color = { 255,255,255 };
	TTF_Font* gFont = TTF_OpenFont("../../res/fnt/Hyperspace.otf", 20);
	SDL_Surface* textSurface[10];
	SDL_Texture* mTexture[10];
	SDL_Rect textRect[10];
	int j = 0;
	
		for (list<PlayerData>::iterator it = ranking.begin(); it != ranking.end(); ++it)
		{
			if (j < 10) {
				
				textureText[j] = to_string(j+1) + "  " + it->name.c_str() + "  " + to_string(it->score);
				textSurface[j] = TTF_RenderText_Solid(gFont, textureText[j].c_str(), color);
				mTexture[j] = SDL_CreateTextureFromSurface(renderer3, textSurface[j]);
				if(j == 0)
				textRect[j] = { 35,WIDTH/2-75, textSurface[j]->w,textSurface[j]->h };
				else {
					textRect[j] = { 35  ,textRect[j - 1].y + textSurface[j]->h, textSurface[j]->w,textSurface[j]->h };
				}
			}
			j++;
		}
		
		for (int i = 0; i < 10; i++) {
			if (textureText[i] == "") {
				textureText[i] = to_string(i + 1) + "  " + "      " + "  " + to_string(0);
				textSurface[i] = TTF_RenderText_Solid(gFont, textureText[i].c_str(), color);
				mTexture[i] = SDL_CreateTextureFromSurface(renderer3, textSurface[i]);
				if (i == 0)
					textRect[i] = { 35,WIDTH/2-75, textSurface[i]->w,textSurface[i]->h };
				else {
					textRect[i] = { 35  ,textRect[i - 1].y + textSurface[i]->h, textSurface[i]->w,textSurface[i]->h };
				}
			}
		}
		ranking.clear();

		
	SDL_Surface* rankingSurface = TTF_RenderText_Solid(gFont, "RANKING", color);
	SDL_Texture* rankingTexture = SDL_CreateTextureFromSurface(renderer3, rankingSurface);
	SDL_Rect rankingRect = { 75,WIDTH / 2 - 100,rankingSurface->w,rankingSurface->h };

	
	

	SDL_Texture *bgTexture = IMG_LoadTexture(renderer3, "../../res/gfx/blacks.png");
	SDL_Rect bgRect = { 0, 0, WIDTH, HEIGHT };

	SDL_Texture *gameOverText = IMG_LoadTexture(renderer3, "../../res/gfx/gameover.png");
	SDL_Rect gameOverRect = { (WIDTH / 2) - 200,50,400,200 };

	SDL_Texture *replayText = IMG_LoadTexture(renderer3, "../../res/gfx/playagain.png");
	SDL_Rect replayRect = { WIDTH / 2 - 75 , HEIGHT / 2 , 150, 100 };

	SDL_Texture *exitText = IMG_LoadTexture(renderer3, "../../res/gfx/exit.png");
	SDL_Rect exitRect = { WIDTH / 2 - 75 , HEIGHT / 2 + 150, 150, 100 };

	if (bgTexture == nullptr) {
		cout << "error";
	}
	SDL_Event event;
	while (alive)
	{
		
		while (SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				alive = !alive;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.motion.x <= exitRect.x + exitRect.w && event.motion.x >= exitRect.x  && event.motion.y <= exitRect.y + exitRect.h && event.motion.y >= exitRect.h) {
					alive = !alive;
				}
				if (event.motion.x <= replayRect.x + replayRect.w && event.motion.x >= replayRect.x  && event.motion.y <= replayRect.y + replayRect.h && event.motion.y >= replayRect.h) {
					SDL_DestroyTexture(bgTexture);
					SDL_DestroyTexture(gameOverText);
					SDL_DestroyTexture(replayText);
					SDL_DestroyTexture(exitText);
					SDL_DestroyWindow(gameOver);
					SDL_DestroyRenderer(renderer3);
					char** maining = (char**)'a';
					main(1, maining);
				}

			}
		}
		SDL_RenderCopy(renderer3, bgTexture, nullptr, &bgRect);
		SDL_RenderCopy(renderer3, replayText, nullptr, &replayRect);
		SDL_RenderCopy(renderer3, gameOverText, nullptr, &gameOverRect);
		for (int i = 0; i < 10; i++) {
			SDL_RenderCopy(renderer3, mTexture[i], nullptr, &textRect[i]);
		}
		SDL_RenderCopy(renderer3, rankingTexture, nullptr, &rankingRect);
		SDL_RenderCopy(renderer3, exitText, nullptr, &exitRect);
		SDL_RenderPresent(renderer3);
	}

}
void play() {
	score = 0;
	TTF_Init();
	//cout << "counter: " << counter << endl;
	if (counter = 1) {
		iSpeed = iSpeed / 100;
		counter++;
	}
	//cout << "counter: " << counter << endl;
	srand(time(NULL));
	SDL_Window* game = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer2 = SDL_CreateRenderer(game, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	//load sprites
	SDL_Texture *bgTexture2 = IMG_LoadTexture(renderer2, "../../res/gfx/blacks.png");
	SDL_Texture *ship = IMG_LoadTexture(renderer2, "../../res/gfx/ship.png");

	SDL_Texture *bigA = IMG_LoadTexture(renderer2, "../../res/gfx/asteroid3.png");
	SDL_Texture *nomA = IMG_LoadTexture(renderer2, "../../res/gfx/asteroid.png");
	SDL_Texture *smlA = IMG_LoadTexture(renderer2, "../../res/gfx/asteroid1.png");

	SDL_Texture *bullet = IMG_LoadTexture(renderer2, "../../res/gfx/bullet.png");
	SDL_Texture *vida = IMG_LoadTexture(renderer2, "../../res/gfx/heart.png");

	//initialize all rects
	SDL_Rect bigARect = { rand() % WIDTH, rand() % HEIGHT, 100, 100 };
	SDL_Rect nomARect = { 50, 0, 75, 75 };
	SDL_Rect smlARect = { 150, 0, 25, 25 };

	SDL_Rect bgRect2 = { 0, 0, WIDTH, HEIGHT };

	SDL_Rect shipRect = { WIDTH / 2 - 25, HEIGHT / 2 - 50, 25, 50 };
	SDL_Rect bulletRect = { shipRect.x + (shipRect.w / 2), shipRect.y,10,10 };
	SDL_Rect cor = { WIDTH - 40,0,40,40 };
	//variable to control fps
	UINT32 start;
	UINT32 delta;
	//lives number & position
	for (int i = 0; i < iLives; i++) {
		cors.push_back(cor);
		//cout << "push";
	}
	for (int i = 0; i < iLives; i++) {
		if (i == 0) {
			cors[i].x = cors[0].x;
		}
		else {
			cors[i].x = cors[i - 1].x - 50;
		}
	}
	//enemy number
	for (int i = 0; i < iEnemies; i++) {



		bigAs.push_back(bigARect);
		//cout << iEnemies;
	}
	string textureText = "score: " + to_string(score);
	SDL_Color color = { 255,255,255 };
	TTF_Font* gFont = TTF_OpenFont("../../res/fnt/Hyperspace.otf", 24);
	

	
	
	
	//instantiate random arround map
	int q = 0;
	for (vector<SDL_Rect>::iterator it = bigAs.begin(); it != bigAs.end(); it++, q++)
	{

		int random = rand() % 2;
		//cout << random;
		if (random == 0) {
			bigAs[q].x = 0 - bigAs[q].w;//rand() % (WIDTH - WIDTH/2+ 1) + 0;;
			bigAs[q].y = rand() % (HEIGHT + bigAs[q].h);
		}
		else {
			bigAs[q].x = rand() % (WIDTH + bigAs[q].w);
			bigAs[q].y = 0 - bigAs[q].h;//rand() % (HEIGHT - HEIGHT / 2 + 1) + 0;;
		}
		//SDL_RenderCopy(renderer2, bigA, nullptr, &bigAs[j]);

	}
	//cors[2].x = cors[0].x + 50;

	//Control Events
	int xClick = 0;
	int yClick = 0;
	int shipX = 0;
	int shipY = 0;
	SDL_Event event2;
	bool lvlpas = false;
	while (iLives > 0)
	{
		textureText = "score:" + to_string(score);
		SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), color);
		SDL_Texture* mTexture = SDL_CreateTextureFromSurface(renderer2, textSurface);
		SDL_Rect textRect = { 5,0, textSurface->w,textSurface->h };
		//cout << iSpeed << endl;
		start = SDL_GetTicks();
		while (SDL_PollEvent(&event2)) {
			switch (event2.type)
			{
			case SDL_QUIT:
				iLives = 0;
				break;
			case SDL_MOUSEMOTION:
				x = event2.motion.x;
				y = event2.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN:
				xClick = event2.motion.x;
				yClick = event2.motion.y;
				shipX = shipRect.x;
				shipY = shipRect.y;
				bulletRect = { shipRect.x + (shipRect.w / 2), shipRect.y + (shipRect.h / 2) ,10,10 };
				bullets.push_back(bulletRect);



				/*
				switch (event2.key.keysym.sym) {
				case SDLK_LEFT:
				shipRect.x -= 0.5 * (1000 / fps - (SDL_GetTicks() - start));
				break;
				case SDLK_RIGHT:
				shipRect.x += 0.5 * (1000 / fps - (SDL_GetTicks() - start));
				break;
				case SDLK_UP:
				shipRect.y -= 0.5 * (1000 / fps - (SDL_GetTicks() - start));
				break;
				case SDLK_DOWN:
				shipRect.y += 0.5 * (1000 / fps - (SDL_GetTicks() - start));
				break;
				default:
				break;
				}*/
			}
		}
		//update

		const Uint8* keyboard = SDL_GetKeyboardState(NULL);

		if (keyboard[SDL_SCANCODE_A]) {
			shipRect.x -= 0.5 * (1000 / fps - (SDL_GetTicks() - start));
		}
		if (keyboard[SDL_SCANCODE_S]) {
			shipRect.y += 0.5 * (1000 / fps - (SDL_GetTicks() - start));
		}
		if (keyboard[SDL_SCANCODE_D]) {
			shipRect.x += 0.5 * (1000 / fps - (SDL_GetTicks() - start));
		}
		if (keyboard[SDL_SCANCODE_W]) {
			shipRect.y -= 0.5 * (1000 / fps - (SDL_GetTicks() - start));
		}
		if (shipRect.x <= -shipRect.w) shipRect.x = WIDTH;
		else if (shipRect.x >= WIDTH) shipRect.x = -shipRect.w;
		if (shipRect.y <= -shipRect.h) shipRect.y = HEIGHT;
		else if (shipRect.y >= HEIGHT) shipRect.y = -shipRect.h;


		//getdelta time
		uint32_t tick_time = SDL_GetTicks();
		delta = tick_time - start;
		start = tick_time;

		//MOUSE angle math
		int deltax = (shipRect.x) - x;
		int deltay = (shipRect.y) - y;
		double angle_deg = ((atan2(deltay, deltax)*180.0000) / 3.1416) - 90;
		//bullets movement 
		if (!bullets.empty()) {
			//int r = 0;
			for (int r = 0; r < bullets.size(); r++)
			{
				bullets[r].x -= (shipX - xClick)*0.1;
				bullets[r].y -= (shipY - yClick)*0.1;
				//out of map
				if (bullets[r].x <= 0 - bullets[r].w || bullets[r].y <= 0 - bullets[r].h || bullets[r].x >= WIDTH || bullets[r].y >= HEIGHT) {
					//erase bullet
					bullets.erase((bullets.begin()++) + r);

				}


			}
		}

		//asteroids movement
		if (!bigAs.empty()) {
			for (int r = 0; r < bigAs.size(); r++) {

				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };
				bigAs[r].x -= (rand() % WIDTH) * iSpeed;
				bigAs[r].y -= (rand() % WIDTH) * iSpeed;
				if (bigAs[r].x <= -bigAs[r].w) bigAs[r].x = WIDTH;
				else if (bigAs[r].x >= WIDTH) bigAs[r].x = -bigAs[r].w;
				if (bigAs[r].y <= -bigAs[r].h) bigAs[r].y = HEIGHT;
				else if (bigAs[r].y >= HEIGHT) bigAs[r].y = -bigAs[r].h;

			}
		}
		if (!nomAs.empty()) {
			for (int r = 0; r < nomAs.size(); r++) {
				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };
				nomAs[r].x -= (rand() % WIDTH) * iSpeed + 1 / 100000000;
				nomAs[r].y -= (rand() % WIDTH) * iSpeed + 1 / 100000000;
				if (nomAs[r].x <= -nomAs[r].w) nomAs[r].x = WIDTH;
				else if (nomAs[r].x >= WIDTH) nomAs[r].x = 0 - nomAs[r].w;
				if (nomAs[r].y <= -nomAs[r].h) nomAs[r].y = HEIGHT;
				else if (nomAs[r].y >= HEIGHT) nomAs[r].y = 0 - nomAs[r].h;

			}
		}
		if (!smlAs.empty()) {
			for (int r = 0; r < smlAs.size(); r++) {
				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };
				smlAs[r].x -= (rand() % WIDTH) * iSpeed + 1 / 10000000;
				smlAs[r].y -= (rand() % WIDTH) * iSpeed + 1 / 10000000;
				if (smlAs[r].x <= -smlAs[r].w) smlAs[r].x = WIDTH;
				else if (smlAs[r].x >= WIDTH) smlAs[r].x = -smlAs[r].w;
				if (smlAs[r].y <= -smlAs[r].h) smlAs[r].y = HEIGHT;
				else if (smlAs[r].y >= HEIGHT) smlAs[r].y = -smlAs[r].h;
			}
		}
		//ship collision
		if (!bigAs.empty()) {
			for (int h = 0; h < bigAs.size(); h++) {
				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };

				if (((shipRect.x - bigAs[h].x) + (shipRect.y - bigAs[h].y)) * ((shipRect.x - bigAs[h].x) + (shipRect.y - bigAs[h].y)) < 0.5*0.5) {
					//cout << "xocBO";
					shipRect.x = WIDTH / 2 - shipRect.w;
					shipRect.y = HEIGHT / 2 - shipRect.h;
					cors.pop_back();
					iLives--;
				}



				//bullet collision
				for (int g = 0; g < bullets.size(); g++) {
					
						if (bullets[g].x >= bigAs[h].x && bullets[g].x <= bigAs[h].x + bigAs[h].w && bullets[g].y >= bigAs[h].y && bullets[g].y <= bigAs[h].y + bigAs[h].h) {
							score += 20;
							iSpeed += score / 10000000;
							bullets.erase(bullets.begin() + g);
							nomARect.x = bigAs[h].x;
							nomARect.y = bigAs[h].y;
							nomAs.push_back(nomARect);
							nomAs.push_back(nomARect);
							bigAs.erase(bigAs.begin() + h);

						}
					



				}
			}
		}
		if (!nomAs.empty()) {
			for (int h = 0; h < nomAs.size(); h++) {
				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };


				if (nomAs[h].x >= shipRect.x && nomAs[h].x <= shipRect.x + shipRect.w && nomAs[h].y >= shipRect.y && nomAs[h].y <= shipRect.y + shipRect.h) {
					//
					shipRect.x = WIDTH / 2 - shipRect.w;
					shipRect.y = HEIGHT / 2 - shipRect.h;
					//cout << "xoc";
					cors.pop_back();
					iLives--;



				}


				//bullet collision
				for (int g = 0; g < bullets.size(); g++) {

					if (bullets[g].x >= nomAs[h].x && bullets[g].x <= nomAs[h].x + nomAs[h].w && bullets[g].y >= nomAs[h].y && bullets[g].y <= nomAs[h].y + nomAs[h].h) {
						score += 50;
						iSpeed += score / 10000000;
						bullets.erase(bullets.begin() + g);
						smlARect.x = nomAs[h].x;
						smlARect.y = nomAs[h].y;
						smlAs.push_back(smlARect);
						smlAs.push_back(smlARect);
						nomAs.erase(nomAs.begin() + h);

					}



				}
			}

		}
		if (!smlAs.empty()) {
			for (int h = 0; h < smlAs.size(); h++) {
				//srand(time(NULL));
				//SDL_Rect target = { (rand() % WIDTH),(rand() % HEIGHT),0,0 };


				if (smlAs[h].x >= shipRect.x && smlAs[h].x <= shipRect.x + shipRect.w && smlAs[h].y >= shipRect.y && smlAs[h].y <= shipRect.y + shipRect.h) {
					//
					shipRect.x = WIDTH / 2 - shipRect.w;
					shipRect.y = HEIGHT / 2 - shipRect.h;
					//cout << "xoc";
					cors.pop_back();
					iLives--;



				}


				//bullet collision
				for (int g = 0; g < bullets.size(); g++) {

					if (bullets[g].x >= smlAs[h].x && bullets[g].x <= smlAs[h].x + smlAs[h].w && bullets[g].y >= smlAs[h].y && bullets[g].y <= smlAs[h].y + smlAs[h].h) {
						score += 100;
						iSpeed += score / 10000000;
						bullets.erase(bullets.begin() + g);
						smlAs.erase(smlAs.begin() + h);

					}



				}
			}

		}



		//draw
		SDL_RenderCopy(renderer2, bgTexture2, nullptr, &bgRect2);
		//asteroids draw
		if (!bigAs.empty()) {
			int j = 0;
			for (vector<SDL_Rect>::iterator it = bigAs.begin(); it != bigAs.end(); it++, j++)
			{
				SDL_RenderCopy(renderer2, bigA, nullptr, &bigAs[j]);
			}
		}
		//SDL_RenderCopy(renderer2, bigA, nullptr, &bigARect);
		if (!nomAs.empty()) {
			int k = 0;
			for (vector<SDL_Rect>::iterator it = nomAs.begin(); it != nomAs.end(); it++, k++)
			{
				SDL_RenderCopy(renderer2, nomA, nullptr, &nomAs[k]);
			}
		}
		//SDL_RenderCopy(renderer2, nomA, nullptr, &nomARect);
		if (!smlAs.empty()) {
			int l = 0;
			for (vector<SDL_Rect>::iterator it = smlAs.begin(); it != smlAs.end(); it++, l++)
			{
				SDL_RenderCopy(renderer2, smlA, nullptr, &smlAs[l]);
			}
		}
		//SDL_RenderCopy(renderer2, smlA, nullptr, &smlARect);
		//life draw
		if (!cors.empty()) {
			int i = 0;
			for (vector<SDL_Rect>::iterator it = cors.begin(); it != cors.end(); it++, i++)
			{
				SDL_RenderCopy(renderer2, vida, nullptr, &cors[i]);
			}
		}
		if (!bullets.empty()) {
			int b = 0;
			for (vector<SDL_Rect>::iterator it = bullets.begin(); it != bullets.end(); it++, b++)
			{
				SDL_RenderCopy(renderer2, bullet, nullptr, &bullets[b]);
			}
		}
		//SDL_RenderCopy(renderer2, bullet, nullptr, &bulletRect);
		//rotate ship on mouse
		SDL_RenderCopyEx(renderer2, ship, NULL, &shipRect, angle_deg, NULL, SDL_FLIP_NONE);
		SDL_RenderCopy(renderer2, mTexture, nullptr, &textRect);
		SDL_RenderPresent(renderer2);
		//regulate FPS
		if ((1000 / fps) > (SDL_GetTicks() - start)) {
			SDL_Delay(1000 / fps - (SDL_GetTicks() - start));
		}
		if (bigAs.empty() && nomAs.empty() && smlAs.empty()) {
			lvlpas = true;
		}
		if (lvlpas) {
			lvl++;
			iEnemies += iPlus*lvl;
			SDL_DestroyTexture(bgTexture2);
			SDL_DestroyTexture(ship);
			SDL_DestroyTexture(bullet);
			SDL_DestroyTexture(vida);
			SDL_DestroyTexture(bigA);
			SDL_DestroyTexture(nomA);
			SDL_DestroyTexture(smlA);
			SDL_DestroyRenderer(renderer2);
			SDL_DestroyWindow(game);
			play();

		}
	}

	if (!bigAs.empty()) {
		bigAs.clear();
	}
	if (!smlAs.empty()) {
		smlAs.clear();
	}
	if (!nomAs.empty()) {
		nomAs.clear();
	}

	SDL_DestroyTexture(bgTexture2);
	SDL_DestroyTexture(ship);
	SDL_DestroyTexture(bullet);
	SDL_DestroyTexture(vida);
	SDL_DestroyTexture(bigA);
	SDL_DestroyTexture(nomA);
	SDL_DestroyTexture(smlA);

	SDL_DestroyRenderer(renderer2);
	SDL_DestroyWindow(game);
	die();
}

int menu() {

	TTF_Init();
	




	alive = true;
	//creamos Window i Renderer
	SDL_Window *window = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	//create and load textures
	SDL_Texture *bgTexture = IMG_LoadTexture(renderer, "../../res/gfx/blacks.png");
	SDL_Texture *asteroidsTexture = IMG_LoadTexture(renderer, "../../res/gfx/Asteroids.png");
	SDL_Texture *ezTexture = IMG_LoadTexture(renderer, "../../res/gfx/easy.png");
	SDL_Texture *nmTexture = IMG_LoadTexture(renderer, "../../res/gfx/medium.png");
	SDL_Texture *hdTexture = IMG_LoadTexture(renderer, "../../res/gfx/hard.png");

	SDL_Texture *playTexture = IMG_LoadTexture(renderer, "../../res/gfx/play.png");
	SDL_Texture *exitTexture = IMG_LoadTexture(renderer, "../../res/gfx/exit.png");

	


	if (bgTexture == NULL) cout << SDL_GetError();
	//set textures rect's
	SDL_Rect bgRect = { 0, 0, WIDTH, HEIGHT };
	SDL_Rect asteroidsRect = { (WIDTH / 2) - 200,10,400,200 };
	SDL_Rect ezRect = { (WIDTH / 4) - 100, HEIGHT / 2, 100, 100 };
	SDL_Rect nmRect = { (WIDTH / 2) - 50, HEIGHT / 2, 100, 100 };
	SDL_Rect hdRect = { (WIDTH / 4) * 3, HEIGHT / 2, 100, 100 };

	SDL_Rect playRect = { WIDTH / 2 - 75 , HEIGHT / 2 - 50, 150, 100 };
	SDL_Rect exitRect = { WIDTH / 2 - 75 , HEIGHT / 2 + 100, 150, 100 };

	xml_document<> doc;
	ifstream file("GameInfo.xml");
	stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	string content(buffer.str());
	doc.parse<0>(&content[0]);
	xml_node<> *pRoot = doc.first_node();
	bool clicked = false;
	//cout << alive;
	SDL_Event event;
	while (alive) {
		
		
		if (clicked) {
			
			while (SDL_PollEvent(&event))
			{

				switch (event.type)
				{
				case SDL_QUIT:
					alive = false;
					break;
				case SDL_MOUSEBUTTONDOWN:

					//cout << event.motion.x << "-" << event.motion.y;
					if (event.motion.x <= (WIDTH / 4) && event.motion.x >= (WIDTH / 4) - 100 && event.motion.y <= HEIGHT / 2 + 100 && event.motion.y >= HEIGHT / 2) {
						cout << "Easy" << endl;
						//cridar funcior play i mirar dificultat
						if (xml_node<> *pNode = pRoot->first_node("easy"))
						{
							for (xml_node<> *pNodeI = pNode->first_node(); pNodeI; pNodeI = pNodeI = pNodeI->next_sibling())
							{
								if (pNodeI->name_size() == 5) {
									//cout << "vides -";
									lives = pNodeI->value();
									//cout << lives << endl;
									std::string::size_type sz;
									iLives = stoi(lives, &sz);

								}
								if (pNodeI->name_size() == 8) {
									//cout << "speed -";
									speed = pNodeI->value();
									//cout << speed << endl;
									std::string::size_type sz;
									iSpeed = stoi(speed, &sz);
									//cout << iSpeed;
								}
								if (pNodeI->name_size() == 7) {
									//cout << "enemies -";
									enemies = pNodeI->value();
									//cout << enemies << endl;
									std::string::size_type sz;
									iEnemies = stoi(enemies, &sz);
								}
								if (pNodeI->name_size() == 11) {
									//cout << "plus -";
									pluse = pNodeI->value();
									//cout << pluse << endl;
									std::string::size_type sz;
									iPlus = stoi(pluse, &sz);
								}
								//cout << pNodeI->name() << ':' << pNodeI->value() << '\n';
							}
						}

						SDL_DestroyTexture(bgTexture);
						SDL_DestroyTexture(ezTexture);
						SDL_DestroyTexture(nmTexture);
						SDL_DestroyTexture(hdTexture);
						SDL_DestroyTexture(asteroidsTexture);
						
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						play();
					}
					else if (event.motion.x <= (WIDTH / 2) - 50 + 100 && event.motion.x >= (WIDTH / 2) - 50 && event.motion.y <= HEIGHT / 2 + 100 && event.motion.y >= HEIGHT / 2) {
						cout << "Normal" << endl;
						//cridar funcior play i mirar dificultat
						if (xml_node<> *pNode = pRoot->first_node("normal"))
						{
							for (xml_node<> *pNodeI = pNode->first_node(); pNodeI; pNodeI = pNodeI = pNodeI->next_sibling())
							{
								if (pNodeI->name_size() == 5) {
									cout << "vides -";
									lives = pNodeI->value();
									cout << lives << endl;
									std::string::size_type sz;
									iLives = stoi(lives, &sz);
								}
								if (pNodeI->name_size() == 8) {
									cout << "speed -";
									speed = pNodeI->value();
									cout << speed << endl;
									std::string::size_type sz;
									iSpeed = stoi(speed, &sz);
								}
								if (pNodeI->name_size() == 7) {
									cout << "enemies -";
									enemies = pNodeI->value();
									cout << enemies << endl;
									std::string::size_type sz;
									//iEnemies = stoi(enemies, &sz);
								}
								if (pNodeI->name_size() == 11) {
									cout << "plus -";
									pluse = pNodeI->value();
									cout << pluse << endl;
								}
								//cout << pNodeI->name() << ':' << pNodeI->value() << '\n';
							}
						}

						SDL_DestroyTexture(bgTexture);
						SDL_DestroyTexture(ezTexture);
						SDL_DestroyTexture(nmTexture);
						SDL_DestroyTexture(hdTexture);
						SDL_DestroyTexture(asteroidsTexture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						play();
					}
					else if (event.motion.x <= (WIDTH / 4) * 3 + 100 && event.motion.x >= (WIDTH / 4) * 3 && event.motion.y <= HEIGHT / 2 + 100 && event.motion.y >= HEIGHT / 2) {
						cout << "Hard" << endl;
						//cridar funcior play i mirar dificultat
						if (xml_node<> *pNode = pRoot->first_node("hard"))
						{
							for (xml_node<> *pNodeI = pNode->first_node(); pNodeI; pNodeI = pNodeI = pNodeI->next_sibling())
							{
								if (pNodeI->name_size() == 5) {
									cout << "vides -";
									lives = pNodeI->value();
									cout << lives << endl;
									std::string::size_type sz;
									iLives = stoi(lives, &sz);
								}
								if (pNodeI->name_size() == 8) {
									cout << "speed -";
									speed = pNodeI->value();
									cout << speed << endl;
									std::string::size_type sz;
									iSpeed = stoi(speed, &sz);
								}
								if (pNodeI->name_size() == 7) {
									cout << "enemies -";
									enemies = pNodeI->value();
									cout << enemies << endl;
									std::string::size_type sz;
									iEnemies = stoi(enemies, &sz);
								}
								if (pNodeI->name_size() == 11) {
									cout << "plus -";
									pluse = pNodeI->value();
									cout << pluse << endl;
								}
								//cout << pNodeI->name() << ':' << pNodeI->value() << '\n';
							}
						}

						SDL_DestroyTexture(bgTexture);
						SDL_DestroyTexture(ezTexture);
						SDL_DestroyTexture(nmTexture);
						SDL_DestroyTexture(hdTexture);
						SDL_DestroyTexture(asteroidsTexture);
						SDL_DestroyRenderer(renderer);
						SDL_DestroyWindow(window);
						play();
					}
				}


			}


			SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
			SDL_RenderCopy(renderer, ezTexture, nullptr, &ezRect);
			SDL_RenderCopy(renderer, nmTexture, nullptr, &nmRect);
			SDL_RenderCopy(renderer, hdTexture, nullptr, &hdRect);
			SDL_RenderCopy(renderer, asteroidsTexture, nullptr, &asteroidsRect);
			SDL_RenderPresent(renderer);
		}
		else
		{
			
			
			while (SDL_PollEvent(&event))
			{

				switch (event.type)
				{
				case SDL_QUIT:
					alive = false;
					break;
				case SDL_MOUSEBUTTONDOWN:

					//cout << event.motion.x << "-" << event.motion.y;
					if (event.motion.x <= playRect.x + playRect.w && event.motion.x >= playRect.x && event.motion.y <= playRect.y + playRect.h && event.motion.y >= playRect.y) {
						clicked = true;
					}
					if (event.motion.x <= exitRect.x + exitRect.w && event.motion.x >= exitRect.x && event.motion.y <= exitRect.y + exitRect.h && event.motion.y >= exitRect.y) {
						alive = false;
					}
					break;
				}
			}
			SDL_RenderCopy(renderer, playTexture, nullptr, &playRect);
			SDL_RenderCopy(renderer, exitTexture, nullptr, &exitRect);
			SDL_RenderCopy(renderer, asteroidsTexture, nullptr, &asteroidsRect);
			
			SDL_RenderPresent(renderer);
		}


	}
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(asteroidsTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return 0;
}
int main(int argc, char** args)
{
	menu();

	return 0;
}