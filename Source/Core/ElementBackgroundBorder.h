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

#ifndef RMLUI_CORE_ELEMENTBACKGROUNDBORDER_H
#define RMLUI_CORE_ELEMENTBACKGROUNDBORDER_H

#include "../../Include/RmlUi/Core/Box.h"
#include "../../Include/RmlUi/Core/Geometry.h"
#include "../../Include/RmlUi/Core/Types.h"

namespace Rml {

class ElementBackgroundBorder {
public:
	ElementBackgroundBorder();
	~ElementBackgroundBorder();

	void Render(Element* element);

	void DirtyBackground();
	void DirtyBorder();

	Geometry* GetClipGeometry(Element* element, BoxArea clip_area);

private:
	enum class BackgroundType { Main, BoxShadow, ClipBorder, ClipPadding, ClipContent, Count };
	struct Background {
		Background(Element* element) : geometry(element) {}
		Geometry geometry;
		Texture texture;
	};

	void GenerateGeometry(Element* element);
	void GenerateBoxShadow(Element* element, ShadowList shadow_list, Vector4f border_radius, float opacity);

	Geometry* GetGeometry(BackgroundType type);
	Background& GetOrCreateBackground(Element* element, BackgroundType type);

	bool background_dirty = false;
	bool border_dirty = false;

	Array<UniquePtr<Background>, (size_t)BackgroundType::Count> geometries;
};

} // namespace Rml
#endif
