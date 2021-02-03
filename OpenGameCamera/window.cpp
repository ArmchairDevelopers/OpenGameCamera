#include "window.h"

#include "imgui/imgui.h"
#include "renderer.h"

Window::Window()
{
	Renderer::RegisterEventHandler(this);
};

void Window::Draw()
{
};

void Window::Resize()
{
};