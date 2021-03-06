/*
==================================================================================
cGame.cpp
==================================================================================
*/

#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance(); //finds buttons


/*
=================================================================================
Constructor
=================================================================================
*/

cGame::cGame()
{

}

/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/

cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	srand(time(NULL));

	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();

	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);

	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "asteroid1", "asteroid2", "asteroid3", "asteroid4","asteroid 5", "bullet","theRocket","theBackground" };
	texturesToUse = { "Images\\Apple.png", "Images\\Orange.png", "Images\\Banana.png", "Images\\Pear.png", "Images\\Strawberry.png", "Images\\bullet.png", "Images\\basket.png", "Images\\backgroundPark.png" };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}

	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge", "skipLegDay" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/SkipLegDay.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	gameTextList = { "Fruit Catcher", "Score : 0", "You Did It" };

	theTextureMgr->addTexture("Title", theFontMgr->getFont("skipLegDay")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("skipLegDay")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("endText", theFontMgr->getFont("skipLegDay")->createTextTexture(theRenderer, gameTextList[2], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	//store buttons
	btnNameList = { "exit_btn", "load_btn", "menu_btn", "play_btn", "save_btn"};
	btnTexturesToUse = { "Images/button_exit.png", "Images/button_load.png", "Images/button_menu.png", "Images/button_play.png", "Images/button_save.png"};
	btnPos = { {740, 550}, {740, 450}, {740, 600}, {740, 450}, {740, 500}};

	for (int buttonCount = 0; buttonCount < btnNameList.size(); buttonCount++)
	{
		theTextureMgr->addTexture(btnNameList[buttonCount], btnTexturesToUse[buttonCount]);
	}

	for (int buttonCount = 0; buttonCount < btnNameList.size(); buttonCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[buttonCount]));
		newBtn->setSpritePos(btnPos[buttonCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[buttonCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[buttonCount])->getTHeight());
		theButtonMgr->add(btnNameList[buttonCount], newBtn);
	}

	theGameState = MENU;
	theBtnType = EXIT;

	// Load game sounds
	soundList = { "theme", "explosion", "squelch"};
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/8bit_song.wav", "Audio/Bell.wav", "Audio/Squelch.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theRocket.setSpritePos({ 500, 600 });
	theRocket.setTexture(theTextureMgr->getTexture("theRocket"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("theRocket")->getTWidth(), theTextureMgr->getTexture("theRocket")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	score = 0;
}

void cGame::createFruit()
{
	theAsteroids.push_back(new cAsteroid);
	int numberOfAsteroids = theAsteroids.size();
	int fruit = numberOfAsteroids - 1;

	theAsteroids[fruit]->setSpritePos({ 150 * (rand() % 5 + 1), 5 * (rand() % 5 + 1) });
	theAsteroids[fruit]->setSpriteTranslation({ 3 * (rand() % 2 + 1), 3 * (rand() % 2 + 1) });
	int randAsteroid = rand() % 5;
	theAsteroids[fruit]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
	theAsteroids[fruit]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
	theAsteroids[fruit]->setAsteroidVelocity({ 3, 3 });
	theAsteroids[fruit]->setActive(true);
}



void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

		while (loop)
		{
			//We get the time that passed since the last frame

			double elapsedTime = this->getElapsedSeconds();

			loop = this->getInput(loop);
			this->update(elapsedTime);
			this->render(theSDLWND, theRenderer);
		}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);	//clear the window
	spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());

	SDL_RenderClear(theRenderer);	//clear the window

	switch (theGameState)
	{
	case MENU:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());	//render background

		// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

		//reset score
		score = 0;

		//Render Score
		if (scoreChanged)
		{
			gameTextList[1] = scoreAsString.c_str();
			theTextureMgr->addTexture("Score", theFontMgr->getFont("skipLegDay")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
			scoreChanged = false;
		}

		SDL_RenderPresent(theRenderer);
	}
	break;

	case PLAYING:
	{
		SDL_RenderClear(theRenderer);	//clear the window
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());	//render background

		// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

		// Render each fruit in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}

		//Render Score
		if (scoreChanged)
		{
			gameTextList[1] = scoreAsString.c_str();
			theTextureMgr->addTexture("Score", theFontMgr->getFont("skipLegDay")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
			scoreChanged = false;
		}

		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 10, 50, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		// render the basket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		
		SDL_RenderPresent(theRenderer);

	}
	break;

	case END:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());	//render background
																					// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("endText");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());

		SDL_RenderPresent(theRenderer);
	}
	break;

	case QUIT:
	{
		loop = false;	//close the program
	}
	break;
	default:
		break;
	}

	SDL_RenderPresent(theRenderer);

}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}





void cGame::update(double deltaTime)
{
	switch (theGameState)
	{
	case MENU:
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);

		if (theGameState == PLAYING)
		{
			// Remove all the asteroids if there are any - they will be recreated anyway below
			vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
			while (asteroidIterator != theAsteroids.end())
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}

			// Create vector array of textures
			for (int astro = 0; astro < 5; astro++)
			{
				theAsteroids.push_back(new cAsteroid);
				theAsteroids[astro]->setSpritePos({ 150 * (rand() % 5 + 1), 5 * (rand() % 5 + 1) });
				theAsteroids[astro]->setSpriteTranslation({ 3 * (rand() % 2 + 1), 3 * (rand() % 2 + 1) });
				int randAsteroid = rand() % 5;
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				theAsteroids[astro]->setAsteroidVelocity({ 3, 3 });
				theAsteroids[astro]->setActive(true);
			}

			score = 0;
		}
	}
	break;
	case PLAYING:
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);

		// Update the visibility and position of each fruit
		int fruitCaught = 0;

		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
				++fruitCaught;
			}
			else
			{
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}

		for (int playerBasket = 0; playerBasket < fruitCaught; ++playerBasket)
		{
			// create a new fruit
			createFruit();
		}

		//Check fruit position

		for (int a = 0; a < theAsteroids.size(); a++)
		{
			if (theAsteroids[a]->getSpritePos().y >= (renderHeight - 75))
			{
				//if fruit position passes the player with no collision, set the fruit to false

				score--;
				if (theTextureMgr->getTexture("Score") != NULL)
				{
					theTextureMgr->deleteTexture("Score");
				}


				string theScore = to_string(score);
				scoreAsString = "Score : " + theScore;
				scoreChanged = true;

				theAsteroids[a]->setActive(false);
				theSoundMgr->getSnd("squelch")->play(0);
			}


		}

		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/

		//collison detector in update(deltaTime);

		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if (theRocket.collidedWith(&(theRocket.getBoundingRect()), &(*asteroidIterator)->getBoundingRect()))
			{
				// if a collision set the fruit to false

				score++;
				if (theTextureMgr->getTexture("Score") != NULL)
				{
					theTextureMgr->deleteTexture("Score");
				}


				string theScore = to_string(score);
				scoreAsString = "Score : " + theScore;
				scoreChanged = true;

				(*asteroidIterator)->setActive(false);
				theSoundMgr->getSnd("explosion")->play(0);
			}
		}

		// Update the basket's position

		theRocket.update(deltaTime);

		if (score >= 10)
		{
			theGameState = END;
		}
	}
	break;
	case END:
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
	}
	break;
	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_RIGHT:
				{
					theRocket.setRocketVelocity({ 250 , 0 });
				}
				break;

				case SDLK_LEFT:
				{
					theRocket.setRocketVelocity({ -250 , 0 });
				}
				break;
				}

			default:
				break;			
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

