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

#include "DecoratorDefender.h"
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/GeometryUtilities.h>
#include <RmlUi/Core/Math.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/Texture.h>

DecoratorDefender::~DecoratorDefender() {}

bool DecoratorDefender::Initialise(const Rml::Texture& texture)
{
	image_index = AddTexture(texture);
	if (image_index == -1)
	{
		return false;
	}

	return true;
}

/// Called on a decorator to generate any required per-element data for a newly decorated element.
Rml::DecoratorDataHandle DecoratorDefender::GenerateElementData(Rml::Element* RMLUI_UNUSED_PARAMETER(element)) const
{
	RMLUI_UNUSED(element);

	return Rml::Decorator::INVALID_DECORATORDATAHANDLE;
}

// Called to release element data generated by this decorator.
void DecoratorDefender::ReleaseElementData(Rml::DecoratorDataHandle RMLUI_UNUSED_PARAMETER(element_data)) const
{
	RMLUI_UNUSED(element_data);
}

// Called to render the decorator on an element.
void DecoratorDefender::RenderElement(Rml::Element* element, Rml::DecoratorDataHandle RMLUI_UNUSED_PARAMETER(element_data)) const
{
	RMLUI_UNUSED(element_data);

	Rml::Vector2f position = element->GetAbsoluteOffset(Rml::BoxArea::Padding);
	Rml::Vector2f size = element->GetBox().GetSize(Rml::BoxArea::Padding);
	Rml::Math::SnapToPixelGrid(position, size);

	if (Rml::RenderInterface* render_interface = element->GetRenderInterface())
	{
		Rml::TextureHandle texture = GetTexture(image_index)->GetHandle(render_interface);
		Rml::Colourb color = element->GetProperty<Rml::Colourb>("color");

		Rml::Vertex vertices[4];
		int indices[6];
		Rml::GeometryUtilities::GenerateQuad(vertices, indices, Rml::Vector2f(0.f), size, color);

		render_interface->RenderGeometry(vertices, 4, indices, 6, texture, position);
	}
}
