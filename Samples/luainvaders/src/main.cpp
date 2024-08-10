/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "DecoratorInstancerDefender.h"
#include "DecoratorInstancerStarfield.h"
#include "ElementGame.h"
#include "HighScores.h"
#include "LuaInterface.h"
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi/Lua.h>
#include <Shell.h>

Rml::Context* context = nullptr;

void GameLoop()
{
	context->Update();

	Shell::BeginFrame();
	context->Render();
	Shell::PresentFrame();
}

#if defined RMLUI_PLATFORM_WIN32
	#include <RmlUi_Include_Windows.h>
int APIENTRY WinMain(HINSTANCE /*instance_handle*/, HINSTANCE /*previous_instance_handle*/, char* /*command_line*/, int /*command_show*/)
#else
int main(int /*argc*/, char** /*argv*/)
#endif
{
	int window_width = 1024;
	int window_height = 768;

	// Initializes and sets the system and render interfaces, creates a window, and attaches the renderer.
	if (!Shell::Initialize() || !Shell::OpenWindow("RmlUi Invaders from Mars (Lua Powered)", window_width, window_height, false))
	{
		Shell::Shutdown();
		return -1;
	}

	// RmlUi initialisation.
	Rml::Initialise();

	// Initialise the Lua interface
	Rml::Lua::Initialise();

	// Create the main RmlUi context and set it on the shell's input layer.
	context = Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));
	if (context == nullptr)
	{
		Rml::Shutdown();
		Shell::Shutdown();
		return -1;
	}

	Rml::Debugger::Initialise(context);
	Shell::SetContext(context);

	// Load the font faces required for Invaders.
	Shell::LoadFonts();

	// Register Invader's custom decorator instancers.
	DecoratorInstancerStarfield decorator_starfield;
	DecoratorInstancerDefender decorator_defender;
	Rml::Factory::RegisterDecoratorInstancer("starfield", &decorator_starfield);
	Rml::Factory::RegisterDecoratorInstancer("defender", &decorator_defender);

	// Construct the game singletons.
	HighScores::Initialise(context);

	// Fire off the startup script.
    LuaInterface::Initialise(Rml::Lua::Interpreter::GetLuaState()); //the tables/functions defined in the samples
    Rml::Lua::Interpreter::LoadFile(Rml::String("luainvaders/lua/start.lua"));

	Shell::EventLoop(GameLoop);	

	// Shut down the game singletons.
	HighScores::Shutdown();

	// Shutdown RmlUi.
	Rml::Shutdown();

	Shell::CloseWindow();
	Shell::Shutdown();

	return 0;
}
