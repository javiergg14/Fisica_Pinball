#pragma once
#include "Module.h"
#include "Globals.h"

#include <limits.h>

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	void UpdateTexture();
	bool CleanUp();

    void SetBackgroundColor(Color color);
	bool Draw(Texture2D texture, int x, int y, const Rectangle* section = NULL, double angle = 0, int pivot_x = 0, int pivot_y = 0) const;
    bool DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const;

public:

	Color background;
    Rectangle camera;
	Texture2D Brillo1;
	Texture2D Apagado1;
	Texture2D Brillo2;
	Texture2D Apagado2;
	Texture2D Brillo3;
	Texture2D Apagado3;
	Texture2D Brillo4;
	Texture2D Apagado4;
	Texture2D Brillo5;
	Texture2D Apagado5;
	Texture2D Brillo6;
	Texture2D Apagado6;
	Texture2D Brillo7;
	Texture2D Apagado7;
	Texture2D Brillo8;
	Texture2D Apagado8;
	Texture2D Brillo9;
	Texture2D Apagado9;
	Texture2D Brillo10;
	Texture2D Apagado10;
	Texture2D Brillo11;
	Texture2D Apagado11;
	Texture2D Brillo12;
	Texture2D Apagado12;
	Texture2D Brillo13;
	Texture2D Apagado13;
	Texture2D Brillo14;
	Texture2D Apagado14;
	Texture2D Brillo15;
	Texture2D Apagado15;
	Texture2D Brillo16;
	Texture2D Apagado16;
	float Interval = 0.7f;
	bool encendido = false;
};