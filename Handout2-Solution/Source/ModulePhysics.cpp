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

	int pinball2[8] = {
	153, 404,
	148, 414,
	149, 427,
	146, 433
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
	CreateRectangle(145,865, 225, 20, b2_staticBody, 0.55);
	CreateRectangle(50, 740, 50, 10, b2_staticBody, 0.6);
	CreateRectangle(68, 780, 10, 60, b2_staticBody, 0);
	CreateRectangle(243, 933, 10, 30, b2_staticBody, -0.05);

	CreateRectangle(465, 865, 200, 15, b2_staticBody, -0.55);
	CreateRectangle(380, 933, 10, 30, b2_staticBody, 0.05);

	CreateRectangle(68, 540, 5, 320, b2_staticBody, 0);

	CreateRectangle(x/2,0,	x, 20, b2_staticBody, 0);

	CreateRectangle(  8,	 y / 2,	   50,  y, b2_staticBody, 0);

	CreateRectangle(x, y / 2, 60, y, b2_staticBody, 0);
	PhysBody* cadenaPinball1 = CreateChain(0, 0, pinball1, 56);
	PhysBody* cadenaPinball2 = CreateChain(0, 0, pinball2, 8);
	PhysBody* cadenaPinball3 = CreateChain(0, 0, pinball3, 58);
	PhysBody* cadenaPinball4 = CreateChain(0, 0, pinball4, 146);

	//Obstacles
	CreateRectangleRebote(432, 708, 150, 10, b2_staticBody, -1.05f, true);
	CreateRectangleRebote(191, 708, 150, 10, b2_staticBody,  1.05f, true);
	CreateRectangleRebote(290, 225, 100, 20, b2_staticBody, -0.5f, true);
	CreateRombo(312, 358, 1.6f, 2.3f, b2_staticBody, 0, true);
	CreateCircleRebote(475, 290, 24, true);
	CreateCircleRebote(413, 335, 24, true);
	CreateCircleRebote(393, 260, 24, true);
	CreateRectangle(187, 757, 85, 10, b2_staticBody, 0.53);
	CreateRectangle(436, 757, 85, 10, b2_staticBody, -0.53);
	CreateRectangle(153, 695, 10, 90, b2_staticBody, 0);
	CreateRectangle(470, 695, 10, 90, b2_staticBody, 0);

	CreateRectangle(160, 800, 115, 23, b2_staticBody, 0.53);
	CreateRectangle(463, 800, 115, 23, b2_staticBody, -0.53);
	CreateRectangle(112, 710, 5, 145, b2_staticBody, 0);
	CreateRectangle(514, 710, 5, 145, b2_staticBody, 0);




	return true;
}



update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);
	
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureB()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;

			scoreTimer = 0;

			// Verificar si pb2 es la pelota
			if (pb2 && pb2->body) { //si la bola cae / toca el sensor
				
				float timer = 0.0f;
				bool waiting = true;

				//while (!WindowShouldClose()) {
				//	// Actualizar el temporizador
				//	if (waiting) {
				//		timer += GetFrameTime(); // Tiempo desde el último frame
				//		if (timer >= 2.0f) { // 2 segundos
				//			waiting = false; // Terminar la espera
				//		}
				//	}
				//}

				// Cambiar la posición de la pelota a un nuevo lugar
				ballCount++; //contador
				if(ballCount <=2) {
				pb2->body->SetTransform(b2Vec2(PIXEL_TO_METERS(SCREEN_WIDTH - 1), PIXEL_TO_METERS(SCREEN_HEIGHT - 4)), pb2->body->GetAngle());
				// Reiniciar la velocidad para evitar aceleración infinita
				pb2->body->SetLinearVelocity(b2Vec2(0, 0));
				pb2->body->SetAngularVelocity(0);
				}
			}
		}

	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.2f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;

	return pbody;
}
PhysBody* ModulePhysics::CreateCircleRebote(int x, int y, int radius, bool isSpecial)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
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
	fixture.restitution = 1.5f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;

	

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleRebote(int x, int y, int width, int height, b2BodyType Type, float rotation, bool isSpecial)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
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
	fixture.restitution = 1.5f; 

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}
PhysBody* ModulePhysics::CreateRombo(int x, int y, float width, float height, b2BodyType Type, float rotation, bool isSpecial)
{
	PhysBody* pbody = new PhysBody();

	if (isSpecial)
	{
		pbody->SetAsSpecial();
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

	// Vértices del rombo (a partir de un cuadrado con width y height)
	vertices[0].Set(0.0f, height * 0.5f);  // Vértice superior
	vertices[1].Set(width * 0.5f, 0.0f);   // Vértice derecho
	vertices[2].Set(0.0f, -height * 0.5f); // Vértice inferior
	vertices[3].Set(-width * 0.5f, 0.0f);  // Vértice izquierdo

	rombo.Set(vertices, 4);

	b2FixtureDef fixture;
	fixture.shape = &rombo;
	fixture.density = 1.0f;
	fixture.restitution = 1.5f;  // Rebote

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}


PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, b2BodyType Type, float rotation)
{
	PhysBody* pbody = new PhysBody();

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

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, float rotation)
{
	PhysBody* pbody = new PhysBody();

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

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
	PhysBody* pbody = new PhysBody();

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
	fixture.restitution = 0.5f;

	b->CreateFixture(&fixture);

	delete[] p;  // Usar delete[] para liberar memoria de arreglos dinámicos

	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}




update_status ModulePhysics::PostUpdate()
{
	
	int displayCount = 3 - ballCount;
	const char* texto = TextFormat("Bolas restantes: %d", displayCount);

	if (IsKeyPressed(KEY_F2))
	{
		gameOver = true;
	}

	if (mostrarTexto)	
	{
		DrawText(texto, SCREEN_WIDTH -300, 10, 30, WHITE);
		DrawText(TextFormat("Score: %d", currentScore), 20, 10, 30, WHITE);
	}
	
	//Game Over Handling
	if (ballCount == 3) {
		gameOver = true;
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
			ballCount = 0;
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

			// test if the current body contains mouse position
		}
		if (mouse_joint)
		{

			b2Vec2 pos(PIXEL_TO_METERS(GetMousePosition().x), PIXEL_TO_METERS(GetMousePosition().y));
			mouse_joint->SetTarget(pos);


			DrawLine(METERS_TO_PIXELS(mouse_joint->GetAnchorB().x), METERS_TO_PIXELS(mouse_joint->GetAnchorB().y), GetMousePosition().x, GetMousePosition().y, RED);
		}

	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

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
