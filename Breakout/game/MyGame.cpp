#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	startScreen(400,300,"startScreen.bmp",0)
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	// do not run game logic when in menu mode
	if (IsMenuMode()) return;

	Uint32 t = GetTime();

	// TODO: add the game update code here
	player.Update(t);	

	for (CSprite* pSprite : platformList) pSprite->Update(t);

	for (CSprite* pSprite : enemies) pSprite->Update(t);

	for (CSprite* pSprite : bosses) pSprite->Update(t);

	for (CSprite* pSprite : explosions) pSprite->Update(t);
    
	if (player.GetY() < -70) GameOver();
	if (player.GetX() > 1200 && player.GetY() < 300) GameOver();
	PlayerControl();
	EnemyControl();
	if (level == 2) BossControl();
}


void CMyGame::OnDraw(CGraphics* g)
{
	int leftScreenLimit = 200;
	int rightScreenLimit = 3000;
	int topScreenLimit = 2000;
	int bottomScreenLimit = 0;
	
	// draw menu when in menu mode
	if (IsMenuMode())
	{
		startScreen.Draw(g);
		*g << font(28) << color(CColor::Red()) << xy(10, 570) << "Level: " << level;
		return;
	}

	if (level == 1)
	{
		// we scroll the whole game world according to the player position
		if (player.GetX() >= leftScreenLimit && player.GetX() <= rightScreenLimit && player.GetY() >= bottomScreenLimit && player.GetY() <= topScreenLimit)
		{
			g->SetScrollPos(400 - player.GetX(), 300 - player.GetY());
		}
		// we stop scrolling with the player once we have reached the right limit
		if (player.GetX() > rightScreenLimit)
		{
			g->SetScrollPos(rightScreenLimit, 300 - player.GetY());
		}
		if (player.GetX() < leftScreenLimit)
		{
			g->SetScrollPos(leftScreenLimit, 300 - player.GetY());
		}
		if (player.GetY() > topScreenLimit)
		{
			g->SetScrollPos(400 - player.GetX(), -topScreenLimit + bottomScreenLimit);
		}
		if (player.GetY() < bottomScreenLimit)
		{
			g->SetScrollPos(400 - player.GetX(), bottomScreenLimit);
		}
	}

	if (level == 2)
	{
		g->SetScrollPos(0, 0);
	}

	
	for (CSprite* pSprite : platformList) pSprite->Draw(g);
	background.Draw(g);
	

	for (CSprite* pSprite : enemies) pSprite->Draw(g);

	for (CSprite* pSprite : bosses) pSprite->Draw(g);

	for (CSprite* pSprite : explosions) pSprite->Draw(g);

	player.Draw(g);

	
	  
	 // don't scroll the overlay screen
	 g->SetScrollPos(0, 0);

	// *g << font(28) << color(CColor::Red()) << xy(10, 570) << "player status: " << player.GetStatus();

	 for (CSprite* boss : bosses)
	 {
		 //*g << font(28) << color(CColor::Red()) << xy(10, 550) << "enemy status: " << boss->GetStatus();
		// *g << font(28) << color(CColor::Red()) << xy(10, 530) << "enemy X: " << boss->GetX();
		// *g << font(28) << color(CColor::Red()) << xy(10, 510) << "enemy Y: " << boss->GetY();
	 }

	 *g << font(28) << color(CColor::Red()) << xy(10, 550) << "Health: " << player.GetHealth();

	 if (IsGameOverMode())  *g << font(40) << color(CColor::Green()) << xy(400, 300) << "Game Over";
	
}

void CMyGame::PlayerControl()
{
	float jumpSpeed = 440; // jumpin up speed
	float slowDown = 15; // speed reduction when jumping
	float walkSpeed = 225; // horizontal walking speed

	if (level == 1 && player.GetX() < 215 && player.GetY() > 900)
	{
		level = 2;
		SetupLevel2();
	}
	

	CVector oldPos = player.GetPosition();
	
	enum { NONE, STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, JUMPLEFT, JUMPRIGHT, DOUBLEJUMPLEFT, DOUBLEJUMPRIGHT, JUMPATTACKLEFT,
		JUMPATTACKRIGHT, ATTACKLEFT, ATTACKRIGHT} p_state;
	/* 
	  player status variable is used to indicate which state the player is in

	  1: standing left looking
	  2: standing right looking
	  3: walking left
	  4: walking right
	  5: jumping/falling left
	  6: jumping/falling right
	  7: double jumping left
	  8: double jumping right
	  9: jumping/falling attacking left
	  10: jumping/falling attacking right
	  11: attacking right
	  12: attacking left
	 

	*/


	

	// gravity simulation 
	if (player.GetStatus() > 4 && player.GetStatus() < 11) player.SetYVelocity(player.GetYVelocity() - slowDown);

	// walking RIGHT
	if (IsKeyDown(SDLK_RIGHT) && player.GetStatus() != WALKRIGHT && player.GetStatus() < WALKRIGHT)
	{
		player.SetMotion( walkSpeed, 0);
		player.SetAnimation("walkright", 10);
		player.SetStatus( WALKRIGHT);
		player.Update(GetTime());
		
	}
	// standing right
	else if((!IsKeyDown(SDLK_RIGHT) && player.GetStatus() == WALKRIGHT) || (!IsKeyDown(SDLK_LSHIFT) && player.GetStatus() == ATTACKRIGHT))
	{
		player.SetMotion(0, 0);
		player.SetAnimation("standright", 3);
		player.SetStatus(STANDRIGHT);
		player.Update(GetTime());
		
	}
	// walking LEFT
	if (IsKeyDown(SDLK_LEFT) && player.GetStatus() != WALKLEFT && player.GetStatus() < WALKLEFT)
	{
		player.SetMotion(-walkSpeed, 0);
		player.SetAnimation("walkleft", 10);
		player.SetStatus(WALKLEFT);
		player.Update(GetTime());
		
	}
	// standing left
	else if ((!IsKeyDown(SDLK_LEFT) && player.GetStatus() == WALKLEFT) || (!IsKeyDown(SDLK_LSHIFT) && player.GetStatus() == ATTACKLEFT))
	{
		player.SetMotion(0, 0);
		player.SetAnimation("standleft", 3);
		player.SetStatus(STANDLEFT);
		player.Update(GetTime());
	}

	//attacking 
	if (IsKeyDown(SDLK_LSHIFT) && player.GetStatus() != ATTACKLEFT && player.GetStatus() != ATTACKRIGHT)
	{
		if (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT || 
			player.GetStatus() == JUMPLEFT || player.GetStatus() == DOUBLEJUMPLEFT)
		{
			
			if (player.GetStatus() == JUMPLEFT || player.GetStatus() == DOUBLEJUMPLEFT)
			{
				player.SetAnimation("jumpattackleft", 14);
				player.SetStatus(JUMPATTACKLEFT);
				SwordSwing.Play("glaneur-de-sons__sword-swing-b-strong-02-.wav", -1);
				SwordSwing.Volume(50);
			}
			else
			{
				player.SetMotion(-5, 0);
				player.SetAnimation("attackleft", 14);
				player.SetStatus(ATTACKLEFT);
				SwordSwing.Play("glaneur-de-sons__sword-swing-b-strong-02-.wav", -1);
				SwordSwing.Volume(50);
			}
			player.Update(GetTime());
		}
		if (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT || 
			player.GetStatus() == JUMPRIGHT || player.GetStatus() == DOUBLEJUMPRIGHT)
		{

			if (player.GetStatus() == JUMPRIGHT || player.GetStatus() == DOUBLEJUMPRIGHT)
			{
				player.SetAnimation("jumpattackright", 14);
				player.SetStatus(JUMPATTACKRIGHT);
				SwordSwing.Play("glaneur-de-sons__sword-swing-b-strong-02-.wav", -1);
				SwordSwing.Volume(50);
			}
			else
			{
				player.SetMotion(10, 0);
				player.SetAnimation("attackright", 14);
				player.SetStatus(ATTACKRIGHT);
				SwordSwing.Play("glaneur-de-sons__sword-swing-b-strong-02-.wav", -1);
				SwordSwing.Volume(50);
			}
			player.Update(GetTime());
		}
	}
	
	// JUMPING
	if ((IsKeyDown(SDLK_LCTRL) && player.GetStatus() != JUMPRIGHT && player.GetStatus() != JUMPLEFT)
		|| (player.GetStatus() == JUMPATTACKRIGHT || player.GetStatus() == JUMPATTACKLEFT))
	{
		
		if (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT)
		{
			 player.SetYVelocity( jumpSpeed);
			 player.SetAnimation("jumpleft");
			 player.SetStatus(JUMPLEFT);
			 player.Update(GetTime());
			 
		}
		if (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT)
			
		{
			player.SetYVelocity( jumpSpeed);
			player.SetAnimation("jumpright");
			player.SetStatus(JUMPRIGHT);
			player.Update(GetTime());
		}

		if (!IsKeyDown(SDLK_LSHIFT) && player.GetStatus() == JUMPATTACKLEFT)
		{
			player.SetAnimation("jumpleft");
			player.SetStatus(JUMPLEFT);
			player.Update(GetTime());
		}
		if (!IsKeyDown(SDLK_LSHIFT) && player.GetStatus() == JUMPATTACKRIGHT)
		{player.SetAnimation("jumpright");
			player.SetStatus(JUMPRIGHT);
			player.Update(GetTime());
		}
	}

	if (IsKeyDown(SDLK_LALT) && (player.GetStatus() == JUMPRIGHT || player.GetStatus() == JUMPLEFT))
	{
		player.SetYVelocity(jumpSpeed / 2);
		if (player.GetStatus() == JUMPRIGHT && IsKeyDown(SDLK_RIGHT))
		{
			player.SetXVelocity(player.GetXVelocity() + 100);
			player.SetAnimation("doublejumpright");
			player.SetStatus(DOUBLEJUMPRIGHT);
		}
		else if (player.GetStatus() == JUMPRIGHT && IsKeyDown(SDLK_LEFT))
		{
			player.SetXVelocity((player.GetXVelocity() + 100) * -1);
			player.SetAnimation("doublejumpleft");
			player.SetStatus(DOUBLEJUMPLEFT);
		}
		else if (player.GetStatus() == JUMPRIGHT && !IsKeyDown(SDLK_LEFT) && !IsKeyDown(SDLK_RIGHT))
		{
			player.SetAnimation("doublejumpright");
			player.SetStatus(DOUBLEJUMPRIGHT);
		}

		if (player.GetStatus() == JUMPLEFT && IsKeyDown(SDLK_LEFT))
		{
			player.SetXVelocity(player.GetXVelocity() - 100);
			player.SetAnimation("doublejumpleft");
			player.SetStatus(DOUBLEJUMPLEFT);
		}
		else if (player.GetStatus() == JUMPLEFT && IsKeyDown(SDLK_RIGHT))
		{
			player.SetXVelocity((player.GetXVelocity() - 100) * -1);
			player.SetAnimation("doublejumpright");
			player.SetStatus(DOUBLEJUMPRIGHT);
		}
		else if (player.GetStatus() == JUMPLEFT && !IsKeyDown(SDLK_LEFT) && !IsKeyDown(SDLK_RIGHT))
		{
			player.SetAnimation("doublejumpleft");
			player.SetStatus(DOUBLEJUMPLEFT);
		}
		player.Update(GetTime());
	}

	if (player.GetStatus() < 9)
	{
		SwordSwing.Stop();
	}
	player.Update(GetTime());
	

	// ----- testing if we are on a platform -------
	bool onPlatform = false;
	player.SetY(player.GetY() - 5); // move a little bit down
	for (CSprite* pPlatform : platformList)
	{
		if (player.HitTest(pPlatform))
		{
			onPlatform = true;
			
		
		}
	}
	player.SetY(player.GetY() + 5);

	// ---- collision with platforms
	player.Proceed(5);
	for (CSprite* pPlatform : platformList)
	{
		if (player.HitTest(pPlatform))
		{
			// where did we hit the platform
			 // below platform
			if (player.GetY() < pPlatform->GetBottom())
			{
				player.SetY(player.GetY() - 5);
				player.SetYVelocity(0);
			}
			// above platform
			if (player.GetBottom() >= pPlatform->GetBottom())
			{
				onPlatform = true;
				if (player.GetStatus() == JUMPLEFT || player.GetStatus() == JUMPATTACKLEFT || player.GetStatus() == DOUBLEJUMPLEFT)
				{
					player.SetMotion(0, 0);
					player.SetAnimation("standleft", 3);
					player.SetStatus(STANDLEFT);
					player.Update(GetTime());
					player.SetY(pPlatform->GetTop() + player.GetHeight() / 2);
				}
				if (player.GetStatus() == JUMPRIGHT || player.GetStatus() == JUMPATTACKRIGHT || player.GetStatus() == DOUBLEJUMPRIGHT)
				{
					player.SetMotion(0, 0);
					player.SetAnimation("standright", 3);
					player.SetStatus(STANDRIGHT);
					player.Update(GetTime());
					player.SetY(pPlatform->GetTop() + player.GetHeight() / 2);
				}

				

			}
	        // left of platform;
			if (player.GetX() < pPlatform->GetLeft())
			{
				player.SetXVelocity(0);
				//player.SetYVelocity(0);
				player.SetPosition(oldPos);

			}
			// right of platform
			if (player.GetX() > pPlatform->GetRight())
			{
				player.SetXVelocity(0);
				//player.SetYVelocity(0);
				player.SetPosition(oldPos);

			}

		}
	}
	player.Proceed(-5);

	// if we are not on a platform then we should fall
	if (!onPlatform && (player.GetStatus() == STANDLEFT || player.GetStatus() == WALKLEFT || player.GetStatus() == ATTACKLEFT)) // no ground
	{
		player.SetStatus(JUMPLEFT); // make fall

	}
	// if we are not on a platform then we should fall
	if (!onPlatform && (player.GetStatus() == STANDRIGHT || player.GetStatus() == WALKRIGHT || player.GetStatus() == ATTACKRIGHT)) // no ground
	{
		player.SetStatus(JUMPRIGHT); // make fall
	}

	if (player.GetHealth() <= 0) GameOver();
	player.Update(GetTime());


}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisationF
void CMyGame::OnInitialize()
{   
	// loading the game map
	
	// configuring the animations for the player sprite
	player.AddImage("adventurer-Sheet copy.png", "standright", 7, 11, 0, 10, 3, 10, CColor::White());
	player.AddImage("adventurer-Sheet copy.png", "walkright", 7, 11, 1, 9, 5, 9, CColor::White());
	player.AddImage("adventurer-Sheet copy.png", "jumpright", 7, 11, 3, 8, 6, 8, CColor::White());
	player.AddImage("adventurer-Sheet2.png", "standleft", 7, 11, 3, 10, 6, 10, CColor::Black());
	player.AddImage("adventurer-Sheet2.png", "walkleft", 7, 11, 0, 9, 5, 9, CColor::Black());
	player.AddImage("adventurer-Sheet2.png", "jumpleft", 7, 11, 6, 8, 1, 8, CColor::Black());
	player.AddImage("adventurer-Sheet copy.png", "attackright", 7, 11, 0, 4, 3, 3, CColor::White());
	player.AddImage("adventurer-Sheet2.png", "attackleft", 7, 11, 6, 4, 3, 3, CColor::Black());
	player.AddImage("adventurer-v1.6-Sheet.bmp", "jumpattackright", 7, 16, 1, 1, 4, 1, CColor::White());
	player.AddImage("adventurer-v1.6-SheetLeft.bmp", "jumpattackleft", 7, 16, 5, 1, 2, 1, CColor::White());
	player.AddImage("adventurer-v1.6-Sheet.bmp", "doublejumpright", 7, 16, 1, 4, 0, 4, CColor::White());
	player.AddImage("adventurer-v1.6-SheetLeft.bmp", "doublejumpleft", 7, 16, 6, 4, 5, 4, CColor::White());
	
	player.SetAnimation("jumpright");
	player.SetHealth(50);
	player.SetStatus(6);

}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{   
	platformList.clear();
	enemies.clear();
	bosses.clear();
	explosions.clear();
	level = 1;


	

	//StartGame();
}


void CMyGame::SetupLevel1()
{
	background.LoadImage("Level1.bmp", "Level1", CColor::White());
	background.SetImage("Level1"); background.SetPosition(750, 560);
	level = 1;
	CSprite* newBandit = new CSprite(400, 105, 0, 0, GetTime());
	newBandit->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit->SetAnimation("standleft", 3);
	enemies.push_back(newBandit);

	CSprite* newBandit1 = new CSprite(700, 5, 0, 0, GetTime());
	newBandit1->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit1->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit1->SetAnimation("standleft", 3);
	enemies.push_back(newBandit1);


	CSprite* newBandit2 = new CSprite(500, 500, 0, 0, GetTime());
	newBandit2->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit2->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit2->SetAnimation("standleft", 3);
	enemies.push_back(newBandit2);

	CSprite* newBandit3 = new CSprite(500, 702, 0, 0, GetTime());
	newBandit3->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit3->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit3->SetAnimation("standleft", 3);
	enemies.push_back(newBandit3);

	CSprite* newBandit4 = new CSprite(620, 921, 0, 0, GetTime());
	newBandit4->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit4->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
	newBandit4->SetAnimation("standleft", 3);
	enemies.push_back(newBandit4);


	int type, x, y;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level1.txt", ios_base::in);

	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				CSprite* pSprite = new CSprite(x, y, "floor.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 2)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 3)
			{
				CSprite* pSprite = new CSprite(x, y, "obstacle.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 4)
			{
				CSprite* pSprite = new CSprite(x, y, "wall.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 5)
			{
				CSprite* pSprite = new CSprite(x, y, "vertplatform.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 6)
			{
				CSprite* pSprite = new CSprite(x, y, "vertplatform2.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 7)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2short.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 8)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2vertshort.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}



		}
	} while (neof);
	myfile.close();


}

// setting up a level 
void CMyGame::SetupLevel2()
{
	background.LoadImage("Level2 Background.bmp", "Level2", CColor::White());
	background.SetImage("Level2"); background.SetPosition(400, 300);
	platformList.clear();
	enemies.clear();
	player.SetPosition(690, 100);
	player.SetMotion(0, 0);
	player.SetHealth(player.GetHealth());

	level = 2;
	CSprite* newBoss = new CSprite(100, 100, 0, 0, GetTime());
	newBoss->AddImage("Lvl2 Enemy - Idle.bmp", "standright", 10, 1, 0, 0, 9, 0, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Idle(2).bmp", "standleft", 10, 1, 9, 0, 0, 0, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Fly.bmp", "flyright", 6, 1, 0, 0, 5, 0, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Fly(2).bmp", "flyleft", 6, 1, 5, 0, 0, 0, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Casting.bmp", "castright", 8, 2, 0, 1, 7, 0, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Casting(2).bmp", "castleft", 8, 2, 7, 0, 0, 1, CColor::White());
	newBoss->AddImage("Lvl2 Enemy - Death.bmp", "death", 6, 1, 0, 0, 5, 0, CColor::White());
	newBoss->SetAnimation("standright", 7);
	newBoss->SetState(1);
	//newBoss->SetMotion(50, 0);
	//newBoss->SetImage("standleft");
	bosses.push_back(newBoss);




	castTime = 0;


	int type, x, y;

	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level2.txt", ios_base::in);

	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				CSprite* pSprite = new CSprite(x, y, "floor.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 2)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 3)
			{
				CSprite* pSprite = new CSprite(x, y, "obstacle.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 4)
			{
				CSprite* pSprite = new CSprite(x, y, "wall.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 5)
			{
				CSprite* pSprite = new CSprite(x, y, "vertplatform.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

			if (type == 6)
			{
				CSprite* pSprite = new CSprite(x, y, "vertplatform2.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 7)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2short.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}
			if (type == 8)
			{
				CSprite* pSprite = new CSprite(x, y, "platform2vertshort.bmp", CColor::Black(), GetTime());
				platformList.push_back(pSprite);
			}

		}
	} while (neof);
	myfile.close();

	
}

void CMyGame::BossControl()
{
	int spawnenemy = rand() % 200;
	if (spawnenemy == 0)
	{
		CSprite* newBandit = new CSprite(float((rand() % 400) + 150), 100, 0, 0, GetTime());
		newBandit->AddImage("LightBandit_Spritesheet2.bmp", "standleft", 8, 5, 0, 4, 3, 4, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet2.bmp", "runleft", 8, 5, 0, 3, 7, 3, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet3.bmp", "standright", 8, 5, 4, 4, 7, 4, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet3.bmp", "runright", 8, 5, 7, 3, 0, 3, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet2.bmp", "attackleft", 8, 5, 0, 2, 7, 2, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet3.bmp", "attackright", 8, 5, 7, 2, 0, 2, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet3.bmp", "deadright", 8, 5, 0, 1, 7, 1, CColor::White());
		newBandit->AddImage("LightBandit_Spritesheet2.bmp", "deadleft", 8, 5, 0, 1, 7, 1, CColor::White());
		newBandit->SetAnimation("standleft", 3);
		enemies.push_back(newBandit);
	}

	int spawnboss = rand() % 300;
	if (spawnboss == 0)
	{
		CSprite* newBoss = new CSprite(100, 100, 0, 0, GetTime());
		newBoss->AddImage("Lvl2 Enemy - Idle.bmp", "standright", 10, 1, 0, 0, 9, 0, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Idle(2).bmp", "standleft", 10, 1, 9, 0, 0, 0, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Fly.bmp", "flyright", 6, 1, 0, 0, 5, 0, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Fly(2).bmp", "flyleft", 6, 1, 5, 0, 0, 0, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Casting.bmp", "castright", 8, 2, 0, 1, 7, 0, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Casting(2).bmp", "castleft", 8, 2, 7, 0, 0, 1, CColor::White());
		newBoss->AddImage("Lvl2 Enemy - Death.bmp", "death", 6, 1, 0, 0, 5, 0, CColor::White());
		newBoss->SetAnimation("standright", 7);
		newBoss->SetState(1);
		//newBoss->SetMotion(50, 0);
		//newBoss->SetImage("standleft");
		bosses.push_back(newBoss);

	}
	

	enum
	{
		STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, CASTRIGHT, CASTLEFT, DEAD
	} p_state;

	long t = GetTime();

	for (CSprite* boss : bosses)
	{
		if (player.HitTest(boss) && player.GetStatus() > 8)
		{
			boss->SetAnimation("death", 10);
			boss->SetStatus(DEAD);
			boss->Die(700);
			boss->Update(t);
			Death.Play("Monster Growl-.wav");
			Death.Volume(0.1);
		}
	}

	// enemy movement control
	
	for (CSprite* boss : bosses)
	{
		int rX1 = boss->GetX();
		int rY1 = boss->GetY();
		float dx = player.GetX() - rX1;
		float dy = player.GetY() - rY1;
		float range = sqrt((dx * dx) + (dy * dy));
		float rangeNx = dx / range;
		float rangeNy = dy / range;
		float dot = dx * rangeNx + dy * rangeNy;
		float reflectX = dx - 2 * dot*rangeNx;
		float reflectY = dy - 2 * dot*rangeNy;

		int move;
		if (range < 20) move = rand() % 5;
		else move = rand() % 100;
		
		
		if (move == 0 && boss->GetStatus() < 4)
		{

			if (boss->GetStatus() == STANDRIGHT && boss->GetStatus() != WALKRIGHT && boss->GetXVelocity() == 0)
			{
				if (boss->GetY() < 150)
				{
					boss->SetVelocity(700, 130);
					boss->SetAnimation("flyright", 10);
					boss->SetStatus(WALKRIGHT);
					boss->Update(t);
				}
				else if (boss->GetY() > 150)
				{
					boss->SetPosition(100, 100);
				}
				
				
			}
			if (boss->GetStatus() == STANDLEFT && boss->GetStatus() != WALKLEFT && boss->GetXVelocity() == 0)
			{
				if (boss->GetY() < 150)
				{
					boss->SetVelocity(-700, 130);

					boss->SetAnimation("flyleft", 10);
					boss->SetStatus(WALKLEFT);
					boss->Update(t);
				}
				else if (boss->GetY() > 150)
				{
					boss->SetPosition(700, 100);
				}

			}

		}
		else if ((boss->GetX() > 730 || boss->GetX() < 70) && boss->GetStatus() != STANDLEFT && boss->GetStatus() != STANDRIGHT)
		{
			
			if ((boss->GetStatus() == WALKRIGHT || boss->GetStatus() == WALKLEFT) && boss->GetX() < 400)
			{
				if (boss->GetY() < 100)
				{
					boss->SetPosition(100, 100);
				}
				if (boss->GetY() > 200)
				{

					boss->SetVelocity(0, 0);
					boss->SetAnimation("castright", 20);
					boss->SetStatus(CASTRIGHT);
					castTime = t;
					boss->Update(t);

					CSprite* explosion = new CSprite(player.GetX(), 300, 0, 0, GetTime());
					explosion->AddImage("Explosion.bmp", "explode", 10, 1, 9, 0, 0, 0, CColor::Black());
					explosion->SetAnimation("explode", 11);
					explosion->Die(900);
					
					explosions.push_back(explosion);

				}
				else if (boss->GetY() < 200)
				{
					boss->SetVelocity(0, 0);
					boss->SetAnimation("standright", 7);
					boss->SetStatus(STANDRIGHT);
					boss->Update(t);

				}
			}

			if ((boss->GetStatus() == WALKLEFT || boss->GetStatus() == WALKRIGHT) && boss->GetX() > 400)
			{
				if (boss->GetY() > 200)
				{
					boss->SetVelocity(0, 0);
					boss->SetAnimation("castleft", 20);
					boss->SetStatus(CASTLEFT);
					castTime = t;
					boss->Update(t);

					CSprite* explosion = new CSprite(player.GetX(), 300, 0, 0, GetTime());
					explosion->AddImage("Explosion.bmp", "explode", 10, 1, 9, 0, 0, 0, CColor::Black());
					explosion->SetAnimation("explode", 11);
					//Explode.Play("Fireball+2.wav");
					explosion->Die(900);
					explosions.push_back(explosion);

				}
				else if (boss->GetY() < 200)
				{
					boss->SetVelocity(0, 0);
					boss->SetAnimation("standleft", 7);
					boss->SetStatus(STANDLEFT);
					boss->Update(t);
				}
			}
		}

		if (t > castTime + 450 && (boss->GetStatus() == CASTLEFT || boss->GetStatus() == CASTRIGHT))
		{
			Explode.Play("Fireball+2.wav");
			Explode.Volume(.2);
		}

		if (t > castTime + 750 && (boss->GetStatus() == CASTLEFT || boss->GetStatus() == CASTRIGHT))
		{
			if (boss->GetStatus() == CASTLEFT)
			{
				boss->SetAnimation("standleft", 7);
				boss->SetStatus(STANDLEFT);
				boss->Update(t);

			}

			if (boss->GetStatus() == CASTRIGHT)
			{
				boss->SetAnimation("standright", 7);
				boss->SetStatus(STANDRIGHT);
				boss->Update(t);
			}

		}

		if (t > castTime + 650 && castTime != 0)
		{
			for (CSprite* pSprite : explosions)
			{
				if (player.HitTest(pSprite)) player.SetHealth(player.GetHealth() - 0.5);
			}

		}


		
	}

	bosses.remove_if(deleted);
	explosions.remove_if(deleted);
}

void CMyGame::EnemyControl()
{

	enum 
	{
		STANDLEFT, STANDRIGHT, WALKLEFT, WALKRIGHT, ATTACKRIGHT, ATTACKLEFT, DEAD
	} p_state;

	long t = GetTime();



	// enemy movement control
	for (CSprite* enemy : enemies)
	{
		
		// just in case one escapes
		if (enemy->GetX() > 1200 || enemy->GetY() > 1200 || enemy->GetX() < 0 || enemy->GetY() < 0)
			enemy->Delete();		

		int rX1 = enemy->GetX();
		int rY1 = enemy->GetY();
		float dx = player.GetX() - rX1;
		float dy = player.GetY() - rY1;
		float range = sqrt((dx * dx) + (dy * dy));
		float rangeNx = dx / range;
		float rangeNy = dy / range;
		float dot = dx * rangeNx + dy * rangeNy;
		float reflectX = dx - 2 * dot*rangeNx;
		float reflectY = dy - 2 * dot*rangeNy;

		if (enemy->GetStatus() != DEAD)
		{
			if ((range < 100 && enemy->GetStatus() != ATTACKRIGHT) || enemy->GetStatus() == ATTACKRIGHT || enemy->GetStatus() == ATTACKLEFT)
			{
				enemy->SetVelocity(0, 0);
				if(enemy->GetStatus() != ATTACKRIGHT || enemy->GetStatus() != ATTACKLEFT) enemy->Accelerate(player.GetX() - enemy->GetPosition().m_x, 0);
				
				if (enemy->GetXVelocity() > 0 && range > 30)
				{
					if (enemy->GetStatus() != WALKRIGHT) enemy->SetAnimation("runright");
					if (range > 30) enemy->SetStatus(WALKRIGHT);
				}
				if (enemy->GetXVelocity() < 0 && range > 30)
				{
					if (enemy->GetStatus() != WALKLEFT) enemy->SetAnimation("runleft");
					if (range > 30) enemy->SetStatus(WALKLEFT);
				}
				if (enemy->GetXVelocity() > 0 && enemy->GetStatus() != ATTACKRIGHT && range < 30)
				{
					if (enemy->GetStatus() != ATTACKRIGHT) enemy->SetAnimation("attackright", 25);
					if (range < 30) enemy->SetStatus(ATTACKRIGHT);
					enemy->SetVelocity(0, 0);
				}
				if (enemy->GetXVelocity() < 0 && enemy->GetStatus() != ATTACKLEFT && range < 30)
				{
					if (enemy->GetStatus() != ATTACKLEFT) enemy->SetAnimation("attackleft", 25);
					if (range < 30) enemy->SetStatus(ATTACKLEFT);
					enemy->SetVelocity(0, 0);
				}
				enemy->Update(t);


			}
			

			int run = rand() % 100;
			int stand = rand() % 100;

			if (range > 100 || (range > 100 && enemy->GetStatus() == ATTACKRIGHT))
			{

				if (enemy->GetStatus() == ATTACKRIGHT)
				{
					enemy->SetVelocity(0, 0);
					enemy->SetAnimation("standright", 2);
					enemy->SetStatus(STANDRIGHT);
					enemy->Update(t);

				}
				

			}

			for (CSprite* platform : platformList)
			{

				//CVector pos = enemy->GetPos();
				int xpos = enemy->GetX();
				int ypos = enemy->GetY();
				enemy->Update(t);
				if (enemy->HitTest(platform))
				{

					enemy->Update(t + 200);
					bool bHit = enemy->HitTest(platform);
					enemy->Update(t);	// back from the future


					// we change direction when hitting an obstacle 
					// very simple random movement

					if (bHit)
					{
							// if still an obstacle - don't move!
						enemy->SetXVelocity(enemy->GetXVelocity() * -1);
						if (enemy->GetXVelocity() > 1)
						{
							enemy->SetAnimation("runright");
							enemy->SetStatus(WALKRIGHT);
							enemy->Update(t);
							enemy->SetPosition(xpos+20, ypos);

						}
						if (enemy->GetXVelocity() < 1)
						{
							enemy->SetAnimation("runleft");
							enemy->SetStatus(WALKLEFT);
							enemy->Update(t);
							enemy->SetPosition(xpos-20, ypos);
						}
					}
				}

			}
			/*

				// additional test - probe 0.2 second in the future
				enemy->Update(t + 200);
				bool bHit = enemy->HitTest(&platform);
				enemy->Update(t);	// back from the future


				// we change direction when hitting an obstacle
				// very simple random movement
				if (bHit)
				{
					enemy->SetPosition(pos);	// if still an obstacle - don't move!
					enemy->SetDirection(float(rand() % 360));
					enemy->SetRotation(enemy->GetDirection() - 90);
				}
			*/
		}
		if (range < 25 && player.GetStatus() > 8 && enemy->GetStatus() != DEAD)
		{
			enemy->SetAnimation("deadright", 10);
			enemy->SetVelocity(0,0);
			enemy->SetStatus(DEAD);
			enemy->Die(800);
			Death.Play("Pain-.wav");
			Death.Volume(.1);
			enemy->Update(t);
			//enemy->Delete();
		}
		
		if (range < 15 && (enemy->GetStatus() == ATTACKRIGHT || enemy->GetStatus() == ATTACKLEFT))
		{
			enemy->Update(t + 2600);
			int futurehp = player.GetHealth();
			enemy->Update(t);

			if (futurehp > player.GetHealth() - 1) 
			{
				player.SetHealth(player.GetHealth() - 2);
			}
			enemy->Update(t);
		}
	}
	
	enemies.remove_if(deleted);
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
   
	Background.Play("BACKIT_-_Pixel_Games-.wav", -1);
	//Background.FadeOut(1000);
	Background.Volume(1);
	if (level == 1)
	{
		player.SetPosition(200, 100);
		player.SetMotion(0, 0);
		player.SetHealth(50);
		
		SetupLevel1();
	}
	if (level == 2)
	{
		player.SetPosition(690, 100);
		player.SetMotion(0, 0);
		player.GetHealth();
		
		SetupLevel2();
	}
	

}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{	
	
}

// called when the game is over
void CMyGame::OnGameOver()
{
	Background.Stop();
	enemies.clear();
	bosses.clear();
	
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
	if (sym == SDLK_1 && IsMenuMode()) level = 1;
	if (sym == SDLK_2 && IsMenuMode()) level = 2;
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	// start game with a left mouse button click
	if (IsMenuMode()) StartGame();
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
