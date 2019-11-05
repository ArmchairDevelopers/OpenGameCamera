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

		// determine if the mouse is clicking
		bool isClicking = KeyMan::ReadKey(VK_LBUTTON);

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
				pRenderer->drawRect(objX, objY, objW, objH, defaultStyle.foregroundColor);

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
							defaultStyle.accentColor);
					}
					else {
						pRenderer->drawRect((this->absolutePos.x + this->width - (e.style.padding.right + e.style.margin.right + e.size)) + 2,
							(currentHeight + e.style.margin.top + e.style.padding.top) + 2,
							(e.size - (e.style.padding.top + e.style.padding.bottom)) - 4,
							(e.size - (e.style.padding.top + e.style.padding.bottom)) - 4,
							defaultStyle.backgroundColor);
					}
				}
				// if the mouse is currently clicking
				if (isClicking && clock() - Menu::lastClick > Menu::clickTimer) {
					// check if it's clicking within the bounds of this element
					if ((this->cursor.x > this->absolutePos.x) &&
						(this->cursor.y > objY) &&
						(this->cursor.x < this->absolutePos.x + this->width) &&
						(this->cursor.y < objY + objH)) {
						// if so, toggle the bool
						*(bool*)e.value = !*(bool*)e.value;
						Menu::lastClick = clock();
					}
				}
			} // checkbox
			if (e.type == Element::ElementType::floatSlider) {
				if (e.value != nullptr) {
					int xPos = (this->absolutePos.x + this->width - (e.style.padding.right + e.style.margin.right + e.size));
					int yPos = (currentHeight + e.style.margin.top + e.style.padding.top);
					int boxw = (e.size - (e.style.padding.top + e.style.padding.bottom));
					const int leftOffset = 120;
					const int textOffset = 100;
					// left box

					pRenderer->drawRect(xPos - leftOffset, yPos, boxw, boxw, defaultStyle.accentColor);
					pRenderer->drawText(xPos - leftOffset + 1, yPos - 2, defaultStyle.foregroundColor, "-", 1);

					float flValue = *(float*)e.value;
					pRenderer->drawText(xPos - textOffset, yPos, e.style.foregroundColor, std::to_string(flValue), 1.f);
					
					// right box
					pRenderer->drawRect(xPos, yPos, boxw, boxw, defaultStyle.accentColor);
					pRenderer->drawText(xPos + 1, yPos -2, defaultStyle.foregroundColor, "+", 1);

					if (isClicking && clock() - Menu::lastClick > Menu::clickTimer) {
						float amount = e.step;
						if (KeyMan::ReadKey(Keys::speedUpCamera)) amount *= 3;
						if (KeyMan::ReadKey(Keys::slowDownCamera)) amount *= .25;

						// left box click
						if ((this->cursor.x > xPos - leftOffset) &&
							(this->cursor.x < xPos + boxw - leftOffset) &&
							(this->cursor.y > yPos) &&
							(this->cursor.y < yPos + boxw)) {
							if (flValue - amount < e.min) {
								*(float*)e.value = e.min;
							}
							else {
								*(float*)e.value = flValue - amount;
							}
							Menu::lastClick = clock();

						}
						// right box click
						if ((this->cursor.x > xPos) &&
							(this->cursor.x < xPos + boxw) &&
							(this->cursor.y > yPos) &&
							(this->cursor.y < yPos + boxw)) {
							if (flValue + amount > e.max) {
								*(float*)e.value = e.max;
							}
							else {
								*(float*)e.value = flValue + amount;
							}
							Menu::lastClick = clock();
						}
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
time_t Menu::lastClick = clock();
time_t Menu::clickTimer = 200;