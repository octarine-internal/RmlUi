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

#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <Shell.h>

#include "FontEngineInterfaceBitmap.h"

/*

	This demo shows how to create a custom bitmap font engine implementation. 
	
	It should work even when RmlUi is compiled without the default font engine (see CMake flag 'NO_FONT_INTERFACE_DEFAULT').
	See the interface in 'FontEngineInterfaceBitmap.h' and the implementation in 'FontEngineBitmap.h'.

*/

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
	if (!Shell::Initialize() || !Shell::OpenWindow("Bitmap Font Sample", window_width, window_height, true))
	{
		Shell::Shutdown();
		return -1;
	}

	// Construct and load the font interface.
	auto font_interface = Rml::MakeUnique<FontEngineInterfaceBitmap>();
	Rml::SetFontEngineInterface(font_interface.get());

	// RmlUi initialisation.
	Rml::Initialise();

	// Create the main RmlUi context.
	context = Rml::CreateContext("main", Rml::Vector2i(window_width, window_height));
	if (!context)
	{
		Rml::Shutdown();
		Shell::Shutdown();
		return -1;
	}

	Rml::Debugger::Initialise(context);
	Shell::SetContext(context);

    // Load bitmap font
	if (!Rml::LoadFontFace("basic/bitmapfont/data/Comfortaa_Regular_22.fnt"))
	{
		Rml::Shutdown();
		Shell::Shutdown();
		return -1;
	}

	// Load and show the demo document.
	if (Rml::ElementDocument * document = context->LoadDocument("basic/bitmapfont/data/bitmapfont.rml"))
	{
		if (auto el = document->GetElementById("title"))
			el->SetInnerRML("Bitmap font");

		document->Show();
	}

	Shell::EventLoop(GameLoop);

	// Shutdown RmlUi.
	Rml::Shutdown();

	// Destroy the font interface before taking down the shell, this way font textures are properly released through the render interface.
	font_interface.reset();

	Shell::CloseWindow();
	Shell::Shutdown();

	return 0;
}
