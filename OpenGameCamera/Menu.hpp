#pragma once

#include "renderer.h"
#include <vector>
#include <string>
#include "KeyMan.hpp"

// define a basic layout for padding/margins
template <typename T>
struct Layout {
	T top;
	T right;
	T bottom;
	T left;
};

// our style class
class Style {
public:
	Layout<int32_t> padding = { 2, 2, 2, 2 };
	Layout<int32_t> margin = { 2, 2, 2, 2 };
	float fontScale = 1;
	Color32 backgroundColor = { 00, 00, 00, 255 };
	Color32 foregroundColor = { 0xdc, 0xdd, 0xde, 255 };
	Color32 accentColor = { 0xe7, 0x3a, 0x39, 0xff };
} defaultStyle; // default style

// an element.  Can be a label, checkbox, int slider, float slider, or button
class Element {
public:
	enum class ElementType {
		label,
		checkBox,
		intSlider,
		floatSlider,
		button
	} type = ElementType::label;

	// the style of the element
	Style style = defaultStyle;

	// void* to the value.  Determined at runtime what to parse it as based off of the 'type' member.
	// for buttons, the void* needs to be a void(void) function pointer
	void* value = nullptr;

	// the text to display
	std::string text;

	// the minimum and maximum values for an int
	float min = 0;
	float max = 10;
	float step = .5;
	// determines the height, and width for checkboxes
	int size = 15;
};

class Menu {
public:
	// default style
	Style style = defaultStyle;

	// vector of elements
	std::vector<Element> elements = {};

	// screen coordinates of the menu
	Vec2 absolutePos = { 100, 100 };

	// the width of the menu
	int width = 400;

	// our current cursor
	POINT cursor;

	// info about the header bar (title)
	struct Header {
		int height = 35;
		std::string text = "Menu";
		Color32 backgroundColor = defaultStyle.accentColor;
		Color32 foregroundColor = defaultStyle.backgroundColor;
	} header;

	// get the current mouse position on the screen, get's called once per frame
	void getCurrentMousePosition() {
		
		GetCursorPos(&this->cursor);
	//	ScreenToClient(g_windowHandle, &this->cursor);
	}

	static time_t lastClick;
	static time_t clickTimer;
	// the draw function, which gets called in the EndFrame hook
	void draw(Renderer* pRenderer) {
		// get the mouse position
		this->getCurrentMousePosition();
	} // render
};
time_t Menu::lastClick = clock();
time_t Menu::clickTimer = 200;