#include <stdio.h>
#include <stdlib.h>
#include <SFML/Graphics.h>
#include <SFML/Audio.h>

#pragma region Define
#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 888
#define BPP 32
#define MAX_FPS 60
#define SCREEN_NAME "Timberman"

#define GROUND SCREEN_HEIGHT * 0.82f 
#define MAX_LIFE_TIME 10
#define BASE_POSITION -1
#pragma endregion

#pragma region Struct and Enum
typedef enum GameState
{
	MENU,
	GAME,
	GAME_OVER,
}GameState;

typedef enum TruncType
{
	NORMAL,
	NORMAL2,
	LEFT,
	RIGHT,
}TruncType;

typedef struct MainData
{
	sfRenderWindow* renderWindow;
	sfClock* clock;
}MainData;

typedef struct Animation
{
	sfTexture* texture;
	sfSprite* sprite;
	int frameCount;
	float frameRate;
	int currentFrame;
	sfBool isLooping;
	sfBool isFinished;
}Animation;

typedef struct PlayerAnimation
{
	Animation* currentAnim;
	Animation idle;
	Animation woodcutting;
	Animation dead;
}PlayerAnimation;

typedef struct Player
{
	PlayerAnimation animation;
	float animationTime;
	int dir;
	sfBool dead;
	sfBool isCutting;
	sfSoundBuffer* soundBufferCutting;
	sfSoundBuffer* soundBufferDeath;
	sfSound* soundPlay;
}Player;

typedef struct Color
{
	sfColor blueGrey;
}Color;

typedef struct HUD
{
	sfFont* font;
	sfFont* fontScore;
	sfText* fpsText;
	sfText* scoreText;
	sfText* maxScoreText;
	sfSprite* title;
	sfSprite* button;
	sfSprite* gameOver;
	sfSprite* timeContainer;
	sfSprite* timeBar;
	sfBool isColiding;
}HUD;

typedef struct TrunKTexture
{
	sfTexture* trunc1;
	sfTexture* trunc2;
	sfTexture* branchLeft;
	sfTexture* branchRight;
}TrunKTexture;

typedef struct Level
{
	sfSprite* background;
	sfSprite* baseLog;
	sfSprite* trunc1;
	sfSprite* trunc2;
	sfSprite* trunc3;
	sfSprite* trunc4;
	sfSprite* trunc5;
	sfSprite* trunc6;
	TrunKTexture texture;
	sfMusic* music;
}Level;

typedef struct Game
{
	Player player;
	Level level;
	sfBool isGameStarted;
	float lifeTime;
	int score;
	int maxScore;
}Game;

typedef struct GameData
{
	HUD hud;
	Color color;
	Game game;
	GameState gameState;
	sfBool isDebug;
}GameData;
#pragma endregion

#pragma region Definition
void Load(MainData* const _mainData, GameData* const _gameData);

void PollEvent(sfRenderWindow* _renderWindow, GameData* const _gameData);
void OnKeyPressed(sfKeyEvent _key, sfRenderWindow* _renderWindow, GameData* const _gameData);
void OnMouseButtonPressed(sfMouseButtonEvent _button);
void OnMouseMoved(void);

void Update(MainData* const _mainData, GameData* const _gameData);
void Draw(sfRenderWindow* const _renderWindow, GameData* const _gameData);
void Cleanup(MainData* const _mainData, GameData* const _gameData);

void Reset(GameData* const _gameData);

void LoadScreen(MainData* const _mainData);
void LoadHud(HUD* const _hud);
void UpdateHud(float const _dt, GameData* const _gameData);
void CleanupHud(HUD* const _hud);

void UpdateText(sfText* const _text, int _value);
void CreateSprite(sfSprite** const _sprite, sfVector2f position, const char* _filepath);

void SetupAnimation(Animation* _anim, sfTexture** const _texture, int _frameCount, float _frameRate, sfBool _isLooping);
void AnimateSprite(Animation* _anim, float const _dt);
sfBool AnimIsFinished(Animation* const _anim);
void cleanupAnimation(Animation* animation);

void LoadGame(Game* const _game);
void GameOnKeyPressed(sfKeyEvent _key, Game* const _game);
void UpdateButton(float _dt, sfRenderWindow* const _renderWindow, GameData* const _gameData);
void UpdateGame(float _dt, Game* const _game, HUD* const _hud, GameState _gameState);
void DrawButton(sfRenderWindow* const _renderWindow, HUD* const _hud);

void LoadLevel(Level* const _level);
void DrawLevel(sfRenderWindow* const _renderWindow, Level* const _level);
void CleanupLevel(Level* const _level);

void CheckPlayerCollide(Level* const _level, Player* const _player);

void UpdateLifeBar(float _dt, Game* const _game, HUD* const _hud, sfBool _isStarted);

void CreateTrunc(sfSprite** const _trunc, sfVector2f position);
void AsigneTruncTexture(sfSprite** const _trunc, TruncType _truncType, TrunKTexture* _texture);
void UpdateTruncTexture(Level* const _level);

void LoadPlayer(Player* const _player);
void LoadPlayerAnimations(Player* const _player);
void PlayerUpdateMovement(Player* const _player);
void PlayerUpdateAnimation(float _dt, Player* const _player);
void DrawPlayer(sfRenderWindow* const _renderWindow, Player* const _player);
void CleanupPlayer(Player* const _player);
#pragma endregion

#pragma region Core
int main(void)
{
	MainData mainData = { 0 };
	GameData gameData = { 0 };
	Load(&mainData, &gameData);

	while (sfRenderWindow_isOpen(mainData.renderWindow))
	{
		PollEvent(mainData.renderWindow, &gameData);

		Update(&mainData, &gameData);

		Draw(mainData.renderWindow, &gameData);
	}

	Cleanup(&mainData, &gameData);

	return EXIT_SUCCESS;
}

void Load(MainData* const _mainData, GameData* const _gameData)
{
	LoadScreen(_mainData);
	LoadHud(&_gameData->hud);
	LoadGame(&_gameData->game);

	_gameData->gameState = MENU;
	_gameData->isDebug = sfFalse;
	_gameData->color.blueGrey = sfColor_fromRGB(119, 136, 153);
	_mainData->clock = sfClock_create();
}

void PollEvent(sfRenderWindow* _renderWindow, GameData* const _gameData)
{
	sfEvent event;
	while (sfRenderWindow_pollEvent(_renderWindow, &event))
	{
		switch (event.type)
		{
		case sfEvtClosed:
			sfRenderWindow_close(_renderWindow);
			break;
		case sfEvtKeyPressed:
			OnKeyPressed(event.key, _renderWindow, _gameData);
			break;
		case sfEvtMouseButtonPressed:
			OnMouseButtonPressed(event.mouseButton);
			break;
		case sfEvtMouseMoved:
			OnMouseMoved();
			break;
		default:
			break;
		}
	}
}

void OnKeyPressed(sfKeyEvent _key, sfRenderWindow* _renderWindow, GameData* const _gameData)
{

	switch (_key.code)
	{
	case sfKeyEscape:
		sfRenderWindow_close(_renderWindow);
		break;

	case sfKeyI:
		_gameData->isDebug = !_gameData->isDebug;
		break;
	case sfKeySpace:
		if (_gameData->gameState == GAME_OVER)
		{
			Reset(_gameData);
		}
		break;
	default:
		switch (_gameData->gameState)
		{
		case MENU:
			_gameData->gameState = GAME;
			_gameData->game.isGameStarted = sfTrue;
			break;
		case GAME:
			GameOnKeyPressed(_key, &_gameData->game);
			break;
		default:
			break;
		}
		break;
	}
}

void OnMouseMoved(void)
{
}

void OnMouseButtonPressed(sfMouseButtonEvent _button)
{
	switch (_button.button)
	{
	default:
		break;
	}
}

void Update(MainData* const _mainData, GameData* const _gameData)
{
	float dt = sfTime_asSeconds(sfClock_restart(_mainData->clock));
	UpdateHud(dt, _gameData);
	if (_gameData->gameState == MENU || _gameData->gameState == GAME_OVER)
	{
		UpdateButton(dt, _mainData->renderWindow, _gameData);
	}
	if (_gameData->game.player.dead)
	{
		_gameData->gameState = GAME_OVER;
	}
	UpdateGame(dt, &_gameData->game, &_gameData->hud, _gameData->gameState);
}

void Draw(sfRenderWindow* const _renderWindow, GameData* const _gameData)
{
	sfRenderWindow_clear(_renderWindow, _gameData->color.blueGrey);

	DrawLevel(_renderWindow, &_gameData->game.level);

	DrawPlayer(_renderWindow, &_gameData->game.player);

	if (_gameData->gameState == MENU || _gameData->gameState == GAME_OVER)
	{
		DrawButton(_renderWindow, &_gameData->hud);
	}
	if (_gameData->gameState == MENU)
	{
		sfRenderWindow_drawSprite(_renderWindow, _gameData->hud.title, NULL);
	}
	else if (_gameData->gameState == GAME_OVER)
	{
		sfRenderWindow_drawSprite(_renderWindow, _gameData->hud.gameOver, NULL);
		sfRenderWindow_drawText(_renderWindow, _gameData->hud.maxScoreText, NULL);
	}
	if (_gameData->gameState != MENU)
	{
		sfRenderWindow_drawSprite(_renderWindow, _gameData->hud.timeContainer, NULL);
		sfRenderWindow_drawSprite(_renderWindow, _gameData->hud.timeBar, NULL);
		sfRenderWindow_drawText(_renderWindow, _gameData->hud.scoreText, NULL);
	}
	if (_gameData->isDebug)
	{
		sfRenderWindow_drawText(_renderWindow, _gameData->hud.fpsText, NULL);
	}
	sfRenderWindow_display(_renderWindow);
}

void Cleanup(MainData* const _mainData, GameData* const _gameData)
{
	CleanupPlayer(&_gameData->game.player);
	CleanupHud(&_gameData->hud);
	CleanupLevel(&_gameData->game.level);

	sfRenderWindow_destroy(_mainData->renderWindow);
	_mainData->renderWindow = NULL;

	sfClock_destroy(_mainData->clock);
	_mainData->clock = NULL;
}

#pragma endregion

void Reset(GameData* const _gameData)
{
	_gameData->game.isGameStarted = sfFalse;
	_gameData->game.lifeTime = 5;
	_gameData->game.score = 0;
	_gameData->gameState = MENU;
	Level* level = &_gameData->game.level;
	AsigneTruncTexture(&level->trunc1, rand() % 2, &level->texture);
	AsigneTruncTexture(&level->trunc2, rand() % 2, &level->texture);
	AsigneTruncTexture(&level->trunc3, rand() % 2, &level->texture);
	AsigneTruncTexture(&level->trunc4, rand() % 2, &level->texture);
	AsigneTruncTexture(&level->trunc5, rand() % 2, &level->texture);
	AsigneTruncTexture(&level->trunc6, rand() % 2, &level->texture);
	_gameData->game.player.dir = BASE_POSITION;
	_gameData->game.player.dead = sfFalse;
}

void CreateSprite(sfSprite** const _sprite, sfVector2f position, const char* _filepath)
{
	*_sprite = sfSprite_create();
	sfSprite_setPosition(*_sprite, position);

	sfTexture* texture = sfTexture_createFromFile(_filepath, NULL);
	sfSprite_setTexture(*_sprite, texture, sfTrue);
}

void UpdateText(sfText* const _text, int _value)
{
	// Copy the value into the text
	char string[5];
	sprintf_s(string, sizeof(string), "%d", _value);
	sfText_setString(_text, string);
}
#pragma region Animation
void SetupAnimation(Animation* _anim, sfTexture** const _texture, int _frameCount, float _frameRate, sfBool _isLooping)
{
	// Charger la texture
	//printf("%c\n", _filepath);
	_anim->sprite = sfSprite_create();
	sfSprite_setTexture(_anim->sprite, *_texture, sfTrue);

	// D�finir les attributs de l'animation
	_anim->frameCount = _frameCount;
	_anim->frameRate = 1 / _frameRate;
	_anim->currentFrame = 0;
	_anim->isLooping = _isLooping;
	_anim->isFinished = sfFalse;

	// Calculer la taille de la texture pour d�couper les frames
	sfIntRect rect = { 0 };
	sfVector2u textureSize = sfTexture_getSize(*_texture);

	int frameWidth = textureSize.x / _frameCount;
	int frameHeight = textureSize.y;

	// Initialiser le rectangle de la texture pour le sprite
	rect.top = 0;
	rect.left = 0;
	rect.width = frameWidth;
	rect.height = frameHeight;

	sfSprite_setTextureRect(_anim->sprite, rect);

	sfVector2f origin = { frameWidth / 2.0f, (float)frameHeight };
	sfSprite_setOrigin(_anim->sprite, origin);
}

void AnimateSprite(Animation* _anim, float const _dt)
{
	if (_anim != NULL)
	{
		sfIntRect area = sfSprite_getTextureRect(_anim->sprite);
		sfVector2u textureSize = sfTexture_getSize(sfSprite_getTexture(_anim->sprite));
		int frameWidth = textureSize.x / _anim->frameCount;

		if (!_anim->isFinished) {
			area.left += frameWidth;

			if (area.left >= _anim->frameCount * frameWidth) {
				if (_anim->isLooping) {
					area.left = 0;
				}
				else {
					area.left = 0;
					_anim->isFinished = sfTrue;
				}
			}

			sfSprite_setTextureRect(_anim->sprite, area);
		}
	}
}

sfBool AnimIsFinished(Animation* const _anim)
{
	return _anim->isFinished;
}

void cleanupAnimation(Animation* animation)
{
	if (animation->texture) {
		sfTexture_destroy(animation->texture);
		animation->texture = NULL;
	}
	if (animation->sprite) {
		sfSprite_destroy(animation->sprite);
		animation->sprite = NULL;
	}
}

#pragma endregion

void LoadScreen(MainData* const _mainData)
{
	sfVideoMode videoMode = { SCREEN_WIDTH, SCREEN_HEIGHT, BPP };
	_mainData->renderWindow = sfRenderWindow_create(videoMode, SCREEN_NAME, sfDefaultStyle, NULL);
	sfRenderWindow_setVerticalSyncEnabled(_mainData->renderWindow, sfFalse);
	sfRenderWindow_setFramerateLimit(_mainData->renderWindow, MAX_FPS);
}

void LoadHud(HUD* const _hud)
{
	_hud->font = sfFont_createFromFile("Assets/Fonts/arial.ttf");
	_hud->fpsText = sfText_create();
	sfText_setCharacterSize(_hud->fpsText, 25);
	sfText_setFont(_hud->fpsText, _hud->font);
	sfVector2f textPosition = { 20, 20 };
	sfText_setPosition(_hud->fpsText, textPosition);

	sfVector2f nill = { 0, 0 };
	CreateSprite(&_hud->title, nill, "Assets/Sprites/Title.png");

	sfVector2u titleSize = sfTexture_getSize(sfSprite_getTexture(_hud->title));
	sfSprite_setOrigin(_hud->title, (sfVector2f) { (float)titleSize.x / 2, 0 });
	sfVector2f titlePosition = { SCREEN_WIDTH / 2,  (SCREEN_HEIGHT / 2) - (float) (titleSize.y / 2) };
	sfSprite_setPosition(_hud->title, titlePosition);

	CreateSprite(&_hud->button, nill, "Assets/Sprites/PlayButton.png");

	sfVector2u buttonSize = sfTexture_getSize(sfSprite_getTexture(_hud->button));
	sfSprite_setOrigin(_hud->button, (sfVector2f) { (float)buttonSize.x / 2, 0 });
	sfVector2f buttonPosition = { SCREEN_WIDTH / 2,  titlePosition.y + (titleSize.y) + buttonSize.y / 2 };
	sfSprite_setPosition(_hud->button, buttonPosition);

	CreateSprite(&_hud->gameOver, nill, "Assets/Sprites/GameOver.png");

	sfVector2u gameOverSize = sfTexture_getSize(sfSprite_getTexture(_hud->gameOver));
	sfSprite_setOrigin(_hud->gameOver, (sfVector2f) { (float)gameOverSize.x / 2, 0 });
	sfVector2f gameOverPosition = { SCREEN_WIDTH / 2,  0 };
	sfSprite_setPosition(_hud->gameOver, gameOverPosition);

	_hud->isColiding = sfFalse;

	CreateSprite(&_hud->timeContainer, nill, "Assets/Sprites/TimeContainer.png");

	sfVector2u timeContainerSize = sfTexture_getSize(sfSprite_getTexture(_hud->timeContainer));
	sfSprite_setOrigin(_hud->timeContainer, (sfVector2f) { (float)timeContainerSize.x / 2, 0 });
	sfVector2f timeContainerPosition = { SCREEN_WIDTH / 2,  SCREEN_HEIGHT * 0.05f };
	sfSprite_setPosition(_hud->timeContainer, timeContainerPosition);


	CreateSprite(&_hud->timeBar, nill, "Assets/Sprites/TimeBar.png");

	sfVector2u timeBarSize = sfTexture_getSize(sfSprite_getTexture(_hud->timeBar));
	sfSprite_setOrigin(_hud->timeBar, (sfVector2f) { (float)timeBarSize.x / 2, 0 });
	sfVector2f timeBarPosition = { SCREEN_WIDTH / 2, SCREEN_HEIGHT * 0.04f + timeBarSize.y / 2 };
	sfSprite_setPosition(_hud->timeBar, timeBarPosition);

	_hud->fontScore = sfFont_createFromFile("Assets/Fonts/KomikaParch.ttf");
	_hud->scoreText = sfText_create();
	sfText_setCharacterSize(_hud->scoreText, 35);
	sfText_setFont(_hud->scoreText, _hud->fontScore);
	sfFloatRect scoreSize = sfText_getLocalBounds(_hud->scoreText);
	sfText_setOrigin(_hud->scoreText, (sfVector2f) { (float)scoreSize.width / 2, scoreSize.height / 2 });

	sfVector2f textScorePosition = { SCREEN_WIDTH / 2 , SCREEN_HEIGHT * 0.13f };
	sfText_setPosition(_hud->scoreText, textScorePosition);

	_hud->maxScoreText = sfText_create();
	sfText_setCharacterSize(_hud->maxScoreText, 35);
	sfText_setFont(_hud->maxScoreText, _hud->fontScore);

	sfFloatRect gameOverBounds = sfSprite_getGlobalBounds(_hud->gameOver);
	sfFloatRect maxScoreBounds = sfText_getLocalBounds(_hud->maxScoreText);


	sfVector2f maxScorePosition = { SCREEN_WIDTH / 2, gameOverBounds.top + gameOverBounds.height / 1.5f };

	sfText_setOrigin(_hud->maxScoreText, (sfVector2f) { maxScoreBounds.width / 2, maxScoreBounds.height / 2 });
	sfText_setPosition(_hud->maxScoreText, maxScorePosition);

}
void UpdateHud(float const _dt, GameData* const _gameData)
{
	HUD* const hud = &_gameData->hud;
	Game* const game = &_gameData->game;
	GameState const gameState = _gameData->gameState;
	if (_gameData->isDebug)
	{
		char buffer[11];
		snprintf(buffer, sizeof(buffer), "Fps: %.2f", 1 / _dt);
		sfText_setString(hud->fpsText, buffer);
	}

	UpdateText(hud->scoreText, game->score);
	UpdateText(hud->maxScoreText, game->maxScore);

	if (gameState == GAME_OVER)
	{
		sfFloatRect scoreSize = sfText_getLocalBounds(hud->scoreText);
		sfText_setOrigin(hud->scoreText, (sfVector2f) { (float)scoreSize.width / 2, scoreSize.height / 2 });
		sfVector2f scorePosition = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2.1f };
		sfText_setPosition(hud->scoreText, scorePosition);

		sfFloatRect maxScoreBounds = sfText_getLocalBounds(hud->maxScoreText);
		sfText_setOrigin(hud->maxScoreText, (sfVector2f) { maxScoreBounds.width / 2, maxScoreBounds.height / 2 });
	}
	else
	{
		sfFloatRect scoreSize = sfText_getLocalBounds(hud->scoreText);
		sfText_setOrigin(hud->scoreText, (sfVector2f) { (float)scoreSize.width / 2, scoreSize.height / 2 });
		sfVector2f textScorePosition = { SCREEN_WIDTH / 2 , SCREEN_HEIGHT * 0.15f };
		sfText_setPosition(hud->scoreText, textScorePosition);

	}
}

void CleanupHud(HUD* const _hud)
{
	if (_hud->title) {
		sfSprite_destroy(_hud->title);
		_hud->title = NULL;
	}
	if (_hud->gameOver) {
		sfSprite_destroy(_hud->gameOver);
		_hud->gameOver = NULL;
	}
	if (_hud->timeContainer) {
		sfSprite_destroy(_hud->timeContainer);
		_hud->timeContainer = NULL;
	}
	if (_hud->timeBar) {
		sfSprite_destroy(_hud->timeBar);
		_hud->timeBar = NULL;
	}
	if (_hud->scoreText) {
		sfText_destroy(_hud->scoreText);
		_hud->scoreText = NULL;
	}
	if (_hud->fpsText) {
		sfText_destroy(_hud->fpsText);
		_hud->fpsText = NULL;
	}
	if (_hud->maxScoreText) {
		sfText_destroy(_hud->maxScoreText);
		_hud->maxScoreText = NULL;
	}
}

#pragma region Menu

void UpdateButton(float _dt, sfRenderWindow* const _renderWindow, GameData* const _gameData)
{
	HUD* hud = &_gameData->hud;
	GameState* gameState = &_gameData->gameState;
	sfBool* isStarted = &_gameData->game.isGameStarted;

	sfVector2i mouse = sfMouse_getPositionRenderWindow(_renderWindow);
	sfVector2i mousePos = { mouse.x, mouse.y };

	sfFloatRect rect = sfSprite_getGlobalBounds(hud->button);
	if (sfFloatRect_contains(&rect, (float) mousePos.x, (float) mousePos.y))
	{
		if (sfMouse_isButtonPressed(sfMouseLeft))
		{
			if (*gameState == MENU)
			{
				*gameState = GAME;
				*isStarted = sfTrue;
				sfMusic_play(_gameData->game.level.music);
			}
			else if (*gameState == GAME_OVER)
			{
				Reset(_gameData);
			}
		}
		hud->isColiding = sfTrue;
	}
	else
	{
		hud->isColiding = sfFalse;
	}
}

void DrawButton(sfRenderWindow* const _renderWindow, HUD* const _hud)
{
	if (_hud->isColiding)
	{
		sfSprite_setColor(_hud->button, sfColor_fromRGB(255, 255, 255));
		sfRenderWindow_drawSprite(_renderWindow, _hud->button, NULL);
	}
	else
	{
		sfSprite_setColor(_hud->button, sfColor_fromRGB(180, 180, 180));
		sfRenderWindow_drawSprite(_renderWindow, _hud->button, NULL);
	}

}

#pragma endregion

#pragma region Game
void LoadGame(Game* const _game)
{
	LoadLevel(&_game->level);
	LoadPlayer(&_game->player);
	_game->isGameStarted = sfFalse;
	_game->lifeTime = 5;
	_game->score = 0;
	_game->maxScore = 0;
}

void GameOnKeyPressed(sfKeyEvent _key, Game* const _game)
{
	switch (_key.code)
	{
	case sfKeyQ:
		if (!_game->player.dead && !_game->player.isCutting)
		{
			_game->player.isCutting = sfTrue;
			_game->player.dir = -1;
			CheckPlayerCollide(&_game->level, &_game->player);
			if (!_game->player.dead)
			{
				_game->lifeTime += 0.2f;
				_game->score++;
				UpdateTruncTexture(&_game->level);
				sfSound_setBuffer(_game->player.soundPlay, _game->player.soundBufferCutting);
				sfSound_play(_game->player.soundPlay);
			}
			CheckPlayerCollide(&_game->level, &_game->player);
		}
		break;
	case sfKeyLeft:
		if (!_game->player.dead && !_game->player.isCutting)
		{
			_game->player.isCutting = sfTrue;
			_game->player.dir = -1;
			CheckPlayerCollide(&_game->level, &_game->player);
			if (!_game->player.dead)
			{
				_game->lifeTime += 0.2f;
				_game->score++;
				UpdateTruncTexture(&_game->level);
				sfSound_setBuffer(_game->player.soundPlay, _game->player.soundBufferCutting);
				sfSound_play(_game->player.soundPlay);
			}
			CheckPlayerCollide(&_game->level, &_game->player);
		}
		break;
	case sfKeyRight:
		if (!_game->player.dead && !_game->player.isCutting)
		{
			_game->player.isCutting = sfTrue;
			_game->player.dir = 1;
			CheckPlayerCollide(&_game->level, &_game->player);
			if (!_game->player.dead)
			{
				_game->lifeTime += 0.2f;
				_game->score++;
				UpdateTruncTexture(&_game->level);
				sfSound_setBuffer(_game->player.soundPlay, _game->player.soundBufferCutting);
				sfSound_play(_game->player.soundPlay);
			}
			CheckPlayerCollide(&_game->level, &_game->player);
		}
	case sfKeyD:
		if (!_game->player.dead && !_game->player.isCutting)
		{
			_game->player.isCutting = sfTrue;
			_game->player.dir = 1;
			CheckPlayerCollide(&_game->level, &_game->player);
			if (!_game->player.dead)
			{
				_game->lifeTime += 0.2f;
				_game->score++;
				UpdateTruncTexture(&_game->level);
				sfSound_setBuffer(_game->player.soundPlay, _game->player.soundBufferCutting);
				sfSound_play(_game->player.soundPlay);
			}
			CheckPlayerCollide(&_game->level, &_game->player);
		}
		break;
	default:
		_game->isGameStarted = sfTrue;
		break;
	}
}

void UpdateGame(float _dt, Game* const _game, HUD* const _hud, GameState _gameState)
{
	PlayerUpdateAnimation(_dt, &_game->player);
	if (_game->maxScore < _game->score)
	{
		_game->maxScore = _game->score;
	}
	if (_game->lifeTime == 0)
	{
		_game->player.dead = sfTrue;
	}

	if (_gameState == GAME_OVER)
	{
		_game->player.dead;
		sfMusic_stop(_game->level.music);
	}

	if (!_game->player.dead)
	{
		UpdateLifeBar(_dt, _game, _hud, _game->isGameStarted);
	}
	PlayerUpdateMovement(&_game->player);
}

#pragma region Level
void LoadLevel(Level* const _level)
{
	sfVector2f backgroundPosition = { 0, 0 };
	CreateSprite(&_level->background, backgroundPosition, "Assets/Sprites/Background.png");

	sfVector2f nill = { 0, 0 };
	CreateSprite(&_level->baseLog, nill, "Assets/Sprites/Stump.png");
	sfFloatRect baseLogSize = sfSprite_getGlobalBounds(_level->baseLog);
	sfSprite_setOrigin(_level->baseLog, (sfVector2f) { (float)baseLogSize.width / 2, baseLogSize.height });
	sfVector2f baseLogPosition = { SCREEN_WIDTH / 2, GROUND };
	sfSprite_setPosition(_level->baseLog, baseLogPosition);

	_level->texture.trunc1 = sfTexture_createFromFile("Assets/Sprites/Trunk1.png", NULL);
	_level->texture.trunc2 = sfTexture_createFromFile("Assets/Sprites/Trunk2.png", NULL);
	_level->texture.branchLeft = sfTexture_createFromFile("Assets/Sprites/BranchLeft.png", NULL);
	_level->texture.branchRight = sfTexture_createFromFile("Assets/Sprites/BranchRight.png", NULL);

	sfVector2u truncSize = sfTexture_getSize(_level->texture.trunc1);
	CreateTrunc(&_level->trunc1, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });
	CreateTrunc(&_level->trunc2, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });
	CreateTrunc(&_level->trunc3, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });
	CreateTrunc(&_level->trunc4, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });
	CreateTrunc(&_level->trunc5, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });
	CreateTrunc(&_level->trunc6, (sfVector2f) { (float)truncSize.x, (float)truncSize.y });

	sfSprite_setOrigin(_level->trunc1, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });
	sfSprite_setOrigin(_level->trunc2, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });
	sfSprite_setOrigin(_level->trunc3, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });
	sfSprite_setOrigin(_level->trunc4, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });
	sfSprite_setOrigin(_level->trunc5, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });
	sfSprite_setOrigin(_level->trunc6, (sfVector2f) { (float)truncSize.x / 2, truncSize.y });

	AsigneTruncTexture(&_level->trunc1, rand() % 2, &_level->texture);
	AsigneTruncTexture(&_level->trunc2, rand() % 2, &_level->texture);
	AsigneTruncTexture(&_level->trunc3, rand() % 2, &_level->texture);
	AsigneTruncTexture(&_level->trunc4, rand() % 2, &_level->texture);
	AsigneTruncTexture(&_level->trunc5, rand() % 2, &_level->texture);
	AsigneTruncTexture(&_level->trunc6, rand() % 2, &_level->texture);

	sfFloatRect baseLog = sfSprite_getGlobalBounds(_level->baseLog);
	sfVector2f currentPosition = { SCREEN_WIDTH / 2, baseLog.top };

	sfSprite_setPosition(_level->trunc1, currentPosition);

	currentPosition.y -= truncSize.y;
	sfSprite_setPosition(_level->trunc2, currentPosition);

	currentPosition.y -= truncSize.y;
	sfSprite_setPosition(_level->trunc3, currentPosition);

	currentPosition.y -= truncSize.y;
	sfSprite_setPosition(_level->trunc4, currentPosition);

	currentPosition.y -= truncSize.y;
	sfSprite_setPosition(_level->trunc5, currentPosition);

	currentPosition.y -= truncSize.y;
	sfSprite_setPosition(_level->trunc6, currentPosition);


	_level->music = sfMusic_createFromFile("Assets/Musics/Theme.ogg");
	sfMusic_setVolume(_level->music, 40);
	sfMusic_play(_level->music);
}

void DrawLevel(sfRenderWindow* const _renderWindow, Level* const _level)
{
	sfRenderWindow_drawSprite(_renderWindow, _level->background, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->baseLog, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc1, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc2, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc3, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc4, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc5, NULL);
	sfRenderWindow_drawSprite(_renderWindow, _level->trunc6, NULL);
}

void CleanupLevel(Level* const _level)
{
	sfSprite_destroy(_level->background);
	_level->background = NULL;

	sfSprite_destroy(_level->baseLog);
	_level->baseLog = NULL;

	sfSprite_destroy(_level->trunc1);
	_level->trunc1 = NULL;

	sfSprite_destroy(_level->trunc2);
	_level->trunc2 = NULL;

	sfSprite_destroy(_level->trunc3);
	_level->trunc3 = NULL;

	sfSprite_destroy(_level->trunc4);
	_level->trunc4 = NULL;

	sfSprite_destroy(_level->trunc5);
	_level->trunc5 = NULL;

	sfSprite_destroy(_level->trunc6);
	_level->trunc6 = NULL;


	sfTexture_destroy(_level->texture.trunc1);
	_level->texture.trunc1 = NULL;

	sfTexture_destroy(_level->texture.trunc2);
	_level->texture.trunc2 = NULL;

	sfTexture_destroy(_level->texture.branchLeft);
	_level->texture.branchLeft = NULL;

	sfTexture_destroy(_level->texture.branchRight);
	_level->texture.branchRight = NULL;

	sfMusic_stop(_level->music);
	sfMusic_destroy(_level->music);
	_level->music = NULL;
}

void CheckPlayerCollide(Level* const _level, Player* const _player)
{
	const sfTexture* originalTexture;

	originalTexture = sfSprite_getTexture(_level->trunc1);
	if (originalTexture != NULL)
	{
		if (originalTexture == _level->texture.branchLeft)
		{
			if (_player->dir == -1)
			{
				_player->dead = sfTrue;
			}
		}
		else if (originalTexture == _level->texture.branchRight)
		{
			if (_player->dir == 1)
			{
				_player->dead = sfTrue;
			}
		}
	}

	if (_player->dead)
	{
		sfSound_setBuffer(_player->soundPlay, _player->soundBufferDeath);
		sfSound_play(_player->soundPlay);
	}
}

void UpdateLifeBar(float _dt, Game* const _game, HUD* const _hud, sfBool _isStarted)
{
	if (_isStarted)
	{
		_game->lifeTime -= _dt;
	}

	if (_game->lifeTime < 0)
	{
		_game->lifeTime = 0;
	}
	else if (_game->lifeTime > MAX_LIFE_TIME)
	{
		_game->lifeTime = MAX_LIFE_TIME;
	}
	sfIntRect area = sfSprite_getTextureRect(_hud->timeBar);
	sfVector2u size = sfTexture_getSize(sfSprite_getTexture(_hud->timeBar));

	area.width = size.x * (_game->lifeTime / (float)MAX_LIFE_TIME);

	sfSprite_setTextureRect(_hud->timeBar, area);
}

void CreateTrunc(sfSprite** const _sprite, sfVector2f position)
{
	*_sprite = sfSprite_create();
	sfSprite_setPosition(*_sprite, position);
}

void AsigneTruncTexture(sfSprite** const _sprite, TruncType _truncType, TrunKTexture* _texture)
{
	switch (_truncType)
	{
	case NORMAL:
		sfSprite_setTexture(*_sprite, _texture->trunc1, sfTrue);
		break;
	case NORMAL2:
		sfSprite_setTexture(*_sprite, _texture->trunc2, sfTrue);
		break;
	case LEFT:
		sfSprite_setTexture(*_sprite, _texture->branchLeft, sfTrue);
		break;
	case RIGHT:
		sfSprite_setTexture(*_sprite, _texture->branchRight, sfTrue);
		break;
	default:
		break;
	}
}

void UpdateTruncTexture(Level* const _level)
{
	const sfTexture* originalTexture;

	// Trunc2 -> Trunc1
	originalTexture = sfSprite_getTexture(_level->trunc2);
	if (originalTexture != NULL)
	{
		sfSprite_setTexture(_level->trunc1, originalTexture, sfTrue);
	}

	originalTexture = sfSprite_getTexture(_level->trunc3);
	if (originalTexture != NULL)
	{
		sfSprite_setTexture(_level->trunc2, originalTexture, sfTrue);
	}

	originalTexture = sfSprite_getTexture(_level->trunc4);
	if (originalTexture != NULL)
	{
		sfSprite_setTexture(_level->trunc3, originalTexture, sfTrue);
	}

	originalTexture = sfSprite_getTexture(_level->trunc5);
	if (originalTexture != NULL)
	{
		sfSprite_setTexture(_level->trunc4, originalTexture, sfTrue);
	}

	originalTexture = sfSprite_getTexture(_level->trunc6);
	if (originalTexture != NULL)
	{
		sfSprite_setTexture(_level->trunc5, originalTexture, sfTrue);
	}

	if (originalTexture == _level->texture.branchLeft || originalTexture == _level->texture.branchRight)
	{
		AsigneTruncTexture(&_level->trunc6, rand() % 2, &_level->texture);
	}
	else
	{
		AsigneTruncTexture(&_level->trunc6, rand() % 4, &_level->texture);
	}
}

#pragma endregion

#pragma region Player

void LoadPlayer(Player* const _player)
{
	_player->dir = BASE_POSITION;
	_player->animationTime = 0;
	LoadPlayerAnimations(_player);
	_player->animation.currentAnim = &_player->animation.idle;

	sfFloatRect box = sfSprite_getGlobalBounds(_player->animation.currentAnim->sprite);
	sfVector2f position = { SCREEN_WIDTH - box.width / 2 , GROUND };
	sfSprite_setPosition(_player->animation.currentAnim->sprite, position);
	sfSprite_setScale(_player->animation.currentAnim->sprite, (sfVector2f) { -1, 1 });

	_player->soundBufferCutting = sfSoundBuffer_createFromFile("Assets/Sounds/Cut.ogg");
	_player->soundBufferDeath = sfSoundBuffer_createFromFile("Assets/Sounds/Death.ogg");

	_player->soundPlay = sfSound_create();
}

void LoadPlayerAnimations(Player* const _player)
{
	sfTexture* idle = sfTexture_createFromFile("Assets/Sprites/ManIdle.png", NULL);
	sfTexture* cut = sfTexture_createFromFile("Assets/Sprites/ManWoodcutting.png", NULL);
	sfTexture* dead = sfTexture_createFromFile("Assets/Sprites/RIP.png", NULL);
	SetupAnimation(&_player->animation.idle, &idle, 2, 4, sfTrue);
	SetupAnimation(&_player->animation.woodcutting, &cut, 2, 20, sfFalse);
	SetupAnimation(&_player->animation.dead, &dead, 1, 1, sfFalse);
}

void PlayerUpdateMovement(Player* const _player)
{
	sfFloatRect box = sfSprite_getGlobalBounds(_player->animation.currentAnim->sprite);
	if (!_player->dead)
	{
		if (_player->dir == 1)
		{
			sfVector2f position = { SCREEN_WIDTH - box.width / 2 , GROUND };
			sfSprite_setPosition(_player->animation.currentAnim->sprite, position);
			sfSprite_setScale(_player->animation.currentAnim->sprite, (sfVector2f) { -1, 1 });
		}
		else
		{
			sfVector2f position = { 0 + box.width / 2, GROUND };
			sfSprite_setPosition(_player->animation.currentAnim->sprite, position);
			sfSprite_setScale(_player->animation.currentAnim->sprite, (sfVector2f) { 1, 1 });
		}

	}
	else
	{
		if (_player->dir == 1)
		{
			sfVector2f position = { SCREEN_WIDTH * 1.1f - box.height, GROUND };
			sfSprite_setPosition(_player->animation.currentAnim->sprite, position);
		}
		else
		{
			sfVector2f position = { SCREEN_WIDTH / 2.5f - box.height, GROUND };
			sfSprite_setPosition(_player->animation.currentAnim->sprite, position);
		}
	}
}

void PlayerUpdateAnimation(float _dt, Player* const _player)
{
	if (!_player->dead)
	{
		if (_player->isCutting)
		{
			_player->animation.currentAnim = &_player->animation.woodcutting;
			if (AnimIsFinished(_player->animation.currentAnim))
			{
				_player->isCutting = sfFalse;
				_player->animation.woodcutting.currentFrame = 0;
				_player->animation.woodcutting.isFinished = sfFalse;
			}
		}
		else
		{
			_player->animation.currentAnim = &_player->animation.idle;
		}
	}
	else
	{
		_player->animation.currentAnim = &_player->animation.dead;
	}

	_player->animationTime += _dt;
	if (_player->animationTime >= _player->animation.currentAnim->frameRate)
	{
		AnimateSprite(_player->animation.currentAnim, _dt);
		_player->animationTime -= _player->animation.currentAnim->frameRate;
	}
}

void DrawPlayer(sfRenderWindow* const _renderWindow, Player* const _player)
{
	sfRenderWindow_drawSprite(_renderWindow, _player->animation.currentAnim->sprite, NULL);
}

void CleanupPlayer(Player* const _player)
{
	if (!_player)
	{
		return;
	}

	// Nettoyer toutes les animations
	cleanupAnimation(&_player->animation.idle);
	cleanupAnimation(&_player->animation.woodcutting);
	cleanupAnimation(&_player->animation.dead);

	sfSoundBuffer_destroy(_player->soundBufferCutting);
	_player->soundBufferCutting = NULL;

	sfSoundBuffer_destroy(_player->soundBufferDeath);
	_player->soundBufferDeath = NULL;

	sfSound_destroy(_player->soundPlay);
	_player->soundPlay = NULL;

}
#pragma endregion
#pragma endregion
