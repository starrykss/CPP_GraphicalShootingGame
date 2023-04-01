#include "SoundTest_GameFunc.h"
#include "SDL_image.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <sstream>

extern SDL_Window* g_window;
extern SDL_Renderer* g_renderer;
extern bool g_flag_running;

bool launched = false;
int result;

int f_state = 0;   // 0 : 정지, 1 : 위쪽, 2 : 아래쪽, 3 : 오른쪽, 4 : 왼쪽, 5 : 정지

struct Missile {
	int x;
	int y;
	Missile(int x, int y) : x(x), y(y) { isDestroyed = false; }
	bool isDestroyed;
};

std::vector<Missile*> missles;

Mix_Chunk *wave1;
Mix_Chunk *wave2;
Mix_Music *music1;

// 폰트
TTF_Font *game_font;			

SDL_Texture *text_point;
SDL_Texture *text_score;

SDL_Rect text_point_rect;


SDL_Texture* g_bg_texture;      // 배경 Texture
SDL_Texture* g_f_texture;       // 비행기 Texture
SDL_Texture* g_m_texture;       // 미사일 Texture

SDL_Rect g_bg_source_rect;      // 배경 Source Rect
SDL_Rect g_bg_destination_rect; // 배경 destination Rect
SDL_Rect g_m_source_rect;       // 미사일 Source Rect
SDL_Rect g_m_destination_rect;  // 미사일 destination Rect
SDL_Rect g_f_source_rect;       // 비행기 Source Rect
SDL_Rect g_f_destination_rect;  // 비행기 destination Rect


// 점수 변경과 관련된 부분
int score = 0;

std::string IntToString(int n) {
	std::stringstream sstream;
	sstream << n;
	return sstream.str();
}

void InitGame()
{
	// 명령창 꾸미기
	{
		std::cout << std::endl;
		std::cout << "† JET SHOOTING GAME (2.0V) †" << std::endl;
		std::cout << "│ Keys : ↑(UP),↓(DOWN), →(RIGHT), ←(LEFT), SpaceBar(Shooting Missile), LSHIFT(STAY)" << std::endl;
		std::cout << "[COMMAND]" << std::endl;
		std::cout << std::endl;
	}


	// 배경
	{
		SDL_Surface* bg_sheet_surface = IMG_Load("Resources/sea.jpg");			// CPU에
		g_bg_destination_rect.x = g_bg_source_rect.x = 0;
		g_bg_destination_rect.y = g_bg_source_rect.y = 0;
		g_bg_destination_rect.w = g_bg_source_rect.w = bg_sheet_surface->w;
		g_bg_destination_rect.h = g_bg_source_rect.h = bg_sheet_surface->h;

		g_bg_texture = SDL_CreateTextureFromSurface(g_renderer, bg_sheet_surface);
		SDL_FreeSurface(bg_sheet_surface);
	}

	// 폰트 설정
	{
		game_font = TTF_OpenFont("Resources/hemi.ttf", 40);	// 폰트 파일 불러오기. 폰트 사이즈 : 40
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

	// 음악 볼륨 설정
	{
		Mix_VolumeMusic(128);

		// wave, mp3 파일 불러오기
		wave1 = Mix_LoadWAV("Resources/missilelaunch.wav");		// 효과음 1 : 미사일 발사
		wave2 = Mix_LoadWAV("Resources/moving.wav");					// 효과음 2 : 비행기 이동
		if (wave1 == NULL || wave2 == NULL) {
			printf("Couldn't load the wav: %s\n", Mix_GetError());
		}

		music1 = Mix_LoadMUS("Resources/warbgm.mp3");				// 배경음
		if (!music1) {
			printf(" %s\n", Mix_GetError());
		}

		Mix_FadeInMusic(music1, -1, 2000);
	}
}

// Score와 관련이 있는 함수

void ScoreTextureUpdate()
{
	std::string text = "Score: " + IntToString(score);

	SDL_Color black = { 0, 0, 0, 0 };
	SDL_Surface *tmp_surface = TTF_RenderText_Blended(game_font, text.c_str(), black);

	text_point_rect.x = 0;
	text_point_rect.y = 0;
	text_point_rect.w = tmp_surface->w;
	text_point_rect.h = tmp_surface->h;

	text_point = SDL_CreateTextureFromSurface(g_renderer, tmp_surface);

	SDL_FreeSurface(tmp_surface);

}


void Update()
{
	ScoreTextureUpdate();
	/*
	if (launched == true) {	// 미사일 발사 될 때 점수 증가
		score += 5;		
	}
	*/

	// 비행기
	{
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
		else if (f_state == 5) { // 제자리에 정지
			g_f_destination_rect.x += 0;
		}
	}

	// 비행기 비행 구역 지정
	{
		if (g_f_destination_rect.x >= 956) {
			g_f_destination_rect.x = 956; 
		}
		if (g_f_destination_rect.x <= -50) {
			g_f_destination_rect.x = -50; 
		}
		if (g_f_destination_rect.y <= -50) {
			g_f_destination_rect.y = -50; 
		}
		if (g_f_destination_rect.y >= 600) {
			g_f_destination_rect.y = 600; 
		}
	}

	// 미사일
	{
		for (size_t i = 0; i < missles.size(); i++) {
			if (missles[i]->y > 0 && missles[i]->isDestroyed == false)
				missles[i]->y -= 100; // 미사일 속도
			else
				missles[i]->isDestroyed = true;
		}
	}


}

void Render()
{
	SDL_RenderClear(g_renderer);
	SDL_RenderCopy(g_renderer, g_bg_texture, &g_bg_source_rect, &g_bg_destination_rect);  // 배경 Render
	SDL_RenderCopy(g_renderer, g_f_texture, &g_f_source_rect, &g_f_destination_rect);  // 비행기 Render

	SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);

	// 미사일
	{
		g_m_destination_rect.x = g_f_destination_rect.x + 27;
		g_m_destination_rect.y = g_f_destination_rect.y;

		for (size_t i = 0; i < missles.size(); i++) {
			if (missles[i]->isDestroyed == false) {
				g_m_destination_rect.x = missles[i]->x;
				g_m_destination_rect.y = missles[i]->y;
				SDL_RenderCopy(g_renderer, g_m_texture, &g_m_source_rect, &g_m_destination_rect);  // 미사일 Render
			}
		}
	}

	//폰트
	{

		{ // Point
			SDL_Rect r;
			r.x = text_point_rect.x;
			r.y = 0;
			r.w = text_point_rect.w;
			r.h = text_point_rect.h;
			SDL_RenderCopy(g_renderer, text_point, 0, &r);
		}
	}

	SDL_RenderPresent(g_renderer);
}



void HandleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			g_flag_running = false;
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_UP) {
				f_state = 1;
				std::cout << "Moving Upward" << std::endl;
				Mix_PlayChannel(0, wave2, 0);
			}
			else if (event.key.keysym.sym == SDLK_DOWN) {
				f_state = 2;
				std::cout << "Moving Downward" << std::endl;
				Mix_PlayChannel(0, wave2, 0);
			}
			else if (event.key.keysym.sym == SDLK_RIGHT) {
				f_state = 3;
				std::cout << "Moving Rightward" << std::endl;
				Mix_PlayChannel(0, wave2, 0);
			}
			else if (event.key.keysym.sym == SDLK_LEFT) {
				f_state = 4;
				std::cout << "Moving Leftward" << std::endl;
				Mix_PlayChannel(0, wave2, 0);
			}
			else if (event.key.keysym.sym == SDLK_LSHIFT) {
				f_state = 5;
				std::cout << "Staying" << std::endl;
			}
			else if (event.key.keysym.sym == SDLK_SPACE) {
				missles.push_back(new Missile(g_m_destination_rect.x, g_m_destination_rect.y));
				std::cout << "A Missile has just launched from our jet!" << std::endl;
				score += 5;
				Mix_PlayChannel(0, wave1, 0);
				launched = true;			// 미사일 발사 상태 : True
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
				launched = false;		// 미사일 발사 상태 : False
			}
			break;

		default:
			break;
		}
	}
}


void ClearGame()
{
	SDL_DestroyTexture(g_bg_texture);
	SDL_DestroyTexture(g_f_texture);
	SDL_DestroyTexture(g_m_texture);
	SDL_DestroyTexture(text_point);

	if (wave1) Mix_FreeChunk(wave1);
	if (wave2) Mix_FreeChunk(wave2);
	if (music1) Mix_FreeMusic(music1);

	TTF_CloseFont(game_font); // 메모리에서 폰트 데이터 제거
}

