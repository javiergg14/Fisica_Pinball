#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "ModuleGame.h"
#include "p2Point.h"

#include <math.h>

int highScore = 0;
int previousScore = 0;
float scoreTimer = 0;
float ballTimer = 0;
bool gameOver = false;	
bool mostrarTexto = true;
bool muerteLuces = true;

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	mostrarTexto = true;

	PerderBola = LoadSound("Assets/PerderBola.ogg");
	GameOver = LoadSound("Assets/GameOver.ogg");

	

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// big static circle as "ground" in the middle of the screen
	int x = (int)(SCREEN_WIDTH);
	int y = (int)(SCREEN_HEIGHT);
	int diameter = SCREEN_WIDTH / 2;
	int pinball1[56] = {
	590, 941,
	588, 225,
	582, 181,
	569, 151,
	555, 127,
	528, 102,
	505, 83,
	475, 69,
	449, 62,
	416, 54,
	375, 50,
	345, 51,
	312, 50,
	277, 54,
	253, 62,
	236, 69,
	209, 88,
	187, 103,
	174, 120,
	165, 142,
	161, 168,
	161, 193,
	168, 223,
	176, 247,
	185, 266,
	190, 283,
	182, 298,
	179, 316
	};

	int pinball2[30] = {
		181, 319,
		178, 335,
		175, 352,
		171, 366,
		166, 381,
		160, 396,
		153, 408,
		150, 422,
		148, 433,
		140, 445,
		132, 456,
		122, 461,
		111, 468,
		98, 472,
		87, 475
	};
	int pinball3[58] = {
	68, 417,
	76, 432,
	83, 449,
	90, 465,
	96, 478,
	103, 489,
	108, 494,
	99, 497,
	92, 504,
	90, 513,
	93, 522,
	98, 531,
	105, 541,
	112, 553,
	119, 562,
	126, 569,
	130, 575,
	119, 580,
	109, 585,
	101, 592,
	92, 600,
	84, 608,
	77, 619,
	73, 636,
	71, 650,
	71, 665,
	70, 679,
	70, 690,
	70, 697
	};
	int pinball4[146] = {
	551, 795,
	551, 607,
	547, 589,
	535, 574,
	515, 561,
	496, 550,
	477, 540,
	455, 528,
	439, 520,
	424, 500,
	427, 483,
	436, 474,
	447, 463,
	453, 446,
	458, 427,
	463, 415,
	472, 406,
	462, 399,
	461, 391,
	470, 383,
	485, 376,
	507, 368,
	522, 351,
	530, 325,
	533, 297,
	528, 265,
	522, 248,
	512, 228,
	498, 213,
	483, 204,
	465, 195,
	444, 187,
	420, 177,
	397, 172,
	380, 165,
	373, 171,
	373, 182,
	355, 190,
	337, 194,
	319, 176,
	300, 167,
	278, 160,
	262, 168,
	252, 191,
	240, 212,
	232, 229,
	233, 246,
	215, 254,
	202, 226,
	197, 201,
	199, 167,
	212, 138,
	227, 117,
	250, 99,
	280, 86,
	312, 82,
	340, 82,
	360, 82,
	387, 82,
	409, 86,
	437, 92,
	457, 99,
	482, 108,
	504, 122,
	525, 139,
	536, 155,
	548, 181,
	554, 210,
	561, 254,
	561, 339,
	560, 541,
	560, 795,
	560, 945
	};

	//Limits
	CreateRectangle(145,865, 225, 20, b2_staticBody, 0.55, false, false);
	CreateRectangle(50, 740, 50, 10, b2_staticBody, 0.6, false, false);
	CreateRectangle(68, 780, 10, 60, b2_staticBody, 0, false, false);
	CreateRectangle(243, 933, 10, 30, b2_staticBody, -0.05, false, false);

	CreateRectangle(465, 865, 200, 15, b2_staticBody, -0.55, false, false);
	CreateRectangle(380, 933, 10, 30, b2_staticBody, 0.05, false, false);

	CreateRectangle(68, 540, 5, 320, b2_staticBody, 0, false, false);

	CreateRectangle(x/2,0,	x, 20, b2_staticBody, 0, false, false);

	CreateRectangle(  8,	 y / 2,	   50,  y, b2_staticBody, 0, false, false);

	CreateRectangle(x, y / 2, 60, y, b2_staticBody, 0, false, false);
	PhysBody* cadenaPinball1 = CreateChain(0, 0, pinball1, 56, false, false);
	PhysBody* cadenaPinball2 = CreateChain(0, 0, pinball2, 30, false, false);
	PhysBody* cadenaPinball3 = CreateChain(0, 0, pinball3, 58, false, false);
	PhysBody* cadenaPinball4 = CreateChain(0, 0, pinball4, 146, false, false);

	//Obstacles
	CreateRectangleRebote(432, 708, 150, 10, b2_staticBody, -1.05f, true, false);
	CreateRectangleRebote(191, 708, 150, 10, b2_staticBody,  1.05f, true, false);
	CreateRectangleRebote(290, 225, 100, 20, b2_staticBody, -0.5f, true, false);
	CreateRombo(312, 358, 1.6f, 2.3f, b2_staticBody, 0, true, false);
	CreateCircleRebote(475, 290, 24, true, false);
	CreateCircleRebote(413, 335, 24, true, false);
	CreateCircleRebote(393, 260, 24, true, false);
	CreateRectangle(187, 757, 85, 10, b2_staticBody, 0.53, false, false);
	CreateRectangle(436, 757, 85, 10, b2_staticBody, -0.53, false, false);
	CreateRectangle(153, 695, 10, 90, b2_staticBody, 0, false, false);
	CreateRectangle(470, 695, 10, 90, b2_staticBody, 0, false, false);

	CreateRectangle(160, 800, 115, 23, b2_staticBody, 0.53, false, false);
	CreateRectangle(463, 800, 115, 23, b2_staticBody, -0.53, false, false);
	CreateRectangle(112, 710, 5, 145, b2_staticBody, 0, false, false);
	CreateRectangle(514, 710, 5, 145, b2_staticBody, 0, false, false);

	Boton(406, 190, 20, 10, b2_staticBody, 0.25, false, true);
	Boton(438, 200, 20, 10, b2_staticBody, 0.30, false, true);
	Boton(469, 211, 20, 10, b2_staticBody, 0.40, false, true);
	Boton(495, 230, 20, 10, b2_staticBody, 0.55, false, true);

	return true;
}



update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);
	
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			ballBool = true;

			muerteLuces = true;

			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureB()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;

			scoreTimer = 0;

			// Verificar si pb2 es la pelota
			if (pb2 && pb2->body) { //si la bola cae / toca el sensor
				ballCount++;//contador
				Combo = false;
				if (ballCount <= 5) {	
					
					PlaySound(PerderBola);
					pb2->body->SetTransform(b2Vec2(PIXEL_TO_METERS(SCREEN_WIDTH - 1), PIXEL_TO_METERS(SCREEN_HEIGHT - 4)), pb2->body->GetAngle());
					// Reiniciar la velocidad para evitar aceleraci�n infinita
					pb2->body->SetLinearVelocity(b2Vec2(0, 0));
					pb2->body->SetAngularVelocity(0);
				}
			}
		}

	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.7f;
	fixture.restitution = 0.5f;

	b->CreateFixture(&fixture);
	b->SetGravityScale(0.3);
	pbody->body = b;
	pbody->width = pbody->height = radius;

	return pbody;
}
PhysBody* ModulePhysics::CreateCircleRebote(int x, int y, int radius, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.3f;
	fixture.restitution = 1.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;

	

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleRebote(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = Type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = 0.7f; 

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}
PhysBody* ModulePhysics::Boton(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();


	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	pbody->Desactivte();

	b2BodyDef body;
	body.type = Type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = 0.5f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);


	return pbody;
}
PhysBody* ModulePhysics::CreateRombo(int x, int y, float width, float height, b2BodyType Type, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = Type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	// Definir un rombo como un cuadrado rotado
	b2PolygonShape rombo;
	b2Vec2 vertices[4];

	// V�rtices del rombo (a partir de un cuadrado con width y height)
	vertices[0].Set(0.0f, height * 0.5f);  // V�rtice superior
	vertices[1].Set(width * 0.5f, 0.0f);   // V�rtice derecho
	vertices[2].Set(0.0f, -height * 0.5f); // V�rtice inferior
	vertices[3].Set(-width * 0.5f, 0.0f);  // V�rtice izquierdo

	rombo.Set(vertices, 4);

	b2FixtureDef fixture;
	fixture.shape = &rombo;
	fixture.density = 1.0f;
	fixture.restitution = 0.7f;  // Rebote

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}


PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = Type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = 0.3f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	// Copiar los puntos en el arreglo b2Vec2.
	for (int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	// Usar prevVertex y nextVertex para no cerrar la cadena.
	b2Vec2 prevVertex(0, 0);  
	b2Vec2 nextVertex(0, 0);  

	shape.CreateChain(p, size / 2, prevVertex, nextVertex);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.restitution = 0.3f;

	b->CreateFixture(&fixture);

	delete[] p;  // Usar delete[] para liberar memoria de arreglos din�micos

	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateFlipper(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial, bool boton)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
	}

	if (boton)
	{
		pbody->SetAsBoton();
	}

	b2BodyDef body;
	body.type = Type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = rotation;
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.restitution = 1.0f;

	b->CreateFixture(&fixture);

	b->SetGravityScale(0.0f);

	b->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
	b->SetLinearDamping(1.0f);

	b->SetAwake(false);

	b->SetFixedRotation(true);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}



update_status ModulePhysics::PostUpdate()
{
	
	int displayCount = 3 - ballCount;
	const char* texto = TextFormat("Bolas restantes: %d", displayCount);

	if (IsKeyPressed(KEY_F2))
	{
		ballCount = 3;
	}

	if (mostrarTexto)	
	{
		DrawText(texto, SCREEN_WIDTH -300, 10, 30, WHITE);
		DrawText(TextFormat("Score: %d", currentScore), 20, 10, 30, WHITE);
	}

	b2Body* mouseSelect = nullptr;
	Vector2 mousePosition = GetMousePosition();
	b2Vec2 pMousePosition = b2Vec2(PIXEL_TO_METERS(mousePosition.x), PIXEL_TO_METERS(mousePosition.y));
	
	//Game Over Handling
	if (ballCount == 3) {
		gameOver = true;
		Combo = false;
		printf("Suena");
		PlaySound(GameOver);
	}
	if (gameOver) {

		DrawText("Game Over!  ", SCREEN_WIDTH / 2 - 225, SCREEN_HEIGHT / 2 - 40, 80, WHITE);

		DrawText(TextFormat("Score: %d", currentScore), SCREEN_WIDTH / 2 - 225, SCREEN_HEIGHT / 2 + 60, 40, WHITE);
		DrawText(TextFormat("High Score: %d", highScore), SCREEN_WIDTH / 2 - 225, SCREEN_HEIGHT / 2 + 120,40, WHITE);
		DrawText(TextFormat("Previous Score: %d", previousScore), SCREEN_WIDTH / 2 - 225, SCREEN_HEIGHT / 2 + 180, 40, WHITE);

		ActualitationScore();

		mostrarTexto = false;

		// Espera a que se presione la tecla de espacio para reiniciar
		if (IsKeyPressed(KEY_SPACE)) {
			mostrarTexto = true;
			gameOver = false;
			previousScore = currentScore;
			currentScore = 0;
		}
	}

	

	//Debug collisions show/hide
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}
	
	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();

				DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->m_count;
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
					prev = v;
				}

			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
			}
			break;
			}

			// TODO 1: If mouse button 1 is pressed ...

			if (IsMouseButtonPressed(0))
			{
				b2Vec2 pos(PIXEL_TO_METERS(GetMousePosition().x), PIXEL_TO_METERS(GetMousePosition().y));

				if (f->TestPoint(pos))
				{
					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = b;
					def.target = pos;
					def.damping = 0.5f;
					def.stiffness = 10.f;
					def.maxForce = 100.f * b->GetMass();

					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
				}
			}
			if (mouse_joint == nullptr && mouseSelect == nullptr && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

				if (f->TestPoint(pMousePosition)) {
					mouseSelect = b;
				}
			}

			// test if the current body contains mouse position
		}
	}
	if (mouseSelect) {
		b2MouseJointDef def;

		def.bodyA = ground;
		def.bodyB = mouseSelect;
		def.target = pMousePosition;
		def.damping = 0.5f;
		def.stiffness = 20.f;
		def.maxForce = 100.f * mouseSelect->GetMass();

		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	}
	else if (mouse_joint && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		mouse_joint->SetTarget(pMousePosition);
		b2Vec2 anchorPosition = mouse_joint->GetBodyB()->GetPosition();
		anchorPosition.x = METERS_TO_PIXELS(anchorPosition.x);
		anchorPosition.y = METERS_TO_PIXELS(anchorPosition.y);

		DrawLine(anchorPosition.x, anchorPosition.y, mousePosition.x, mousePosition.y, RED);
	}
	else if (mouse_joint && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	UnloadSound(PerderBola);

	// Delete the whole physics world!
	delete world;



	return true;
}

void ModulePhysics::ActualitationScore() 
{	
	if (currentScore > highScore)
	{
		highScore = currentScore;
	}
}



void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}
