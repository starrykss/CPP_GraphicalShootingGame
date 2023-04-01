#include "Drawing_GameFunc.h"
#include "SDL_image.h"
#include <iostream>
#include <windows.h>
#include <vector>

int f_state = 0;   // 0 : 정지, 1 : 위쪽, 2 : 아래쪽, 3 : 오른쪽, 4 : 왼쪽
int m_state = 0;

struct Missile {
	int x;
	int y;
	Missile(int x, int y) : x(x), y(y) { isDestroyed = false; }
	bool isDestroyed;
};

std::vector<Missile*> missles;


extern SDL_Renderer* g_renderer;
SDL_Texture* g_bg_texture;      // 배경 Texture
SDL_Texture* g_f_texture;       // 비행기 Texture
SDL_Texture* g_m_texture;       // 미사일 Texture

SDL_Rect g_bg_source_rect;      // 배경 Source
SDL_Rect g_bg_destination_rect; // 배경 destination
SDL_Rect g_m_source_rect;       // 미사일 Source
SDL_Rect g_m_destination_rect;  // 미사일 destination
SDL_Rect g_f_source_rect;       // 비행기 Source
SDL_Rect g_f_destination_rect;  // 비행기 destination

void InitGame() {
	std::cout << std::endl;
	std::cout << "† GRAPHICAL SHOOTING GAME †" << std::endl;
	std::cout << "│ Keys : ↑(UP),↓(DOWN), →(RIGHT), ←(LEFT), SpaceBar(Shooting Missile)" << std::endl;
	std::cout << "[COMMAND]" << std::endl;
	std::cout << std::endl;


	// 배경
	{
		SDL_Surface* bg_sheet_surface = IMG_Load("Resources/sea.jpg");
		g_bg_destination_rect.x = g_bg_source_rect.x = 0;
		g_bg_destination_rect.y = g_bg_source_rect.y = 0;
		g_bg_destination_rect.w = g_bg_source_rect.w = bg_sheet_surface->w;
		g_bg_destination_rect.h = g_bg_source_rect.h = bg_sheet_surface->h;

		g_bg_texture = SDL_CreateTextureFromSurface(g_renderer, bg_sheet_surface);
		SDL_FreeSurface(bg_sheet_surface);
	}

	// 비행기
	{
		SDL_Surface* g_f_sheet_surface = IMG_Load("Resources/jet.png");
		g_f_source_rect.x = 0;
		g_f_source_rect.y = 0;
		g_f_source_rect.w = 105;
		g_f_source_rect.h = 175;

		g_f_destination_rect.x = 512;
		g_f_destination_rect.y = 300;
		g_f_destination_rect.w = g_f_source_rect.w;
		g_f_destination_rect.h = g_f_source_rect.h;

		g_f_texture = SDL_CreateTextureFromSurface(g_renderer, g_f_sheet_surface);	// 비행기
		SDL_FreeSurface(g_f_sheet_surface);
	}

	// 미사일
	{
		SDL_Surface* g_m_surface = IMG_Load("Resources/missile.png");
		g_m_source_rect.x = 0;
		g_m_source_rect.y = 0;
		g_m_source_rect.w = 800;
		g_m_source_rect.h = 800;

		g_m_destination_rect.x = g_f_destination_rect.x;
		g_m_destination_rect.y = g_f_destination_rect.x;
		g_m_destination_rect.w = 50;
		g_m_destination_rect.h = 50;

		g_m_texture = SDL_CreateTextureFromSurface(g_renderer, g_m_surface);
		SDL_FreeSurface(g_m_surface);
	}
}

void Update() {


	if (f_state == 1) { // 위쪽으로 이동
		g_f_destination_rect.y -= 10;
	}
	else if (f_state == 2) { // 아래쪽으로 이동
		g_f_destination_rect.y += 10;
	}
	else if (f_state == 3) { // 오른쪽으로 이동
		g_f_destination_rect.x += 10;
	}
	else if (f_state == 4) { // 왼쪽으로 이동
		g_f_destination_rect.x -= 10;
	}


	// 비행기 비행 구역 지정

	if (g_f_destination_rect.x >= 956) {
		g_f_destination_rect.x = 956;
	}
	if (g_f_destination_rect.x <= -50) {
		g_f_destination_rect.x = -50;
	}
	if (g_f_destination_rect.y <= -50) {
		g_f_destination_rect.y = -50;
	}
	if (g_f_destination_rect.y >= 700) {
		g_f_destination_rect.y = 700;
	}

	// 미사일

	for (size_t i = 0; i < missles.size(); i++) {
		if (missles[i]->y > 0 && missles[i]->isDestroyed == false)
			missles[i]->y -= 50; // 미사일 속도
		else
			missles[i]->isDestroyed = true;
	}
}

void Render() {
	SDL_RenderClear(g_renderer);
	SDL_RenderCopy(g_renderer, g_bg_texture, &g_bg_source_rect, &g_bg_destination_rect);  // 배경 Render
	SDL_RenderCopy(g_renderer, g_f_texture, &g_f_source_rect, &g_f_destination_rect);  // 비행기 Render
	
	// 미사일

	g_m_destination_rect.x = g_f_destination_rect.x + 27;
	g_m_destination_rect.y = g_f_destination_rect.y;


	for (size_t i = 0; i < missles.size(); i++) {
		if (missles[i]->isDestroyed == false) {
			g_m_destination_rect.x = missles[i]->x;
			g_m_destination_rect.y = missles[i]->y;
			SDL_RenderCopy(g_renderer, g_m_texture, &g_m_source_rect, &g_m_destination_rect);  // 미사일 Render
		}
	}

	SDL_RenderPresent(g_renderer);

}

void HandleEvents() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			exit(0);
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_UP) {
				f_state = 1;
				std::cout << "Moving Upward" << std::endl;
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				f_state = 2;
				std::cout << "Moving Downward" << std::endl;
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				f_state = 3;
				std::cout << "Moving Rightward" << std::endl;
			}
			else if (event.key.keysym.sym == SDLK_LEFT) {
				f_state = 4;
				std::cout << "Moving Leftward" << std::endl;
			}
			else if (event.key.keysym.sym == SDLK_SPACE) {
				// m_state = 1;
				missles.push_back(new Missile(g_m_destination_rect.x, g_m_destination_rect.y));
				std::cout << "A Missile has just launched from our jet!" << std::endl;
			}

			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_UP) {
				f_state = 1;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				f_state = 2;
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				f_state = 3;
			}
			if (event.key.keysym.sym == SDLK_LEFT) {
				f_state = 4;
			}

			if (event.key.keysym.sym == SDLK_SPACE) {
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) {
			}

			break;

		default:
			break;
		}
	}
}

void ClearGame() {
	SDL_DestroyTexture(g_bg_texture);
	SDL_DestroyTexture(g_f_texture);
	SDL_DestroyTexture(g_m_texture);
}