#pragma once
#include "Renderer.hpp"
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
	Color32 backgroundColor = { 0x33, 0x33, 0x55, 192 };
	Color32 foregroundColor = { 255, 255, 255, 255 };
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
	int min = 0;
	int max = 20000;

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
		Color32 backgroundColor = { 0x55, 0x55, 0x88, 0xff };
		Color32 foregroundColor = { 0xff, 0xff, 0xff, 0xff };
	} header;

	// get the current mouse position on the screen, get's called once per frame
	void getCurrentMousePosition() {
		
		GetCursorPos(&this->cursor);
	//	ScreenToClient(g_windowHandle, &this->cursor);
	}

	// the draw function, which gets called in the EndFrame hook
	void draw(Renderer* pRenderer) {
		// get the mouse position
		this->getCurrentMousePosition();

		// determine if the mouse is clicking
		bool isClicking = KeyMan::ReadKeyOnce(VK_LBUTTON, 150);

		// get the total width and height
		int totalHeight = this->header.height;
		for (auto& e : this->elements) {
			totalHeight += e.size + e.style.padding.top + e.style.padding.bottom + e.style.margin.top + e.style.margin.bottom;
		}
		// draw the menu
		pRenderer->drawRect(this->absolutePos.x, this->absolutePos.y, this->width, totalHeight, this->style.backgroundColor);
		
		// draw the header
		pRenderer->drawRect(this->absolutePos.x, this->absolutePos.y, this->width, this->header.height, this->header.backgroundColor);
		pRenderer->drawText(this->absolutePos.x + 5, this->absolutePos.y + 7, this->header.foregroundColor, this->header.text, 1.25f);
		
		// height value we'll increment for each element
		int currentHeight = this->absolutePos.y + this->header.height;
		for (auto& e : this->elements) {
			// draw the text of the element
			pRenderer->drawText(this->absolutePos.x + e.style.margin.left + e.style.padding.left,
				currentHeight + e.style.margin.top + e.style.padding.top, 
				e.style.foregroundColor,
				e.text,
				e.style.fontScale);

			// handle based on what type it is
			if (e.type == Element::ElementType::checkBox) {
				int objX = this->absolutePos.x + this->width - (e.style.padding.right + e.style.margin.right + e.size);
				int objY = currentHeight + e.style.margin.top + e.style.padding.top;
				int objW = e.size - (e.style.padding.top + e.style.padding.bottom);
				int objH = e.size - (e.style.padding.top + e.style.padding.bottom);

				// draw the checkbox
				pRenderer->drawRect(objX, objY, objW, objH, e.style.foregroundColor);

				// check if the value points to something
				if (e.value != nullptr) {
					// get it's boolean value
					bool checked = *(bool*)e.value;

					// if it's checked, draw another rectangle inside it
					if (checked) {
						pRenderer->drawRect((this->absolutePos.x + this->width - (e.style.padding.right + e.style.margin.right + e.size)) + 2,
							(currentHeight + e.style.margin.top + e.style.padding.top) + 2,
							(e.size - (e.style.padding.top + e.style.padding.bottom)) - 4,
							(e.size - (e.style.padding.top + e.style.padding.bottom)) - 4,
							e.style.backgroundColor);
					}
				}
				// if the mouse is currently clicking
				if (isClicking) {
					// check if it's clicking within the bounds of this element
					if ((this->cursor.x > this->absolutePos.x) &&
						(this->cursor.y > objY) &&
						(this->cursor.x < this->absolutePos.x + this->width) &&
						(this->cursor.y < objY + objH)) {
						// if so, toggle the bool
						*(bool*)e.value = !*(bool*)e.value;
					}
				}
			}
			// inc the height
			currentHeight += e.style.margin.top + e.style.margin.bottom + e.size + e.style.padding.top + e.style.padding.bottom;
		} // elements enumerator

		// drag window around
		if (KeyMan::ReadKey(VK_LBUTTON)) {
			// check if the mouse is clicking, and within the header
			if ((this->cursor.x > this->absolutePos.x) &&
				(this->cursor.y > this->absolutePos.y) &&
				(this->cursor.x < this->absolutePos.x + this->width) &&
				(this->cursor.y < this->absolutePos.y + this->header.height)) {

				// set the middle of the header to the mouse's current location
				this->absolutePos.x = this->cursor.x - (this->width/2);
				this->absolutePos.y = this->cursor.y - (this->header.height/2);
			}
		}
	} // render
};