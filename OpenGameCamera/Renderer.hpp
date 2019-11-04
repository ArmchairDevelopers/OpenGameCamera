#pragma once
#include "sdk.hpp"
#include "Candy.hpp"
#include <string>

// renderer class, for interfacing with DebugRenderer
class Renderer {
public:
	// our custom callback function
	typedef void(__fastcall* tRENDERERCALLBACK)(Renderer*, uint32_t, uint32_t);
private:
	// instance of previously mentioned callback function
	static tRENDERERCALLBACK callbackFunction;

	// the function that our hook will jump to
	static DWORD64 __fastcall hkEndFrame(void* _this, DWORD64 a2, bool presentEnable);

	// get the DebugRenderer Thread Context
	void* getDebugRenderer(void);

	// create a static pointer for a default instance
	static Renderer* pRenderer;
public:
	// what the user calls to setup the renderer
	static void setup(tRENDERERCALLBACK callback);
	static void shutdown();
	// function to draw a rectangle and a line
	void drawRect(float x, float y, float width, float height, Color32 color);
	void drawLine(float x1, float y1, float x2, float y2, Color32 color);

	// draw text, either from a char* or an std::string
	void drawText(int x, int y, Color32 color, const std::string& text, float scale);
	void drawText(int x, int y, Color32 color, char* text, float scale);
};

// -------------------------

// our endframe hook
DWORD64 __fastcall Renderer::hkEndFrame(void* _this, DWORD64 a2, bool presentEnable)
{
	// call the user-supplied callback function
	Renderer::callbackFunction(Renderer::pRenderer, 1920, 1080);
	// return the original EndFrame value
	return oEndFrame(_this, a2, presentEnable);
};

// get the DebugRenderer Thread Context, for argument 1 in all drawing functions
void* Renderer::getDebugRenderer(void)
{
	// only do this once
	static void* dbgRenderer = nullptr;
	if (dbgRenderer == nullptr) {
		// typdef and call at our OFFSET_DEBUGRENDERER
		typedef void* (__thiscall* fb__DebugRenderManager_getThreadContext_t)(void);
		fb__DebugRenderManager_getThreadContext_t fb__DebugRenderManager_getThreadContext = (fb__DebugRenderManager_getThreadContext_t)StaticOffsets::Get_OFFSET_DEBUGRENDERER2();
		dbgRenderer = fb__DebugRenderManager_getThreadContext();
	}
	return dbgRenderer;
}

// the setup function
void Renderer::setup(tRENDERERCALLBACK callback) {
	// check if this has been called more than once
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		// set the user-supplied callback function
		Renderer::callbackFunction = callback;
		// make a new Renderer*
		Renderer::pRenderer = new Renderer();

		// apply a VTable Hook to DXRenderer's 13th vtable function
		Candy::CreateVTableHook<tEndFrame>(*(void**)StaticOffsets::Get_OFFSET_DXRENDERER(), hkEndFrame, oEndFrame, 13);
	}
}

void Renderer::shutdown() {
	tEndFrame ohEndFrame;
	Candy::CreateVTableHook<tEndFrame>(*(void**)StaticOffsets::Get_OFFSET_DXRENDERER(), oEndFrame, ohEndFrame , 13);
}

void Renderer::drawRect(float x, float y, float width, float height, Color32 color)
{
	// setup a vec2, and call the engine function
	Vec2 f = { x, y }, t = { x + width, y + height };
	((void(__fastcall *)(void*, Vec2*, Vec2*, Color32))StaticOffsets::Get_OFFSET_DRAWRECT2D())(this->getDebugRenderer(), &t, &f, color);
}

void Renderer::drawText(int x, int y, Color32 color, char* text, float scale)
{
	// call the DebugRenderer::DrawText function
	((void(__fastcall*)(void*, int, int, char*, Color32, float))StaticOffsets::Get_OFFSET_DRAWTEXT())(this->getDebugRenderer(), x, y, text, color, scale);
}

void Renderer::drawText(int x, int y, Color32 color, const std::string& text, float scale) {
	// call the above function
	this->drawText(x, y, color, (char*)text.c_str(), scale);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, Color32 color)
{
	// setup our two vectors, then call the DebugRenderer::DrawLine function
	Vec2 f = { x1,x2 }, t = { x2, y2 };
	((void(__fastcall *)(void*, Vec2*, Vec2*, Color32))StaticOffsets::Get_OFFSET_DRAWLINE())(this->getDebugRenderer(), &f, &t, color);
}

// static stuff we declared earlier
Renderer* Renderer::pRenderer = nullptr;
Renderer::tRENDERERCALLBACK Renderer::callbackFunction = nullptr;