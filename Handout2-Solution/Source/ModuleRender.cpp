#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    background = RAYWHITE;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;

    Brillo = LoadTexture("Assets/Brillo2.png");
    Apagado = LoadTexture("Assets/Apagado2.png");

    EstrellaBrillo = LoadTexture("Assets/EstrellaEncendida.png");
    EstrellaApagado = LoadTexture("Assets/EstrellaApagada.png");

    CaminosPinball = LoadTexture("Assets/CaminosPinball.png");

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
    ClearBackground(background);

    // NOTE: This function setups render batching system for
    // maximum performance, all consecutive Draw() calls are
    // not processed until EndDrawing() is called
    BeginDrawing();

    UpdateTexture();

    if (mostrarTexto)  
    {
        DrawTexture(CaminosPinball, 65, 68, WHITE);
    }

    if (mostrarTexto && !muerteLuces)
    {
        if (encendido) {
            DrawTexture(Apagado, 431, 592, WHITE);
            DrawTexture(Brillo, 420, 613, WHITE);
            DrawTexture(Apagado, 407, 635, WHITE);
            DrawTexture(Brillo, 373, 690, WHITE);
            DrawTexture(Apagado, 428, 766, WHITE);
            DrawTexture(Brillo, 482, 623, WHITE);
            DrawTexture(Apagado, 522, 664, WHITE);

            DrawTexture(Apagado, 178, 587, WHITE);
            DrawTexture(Brillo, 188, 611, WHITE);
            DrawTexture(Apagado, 201, 633, WHITE);
            DrawTexture(Brillo, 228, 690, WHITE);
            DrawTexture(Apagado, 173, 766, WHITE);
            DrawTexture(Brillo, 118, 623, WHITE);
            DrawTexture(Apagado, 81, 663, WHITE);

            DrawTexture(Apagado, 300, 820, WHITE);

            DrawTexture(Brillo, 41, 702, WHITE);

            DrawTexture(Apagado, 40, 580, WHITE);

            DrawTexture(Apagado, 154, 416, WHITE);
        }
        if (!encendido) {
            DrawTexture(Brillo, 431, 592, WHITE);
            DrawTexture(Apagado, 420, 613, WHITE);
            DrawTexture(Brillo, 407, 635, WHITE);
            DrawTexture(Apagado, 373, 690, WHITE);
            DrawTexture(Brillo, 428, 766, WHITE);
            DrawTexture(Apagado, 482, 623, WHITE);
            DrawTexture(Brillo, 522, 664, WHITE);

            DrawTexture(Brillo, 178, 587, WHITE);
            DrawTexture(Apagado, 188, 611, WHITE);
            DrawTexture(Brillo, 201, 633, WHITE);
            DrawTexture(Apagado, 228, 690, WHITE);
            DrawTexture(Brillo, 173, 766, WHITE);
            DrawTexture(Apagado, 118, 623, WHITE);
            DrawTexture(Brillo, 81, 663, WHITE);

            DrawTexture(Brillo, 300, 820, WHITE);

            DrawTexture(Apagado, 41, 702, WHITE);

            DrawTexture(Brillo, 40, 580, WHITE);

            DrawTexture(Brillo, 154, 416, WHITE);
        }
        if (Combo)
        {
            DrawTexture(EstrellaBrillo, 384, 202, WHITE);
            DrawTexture(EstrellaBrillo, 415, 210, WHITE);
            DrawTexture(EstrellaBrillo, 445, 221, WHITE);
            DrawTexture(EstrellaBrillo, 472, 235, WHITE);

            DrawTexture(Brillo, 431, 592, WHITE);
            DrawTexture(Brillo, 420, 613, WHITE);
            DrawTexture(Brillo, 407, 635, WHITE);
            DrawTexture(Brillo, 373, 690, WHITE);
            DrawTexture(Brillo, 428, 766, WHITE);
            DrawTexture(Brillo, 482, 623, WHITE);
            DrawTexture(Brillo, 522, 664, WHITE);

            DrawTexture(Brillo, 178, 587, WHITE);
            DrawTexture(Brillo, 188, 611, WHITE);
            DrawTexture(Brillo, 201, 633, WHITE);
            DrawTexture(Brillo, 228, 690, WHITE);
            DrawTexture(Brillo, 173, 766, WHITE);
            DrawTexture(Brillo, 118, 623, WHITE);
            DrawTexture(Brillo, 81, 663, WHITE);

            DrawTexture(Brillo, 300, 820, WHITE);

            DrawTexture(Brillo, 41, 702, WHITE);

            DrawTexture(Brillo, 40, 580, WHITE);

            DrawTexture(Brillo, 154, 416, WHITE);
        }
    }
    if (IsKeyDown(KEY_F4))
    {
        DrawFPS(10, 910);
    }
    DrawTexture(EstrellaApagado, 384, 202, WHITE);
    DrawTexture(EstrellaApagado, 415, 210, WHITE);
    DrawTexture(EstrellaApagado, 445, 221, WHITE);
    DrawTexture(EstrellaApagado, 472, 235, WHITE);
	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
    // Draw everything in our batch!


    EndDrawing();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	return true;
    UnloadTexture(Brillo);
    UnloadTexture(Apagado);
}

void ModuleRender::UpdateTexture() {
    // Obtener el tiempo transcurrido desde que el programa comenzó
    float currentTime = GetTime();

    // Comprobar si ha pasado el intervalo de tiempo
    if ((int)(currentTime / Interval) % 2 == 0) {
        encendido = true;
    }
    else {
        encendido = false;
    }
}

void ModuleRender::SetBackgroundColor(Color color)
{
	background = color;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	float scale = 1.0f;
    Vector2 position = { (float)x, (float)y };
    Rectangle rect = { 0.f, 0.f, (float)texture.width, (float)texture.height };

    if (section != NULL) rect = *section;

    position.x = (float)(x-pivot_x) * scale + camera.x;
    position.y = (float)(y-pivot_y) * scale + camera.y;

	rect.width *= scale;
	rect.height *= scale;

    DrawTextureRec(texture, rect, position, WHITE);

	return ret;
}

bool ModuleRender::DrawText(const char * text, int x, int y, Font font, int spacing, Color tint) const
{
    bool ret = true;

    Vector2 position = { (float)x, (float)y };

    DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

    return ret;
}
