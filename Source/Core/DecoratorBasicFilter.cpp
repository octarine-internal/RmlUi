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

#include "DecoratorBasicFilter.h"
#include "../../Include/RmlUi/Core/Element.h"
#include "../../Include/RmlUi/Core/ElementUtilities.h"
#include "../../Include/RmlUi/Core/PropertyDefinition.h"
#include "../../Include/RmlUi/Core/RenderInterface.h"
#include "ComputeProperty.h"
#include "DecoratorElementData.h"

namespace Rml {

DecoratorBasicFilter::DecoratorBasicFilter() {}

DecoratorBasicFilter::~DecoratorBasicFilter() {}

bool DecoratorBasicFilter::Initialise(const String& in_name, float in_value)
{
	name = in_name;
	value = in_value;
	return true;
}

DecoratorDataHandle DecoratorBasicFilter::GenerateElementData(Element* element) const
{
	RenderInterface* render_interface = element->GetRenderInterface();
	if (!render_interface)
		return INVALID_DECORATORDATAHANDLE;

	CompiledFilterHandle handle = render_interface->CompileFilter(name, Dictionary{{"value", Variant(value)}});

	BasicFilterElementData* element_data = GetBasicFilterElementDataPool().AllocateAndConstruct(render_interface, handle);
	return reinterpret_cast<DecoratorDataHandle>(element_data);
}

void DecoratorBasicFilter::ReleaseElementData(DecoratorDataHandle handle) const
{
	BasicFilterElementData* element_data = reinterpret_cast<BasicFilterElementData*>(handle);
	RMLUI_ASSERT(element_data && element_data->render_interface);

	element_data->render_interface->ReleaseCompiledFilter(element_data->filter);
	GetBasicFilterElementDataPool().DestroyAndDeallocate(element_data);
}

void DecoratorBasicFilter::RenderElement(Element* /*element*/, DecoratorDataHandle handle) const
{
	BasicFilterElementData* element_data = reinterpret_cast<BasicFilterElementData*>(handle);
	element_data->render_interface->AttachFilter(element_data->filter);
}

DecoratorBasicFilterInstancer::DecoratorBasicFilterInstancer(ValueType value_type, const char* default_value) :
	DecoratorInstancer(DecoratorClass::Filter), ids{}
{
	switch (value_type)
	{
	case ValueType::NumberPercent:
		ids.value = RegisterProperty("value", default_value).AddParser("number_percent").GetId();
		break;
	case ValueType::Angle:
		ids.value = RegisterProperty("value", default_value).AddParser("angle").GetId();
		break;
	}

	RegisterShorthand("decorator", "value", ShorthandType::FallThrough);
}

DecoratorBasicFilterInstancer::~DecoratorBasicFilterInstancer() {}

SharedPtr<Decorator> DecoratorBasicFilterInstancer::InstanceDecorator(const String& name, const PropertyDictionary& properties_,
	const DecoratorInstancerInterface& /*interface_*/)
{
	const Property* p_value = properties_.GetProperty(ids.value);
	if (!p_value)
		return nullptr;

	float value = p_value->Get<float>();
	if (p_value->unit == Unit::PERCENT)
		value *= 0.01f;
	else if (p_value->unit == Unit::DEG)
		value = Rml::Math::DegreesToRadians(value);

	auto decorator = MakeShared<DecoratorBasicFilter>();
	if (decorator->Initialise(name, value))
		return decorator;

	return nullptr;
}

} // namespace Rml
