// Adam Peters - Assignment 3.

//libraries

#include <TL-Engine.h>	
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <time.h>
#include <sstream>

using namespace tle;

//structs-------------------------------------------------------------------------------------------------------------------------------

struct checkpoints
{
	IModel* model;
};

struct isles
{
	IModel* model;
};

struct walls
{
	IModel* model;
};

struct vector2D
{
	float x;
	float z;

};



//function declarations--------------------------------------------------------------------------------------------------------------------

void OpenFile(ifstream& infile, string filename);

vector2D scalar(float s, vector2D v);

vector2D sum3(vector2D v1, vector2D v2, vector2D v3);

bool sphere2sphere(float s1xPos, float s1zPos, float s1rad, float s2xPos, float s2zPos, float s2rad);

bool sphere2box(float sxPos, float szPos, float srad, float bxPos, float bzPos, float bWidth, float bDepth);



//variables---------------------------------------------------------------------------------------------------------------------------------

//frametime

float frametime;


//constants---------------------------------------------------------------------------------------------------------------------------------

const float kGameSpeed = 1.0f;

const float kCarRadius = 1.70f;

const float checkpointWidth = 5.0f;
const float checkpointDepth = 0.20f;



//enums-------------------------------------------------------------------------------------------------------------------------------------

enum gameStates
{
	ready, racing, over
};

enum racingStates
{
	start, checkpoint1, checkpoint2, checkpoint3
};

//key codes---------------------------------------------------------------------------------------------------------------------------------

EKeyCode quitKey = Key_Escape;

EKeyCode camUpKey = Key_Up;
EKeyCode camDownKey = Key_Down;
EKeyCode camRightKey = Key_Right;
EKeyCode camLeftKey = Key_Left;
EKeyCode camResetKey = Key_1;

EKeyCode carForwardKey = Key_W;
EKeyCode carBackwardKey = Key_S;
EKeyCode carLeftKey = Key_A;
EKeyCode carRightKey = Key_D;

EKeyCode startKey = Key_Space;




void main()
{
	//create engine.

	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	
	// ---------------------------------------------folders for media assets.------------------------------------------------------------------

	myEngine->AddMediaFolder("D:\\DOWNLOADS\\Uni\\Modules\\2- CO1301 Games Concepts\\Assignments\\Assignment 3\\Models");
	myEngine->AddMediaFolder("D:\\DOWNLOADS\\Uni\\Modules\\2- CO1301 Games Concepts\\Assignments\\Assignment 3\\Extra models");

	//------------------------------------------------------meshes-----------------------------------------------------------------------------

	IMesh* carMesh = myEngine->LoadMesh("race2.x");
	IMesh* checkpointMesh = myEngine->LoadMesh("checkpoint.x");
	IMesh* skyboxMesh = myEngine->LoadMesh("skybox 07.x");
	IMesh* groundMesh = myEngine->LoadMesh("ground.x");
	IMesh* smallTankMesh = myEngine->LoadMesh("TankSmall1.x");
	IMesh* smallTankMesh2 = myEngine->LoadMesh("TankSmall2.x");
	IMesh* wallEndMesh = myEngine->LoadMesh("IsleStraight.x");
	IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
	IMesh* trapMesh = myEngine->LoadMesh("Tribune1.x");
	IMesh* enemyMesh = myEngine->LoadMesh("Interstellar.x");
	IMesh* crossMesh = myEngine->LoadMesh("Cross.x");
	IMesh* bombMesh = myEngine->LoadMesh("Flare.x");
	IMesh* backdropMesh = myEngine->LoadMesh("banner.tga");

	//-------------------------------------backdrop--------------------------------------------------------------
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);
	ISprite* backdrop = myEngine->CreateSprite("ui_backdrop.jpg", 268,658);


	//------------------------------------------------------models-------------------------------------------------------------------------------

	//car---------------------------------------------------------------------------------

	IModel* hoverCar = carMesh->CreateModel(0, 0, -30);

	//skybox-------------------------------------------------------------------------------

	IModel* skybox = skyboxMesh->CreateModel(0, -960, 0);

	//ground-------------------------------------------------------------------------------

	IModel* ground = groundMesh->CreateModel();

	//checkpoints--------------------------------------------------------------------------

	ifstream infile;

	OpenFile(infile, "checkpointPositions.txt");


	int checkpointNum = 1;

	bool checkpoint1Hit = false;
	bool checkpoint2Hit = false;


	const int kNumCheckpoints = 3;

	int checkpointPositions[3][3];

	checkpoints checkpoint[kNumCheckpoints];
	
	for (int i = 0; i < kNumCheckpoints; i++)
	{
		for (int e = 0; e < kNumCheckpoints; e++)
		{
			infile >> checkpointPositions[i][e];
		}
	}

	for (int i = 0; i < kNumCheckpoints; i++)
	{
		checkpoint[i].model = checkpointMesh->CreateModel( checkpointPositions[i][0], checkpointPositions[i][1], checkpointPositions[i][2] );
	}
	checkpoint[1].model->RotateY(90);


	//close the current file
	infile.close();

	//checkpoint structs-----------------------------------------------------------------------------------------------


	OpenFile( infile, "structPositions.txt" );

	int structPositions[6][3];

	const int kNumStructs = 6;
	const int kNumStructColumns = 3;

	const float kStructRadius = 2.1f;

	for (int i = 0; i < kNumStructs; i++)
	{
		for (int e = 0; e < kNumStructColumns; e++)
		{
			infile >> structPositions[i][e];
		}
	}

	infile.close();

	//---------------------------isles-------------------------------------------------------------------------------------

	OpenFile(infile, "islePositions.txt");


	const int kNumIsles = 6;
	const int kNumIslecolumns = 3;

	const float isleWidth = 6.2f;
	const float isleDepth = 4.8f;

	int islePositions[6][3];

	isles isle[kNumIsles];

	for (int i = 0; i < kNumIsles; i++)
	{
		for (int e = 0; e < kNumIslecolumns; e++)
		{
			infile >> islePositions[i][e];
		}
	}

	for (int i = 0; i < kNumIsles; i++)
	{
		isle[i].model = wallEndMesh->CreateModel( islePositions[i][0], islePositions[i][1], islePositions[i][2] );
	}

	infile.close();

	//-------------------------walls-------------------------------------------------------------------------------------------

	OpenFile(infile, "wallpositions.txt");

	const int kNumWalls = 4;
	const int kNumWallColumns = 3;

	const float wallWidth = 5.0f;
	const float wallDepth = 8.0f;

	int wallPositions[4][3];

	walls wall[kNumWalls];

	for (int i = 0; i < kNumWalls; i++)
	{
		for (int e = 0; e < kNumWallColumns; e++)
		{
			infile >> wallPositions[i][e];
		}
	}

	for (int i = 0; i < kNumWalls; i++)
	{
		wall[i].model = wallMesh->CreateModel( wallPositions[i][0], wallPositions[i][1], wallPositions[i][2] );
	}

	infile.close();

	//----------------------------------------------------camera---------------------------------------------------------------------------------------

	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 10, -25);

	myCamera->AttachToParent(hoverCar);

	const float kCameraSpeed = 0.1f * kGameSpeed;
	const float kCameraRotation = 0.1f * kGameSpeed;

	int mouseMoveX = 0;
	int mouseMoveY = 0;

	//------------------------------------------------------vector variables-------------------------------------------------------------------------------

	vector2D momentum{ 0.0f, 0.0f };
	vector2D thrust{ 0.0f, 0.0f };
	vector2D drag{ 0.0f, 0.0f };

	float matrix[4][4];
	const float thrustFactor = 0.0002f * kGameSpeed;		//0.00006f
	const float dragCoefficient = 0.00005f * kGameSpeed;	//0.00005f

	const float carRotationSpeed = 80.0f * kGameSpeed;

	gameStates gameState = ready;
	racingStates racingState = start;

	//-------------------------------timer----------------------------------------------

	int timer = 1;

	bool start = false;


	// The main game loop, repeat until engine is stopped------------------------------------------------------------------------------------------------
	while (myEngine->IsRunning())
	{
		myEngine->DrawScene();

		//draw the scene frame by frame-------------------------------------------------------------------------------------------------------------------

		//frame variables-------------------------------------------------------------------------------------------------------------------------------

		bool checkpointCollision1 = false;
		bool checkpointCollision2 = false;
		bool checkpointCollision3 = false;

		bool structCollision = false;
		bool wallCollision = false;
		bool isleCollision = false;

		float oldX = hoverCar->GetX();
		float oldZ = hoverCar->GetZ();

		//first part of gamestate 

		if (gameState == ready)
		{
			

			//---------------------------------------------------------------frametime------------------------------------------------------

			frametime = myEngine->Timer() * kGameSpeed;

			//------------------------------------------------------------starting text---------------------------------------------------

			if (start == false)
			{
				stringstream outText;
				outText << "Hit Space to Start: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str(""); // Clear myStream
				outText << "Ready";
				myFont->Draw(outText.str(), 300, 658);
				outText.str("");
			}

			//-------------------------------display counter for text and gamestate---------------------------------------------------

			if (myEngine->KeyHit(startKey))
			{
				start = true;
			}

			if (start == true && timer < 2400)
			{
				timer++;
			}

			if (timer < 500 && start == true)
			{
				stringstream outText;
				outText << "3: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str("");
			}

			if (timer < 1000 && timer > 500 && start == true)
			{
				stringstream outText;
				outText << "2: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str("");
			}

			if (timer < 1500 && timer > 1000 && start == true)
			{
				stringstream outText;
				outText << "1: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str("");
			}

			if (start == true && gameState != over && timer >= 1501)
			{
				stringstream outText;
				outText << "GO: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str("");
			}

			if (start == true && gameState != over && timer >= 1800)
			{
				gameState = racing;
			}

		}

		//second part of gamestate--------------------------------------
		if (gameState == racing)
		{

			stringstream outText;
			outText << "Racing ";
			myFont->Draw(outText.str(), 300, 658);
			outText.str(""); // Clear myStream

			//---------------------------------------------------get facing vector (local z)-------------------------------------------------------------------

			hoverCar->GetMatrix(&matrix[0][0]);

			vector2D facingVector = { matrix[2][0], matrix[2][2] };

			//-----------------------------------------------------calculate thrust (based on keyboard input)---------------------------------------------------

			if (myEngine->KeyHeld(carLeftKey))
			{
				hoverCar->RotateY(-carRotationSpeed * frametime);
			}

			else if (myEngine->KeyHeld(carRightKey))
			{
				hoverCar->RotateY(carRotationSpeed * frametime);
			}

			if (myEngine->KeyHeld(carForwardKey))
			{
				thrust = scalar(thrustFactor, facingVector);
			}
			else if (myEngine->KeyHeld(carBackwardKey))
			{
				thrust = scalar(-thrustFactor / 2, facingVector);
			}
			else
			{
				thrust = { 0.0f, 0.0f };
			}

			//------------------------------------calculate drag (based on previous momentum)----------------------------------------------------

			drag = scalar(dragCoefficient, momentum);

			//---------------------------------calculate momentum (based on thrust, drag and previous momentum)------------------------------------------

			momentum = sum3(momentum, thrust, drag);

			//-----------------------------------------move the hover car (according to new momentum)-------------------------------------------------

			hoverCar->Move(momentum.x, 0.0f, momentum.z);	//if i add frametime the car moves even when no thrust is applied

			//-------------------------------------------checkpoint collisions--------------------------------------------------------------

			checkpointCollision1 = sphere2box(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
				checkpointPositions[1][0], checkpointPositions[1][2], checkpointWidth, checkpointDepth);

			checkpointCollision2 = sphere2box(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
				checkpointPositions[2][0], checkpointPositions[2][2], checkpointWidth, checkpointDepth);

			checkpointCollision3 = sphere2box(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
				checkpointPositions[0][0], checkpointPositions[0][2], checkpointWidth, checkpointDepth);

			//-----------------------------------------------resolve collisions--------------------------------------------------

			if (checkpointCollision1)
			{
				racingState = checkpoint1;
				checkpoint1Hit = true;

				stringstream outText;
				outText.str("");
				outText << "Checkpoint 1 Hit: ";
				myFont->Draw(outText.str(), 550, 658);

			}
			else if (checkpointCollision2 && checkpoint1Hit == true)
			{
				racingState = checkpoint2;
				checkpoint2Hit = true;

				stringstream outText;
				outText.str("");
				outText << "Checkpoint 2 Hit: ";
				myFont->Draw(outText.str(), 550, 658);

			}
			else if (checkpointCollision3 && checkpoint1Hit == true && checkpoint2Hit == true)
			{
				racingState = checkpoint3;

				stringstream outText;
				outText << "Checkpoint 3 Hit: ";
				myFont->Draw(outText.str(), 550, 658);
				outText.str("");

			}


			if (racingState == checkpoint3)
			{
				gameState = over;
			}

			//-----------------------------------------checkpoint strut collisions-----------------------------------------------

			float oldX = hoverCar->GetX();
			float oldZ = hoverCar->GetZ();

			for (int i = 0; i < kNumStructs; i++)
			{
				if (sphere2sphere(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
					structPositions[i][0], structPositions[i][2], kStructRadius))
				{
					structCollision = true;
				}
			}

			if (structCollision == true)
			{
				thrust = {0.0f, 0.0f};
				hoverCar->Move((momentum.x ) , 0.0f, (-momentum.z / 2 ));
			}

			//--------------wall collisions------------------------------------------------------------------------------------------

			for (int i = 0; i < kNumWalls; i++)
			{
				if (sphere2box(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
					wallPositions[i][0], wallPositions[i][2], wallWidth, wallDepth))
				{
					wallCollision = true;
				}
			}

			if (wallCollision == true)
			{
				thrust = { 0.0f, 0.0f };
				hoverCar->Move(-momentum.x / 1.5, 0.0f, momentum.z / 1.5);
			}

			//---------------------------------------------isle collisions------------------------------------------------------

			for (int i = 0; i < kNumIsles; i++)
			{
				if (sphere2box(hoverCar->GetX(), hoverCar->GetZ(), kCarRadius,
					islePositions[i][0], islePositions[i][2], isleWidth, isleDepth))
				{
					isleCollision = true;
				}


				if (isleCollision == true)
				{
					thrust = { 0.0f, 0.0f };
					hoverCar->Move(-momentum.x / 1.5, 0.0f, momentum.z / 1.5);
				}
			}
			//--------------------------------------------camera control keys----------------------------------------------

			if (myEngine->KeyHeld(camUpKey))
			{
				myCamera->MoveZ(kCameraSpeed);
			}
			if (myEngine->KeyHeld(camDownKey))
			{
				myCamera->MoveZ(-kCameraSpeed);
			}
			if (myEngine->KeyHeld(camRightKey))
			{
				myCamera->MoveX(kCameraSpeed);
			}
			if (myEngine->KeyHeld(camLeftKey))
			{
				myCamera->MoveX(-kCameraSpeed);
			}
			if (myEngine->KeyHit(camResetKey))
			{
				myCamera->SetLocalPosition(0, 10, -25);
			}

			//-------------------------------------------stop camera from "wiggling round"-----------------------------------

			mouseMoveX += myEngine->GetMouseMovementX();
			mouseMoveY += myEngine->GetMouseMovementY();

			myCamera->ResetOrientation();
			myCamera->RotateY(mouseMoveX * kCameraSpeed);
			myCamera->RotateX(mouseMoveY * kCameraSpeed);

		}

		//gamestate over.

		if (gameState == over)
		{

			stringstream outText;
			outText << "Over ";
			myFont->Draw(outText.str(), 550, 658);
			outText.str(""); // Clear myStream

			myEngine->Stop();
		}


		//--------stop engine if esc key hit.---------

		if (myEngine->KeyHit(quitKey))
		{
			myEngine->Stop();
		}

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}



//-----------------------------------------------function definitions------------------------------------------------------------------------------


void OpenFile(ifstream& infile, string filename)
{
	infile.open(filename);
	if (!infile)
	{
		return;
	}
}


vector2D scalar(float s, vector2D v)
{

	return{ s * v.x, s * v.z };

}


vector2D sum3(vector2D v1, vector2D v2, vector2D v3)
{

	return{ v1.x + v2.x + v3.x, v1.z + v2.z + v3.z };

}


bool sphere2sphere(float s1xPos, float s1zPos, float s1rad, float s2xPos, float s2zPos, float s2rad)
{

	float distX = s2xPos - s1xPos;
	float distZ = s2zPos - s1zPos;
	float distance = sqrt(distX * distX + distZ * distZ);

	return(distance < (s1rad + s2rad));		

}


bool sphere2box(float sxPos, float szPos, float srad, float bxPos, float bzPos, float bWidth, float bDepth)
{

	float minX = bxPos - bWidth / 2 - srad;
	float maxX = bxPos + bWidth / 2 + srad;
	float minZ = bzPos - bDepth / 2 - srad;
	float maxZ = bzPos + bDepth / 2 + srad;

	bool barrierCollision = false;

	if (sxPos > minX && sxPos < maxX && szPos > minZ && szPos < maxZ)
	{
		barrierCollision = true;
	}

	return(barrierCollision);


}




