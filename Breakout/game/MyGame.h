#pragma once

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSprite player;	// player sprite
	CSprite startScreen;
    CSprite background;
	CSpriteList platformList; // platforms and obstacles are kept in a sprite list
	CSpriteList enemies; //enemy sprites
	CSpriteList bosses; //level 2 enemy sprites
	CSpriteList explosions; //level 2 enemy sprites

	CSoundPlayer SwordSwing;
	CSoundPlayer Explode;
	CSoundPlayer Background;
	CSoundPlayer Death;

	int level;
	int castTime;
	int score;
	

	// my own member functions
void PlayerControl();
void EnemyControl();
void BossControl();

void SetupLevel1();
void SetupLevel2();

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
