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

#include "DecoratorDropShadow.h"
#include "../../Include/RmlUi/Core/Element.h"
#include "../../Include/RmlUi/Core/ElementUtilities.h"
#include "../../Include/RmlUi/Core/PropertyDefinition.h"
#include "../../Include/RmlUi/Core/RenderInterface.h"
#include "ComputeProperty.h"
#include "DecoratorElementData.h"

namespace Rml {

DecoratorDropShadow::DecoratorDropShadow() {}

DecoratorDropShadow::~DecoratorDropShadow() {}

bool DecoratorDropShadow::Initialise(Colourb in_color, NumericValue in_offset_x, const NumericValue in_offset_y, NumericValue in_sigma)
{
	color = in_color;
	value_offset_x = in_offset_x;
	value_offset_y = in_offset_y;
	value_sigma = in_sigma;
	return Any(in_offset_x.unit & Unit::LENGTH) && Any(in_offset_y.unit & Unit::LENGTH) && Any(in_sigma.unit & Unit::LENGTH);
}

DecoratorDataHandle DecoratorDropShadow::GenerateElementData(Element* element) const
{
	RenderInterface* render_interface = element->GetRenderInterface();
	if (!render_interface)
		return INVALID_DECORATORDATAHANDLE;

	const float sigma = element->ResolveLength(value_sigma);
	const Vector2f offset = {
		element->ResolveLength(value_offset_x),
		element->ResolveLength(value_offset_y),
	};

	CompiledFilterHandle handle =
		render_interface->CompileFilter("drop-shadow", Dictionary{{"color", Variant(color)}, {"offset", Variant(offset)}, {"sigma", Variant(sigma)}});

	BasicFilterElementData* element_data = GetBasicFilterElementDataPool().AllocateAndConstruct(render_interface, handle);
	return reinterpret_cast<DecoratorDataHandle>(element_data);
}

void DecoratorDropShadow::ReleaseElementData(DecoratorDataHandle handle) const
{
	BasicFilterElementData* element_data = reinterpret_cast<BasicFilterElementData*>(handle);
	RMLUI_ASSERT(element_data && element_data->render_interface);

	element_data->render_interface->ReleaseCompiledFilter(element_data->filter);
	GetBasicFilterElementDataPool().DestroyAndDeallocate(element_data);
}

void DecoratorDropShadow::RenderElement(Element* /*element*/, DecoratorDataHandle handle) const
{
	BasicFilterElementData* element_data = reinterpret_cast<BasicFilterElementData*>(handle);
	element_data->render_interface->AttachFilter(element_data->filter);
}

void DecoratorDropShadow::ModifyScissorRegion(Element* element, Rectanglef& scissor_region) const
{
	// Expand the scissor region to cover both the native element *and* its offset shadow w/blur.

	const float sigma = element->ResolveLength(value_sigma);
	const Vector2f offset = {
		element->ResolveLength(value_offset_x),
		element->ResolveLength(value_offset_y),
	};

	const float blur_radius = 2.f * sigma;
	const float blur_extent = 1.5f * blur_radius;
	scissor_region.ExtendTopLeft(Math::Max(-offset, Vector2f(0.f)) + Vector2f(blur_extent));
	scissor_region.ExtendBottomRight(Math::Max(offset, Vector2f(0.f)) + Vector2f(blur_extent));
}

DecoratorDropShadowInstancer::DecoratorDropShadowInstancer() : DecoratorInstancer(DecoratorClass::Filter)
{
	ids.color = RegisterProperty("color", "transparent").AddParser("color").GetId();
	ids.offset_x = RegisterProperty("offset-x", "0px").AddParser("length").GetId();
	ids.offset_y = RegisterProperty("offset-y", "0px").AddParser("length").GetId();
	ids.sigma = RegisterProperty("sigma", "0px").AddParser("length").GetId();
	RegisterShorthand("decorator", "color, offset-x, offset-y, sigma", ShorthandType::FallThrough);
}

DecoratorDropShadowInstancer::~DecoratorDropShadowInstancer() {}

SharedPtr<Decorator> DecoratorDropShadowInstancer::InstanceDecorator(const String& /*name*/, const PropertyDictionary& properties_,
	const DecoratorInstancerInterface& /*interface_*/)
{
	const Property* p_color = properties_.GetProperty(ids.color);
	const Property* p_offset_x = properties_.GetProperty(ids.offset_x);
	const Property* p_offset_y = properties_.GetProperty(ids.offset_y);
	const Property* p_sigma = properties_.GetProperty(ids.sigma);
	if (!p_color || !p_offset_x || !p_offset_y || !p_sigma)
		return nullptr;

	auto decorator = MakeShared<DecoratorDropShadow>();
	if (decorator->Initialise(p_color->Get<Colourb>(), p_offset_x->GetNumericValue(), p_offset_y->GetNumericValue(), p_sigma->GetNumericValue()))
		return decorator;

	return nullptr;
}
} // namespace Rml
