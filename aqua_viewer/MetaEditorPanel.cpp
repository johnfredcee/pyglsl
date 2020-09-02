
#include "StdAfx.h"

#include <Utils/stringfunctions.h>
#include "SceneManager.h"
#include "MetaEditorPanel.h"

using namespace aqua;

const u32 METADATA_TYPE_PBCOLOR(CalcCRC("aqua::PbColor"));

IMPLEMENT_CLASS(MetaEditorPanel, wxPanel);

BEGIN_EVENT_TABLE(MetaEditorPanel, wxPanel)
	EVT_SIZE(MetaEditorPanel::OnSize)
	EVT_PG_CHANGED(wxID_ANY, MetaEditorPanel::OnChanged)
END_EVENT_TABLE()

#pragma warning( disable : 4355 )
MetaEditorPanel::MetaEditorPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) 
: wxPanel(parent, id, pos, size, style, wxT("MetaEditorPanel")),
	propertyGrid_(new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE))
{
	propertyGrid_->SetSize(GetClientSize());
	propertyGrid_->Freeze();
	wxPropertyCategory* category(new wxPropertyCategory("Scene"));
	addProperty(category, NULL);
	propertyGrid_->Thaw();
}
#pragma warning( default : 4355 )

MetaEditorPanel::~MetaEditorPanel()
{
}

void MetaEditorPanel::setMetaData(aqua::shared_ptr<DynamicMetaData> metaData) 
{
	metaData_ = metaData;
}

void MetaEditorPanel::refresh(const SceneManager::sceneIds& ids)
{
	propertyGrid_->Clear();
	propertyGrid_->Freeze();
	wxPropertyCategory* category(new wxPropertyCategory("Scene"));
	addProperty(category, NULL);
	for(SceneManager::sceneIds::const_iterator it = ids.begin();
		it != ids.end();
		++it) {
		DynamicMetaParser metaParser(*metaData_, *it);
		PBASSERT(metaParser.IsValid(), "Invalid id");
		addMetaProperty(metaParser, category, TYPE_NORMAL, it - ids.begin());
	};
	propertyGrid_->Thaw();
}

void MetaEditorPanel::OnSize(wxSizeEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	
	if (propertyGrid_) {
		propertyGrid_->SetSize(GetClientSize());
	}
}

void MetaEditorPanel::OnChanged(wxPropertyGridEvent& event)
{
	PBUNREFERENCED_PARAMETER(event);
	
	if (propertyGrid_->GetSelectedProperty()) {
		wxPGProperty* property(propertyGrid_->GetSelectedProperty());
		
		wxString fullName(property->GetName());
		wxLogDebug("Setting Property %s ", fullName.c_str());
		wxString instanceName(fullName.BeforeFirst(wxT('.')));
		wxString valueName(fullName.AfterFirst(wxT('.')));
		if (!valueName.AfterFirst(wxT('.')).IsEmpty())
			valueName = valueName.AfterFirst(wxT('.'));
		DynamicMetaParser instance(*metaData_, instanceName.c_str());
		if (instance.IsValid()) {
			DynamicMetaParser value(instance, valueName.c_str());
			setValue(value, property);
		}
		else {
			PBBREAK("Cannot find '" << std::string(fullName.c_str()) << "' to re-assign");
		}
	}
}

void MetaEditorPanel::setValue(DynamicMetaParser& parser, wxPGProperty* property)
{
	const u32& itrTypeId(parser.GetTypeId());
		
	if (itrTypeId == METADATA_TYPE_CRC) { // HashString
		parser.SetValue(std::string(property->GetValue().GetString()).c_str());
	}
	else if (itrTypeId == METADATA_TYPE_STRING) {
		parser.SetValue(std::string(property->GetValue().GetString()).c_str());
	}
	else if (itrTypeId == METADATA_TYPE_F32) {
		parser.SetValue(property->GetValue().GetDouble());
	}
	else if (itrTypeId == METADATA_TYPE_U32) {
		parser.SetValue(property->GetValue().GetInteger());
	}
	else if (itrTypeId == METADATA_TYPE_S32) {
		parser.SetValue(property->GetValue().GetInteger());
	}
	else if (itrTypeId == METADATA_TYPE_BOOL) {
		parser.SetValue(property->GetValue().GetBool());
	}
	else if (itrTypeId == METADATA_TYPE_PBCOLOR) {
		wxVariant value(property->GetValue());
		wxColour colour;
		colour << value;

		const Float32 colourR(static_cast<Float32>(colour.Red()) / 255.0F);
		const Float32 colourG(static_cast<Float32>(colour.Green()) / 255.0F);
		const Float32 colourB(static_cast<Float32>(colour.Blue()) / 255.0F);

		DynamicMetaParser::Iterator itr(parser);
		itr.SetValue(colourR);
		itr.Next();
		itr.SetValue(colourG);
		itr.Next();
		itr.SetValue(colourB);
	}
	else	{
		// I must be an Enum... check or panic..
		const MetaData* metaData(static_cast<MetaParser>(parser).GetMetaData());
		const MetaEnum* metaEnum(metaData->FindEnum(itrTypeId));
		if (metaEnum) {
			parser.SetValue(property->GetValue().GetInteger());
		}
		else {
			const char* string(metaData->FindString(itrTypeId));
			if (string) {
				PBBREAK("Unsupported Type: " << string);
			}
			else {
				PBBREAK("Unsupported Type: " << unsignedIntToHexString(itrTypeId));
			}
		}
	}
}

wxPGProperty *MetaEditorPanel::addProperty(wxPGProperty* child, wxPGProperty* parent)
{
	wxPGProperty *result = NULL;
	if (parent) {
		result = propertyGrid_->AppendIn(parent, child);
	} else {
		result = propertyGrid_->Append(child);
	}
	PBASSERT(result != NULL, "Poropety Grid failure");
	return result;
}

void MetaEditorPanel::addColourProperty(DynamicMetaParser& parser, wxPGProperty* parent, const std::string& objectName)
{
	DynamicMetaParser::Iterator itr(parser);

	Byte colourR(static_cast<Byte>(itr.GetValue<Float32>() * 255.0F));
	itr.Next();
	Byte colourG(static_cast<Byte>(itr.GetValue<Float32>() * 255.0F));
	itr.Next();
	Byte colourB(static_cast<Byte>(itr.GetValue<Float32>() * 255.0F));
	itr.Next();

	const std::string originalName(parent->GetParent()->GetName().c_str());

	wxPGProperty* colourProperty(new wxColourProperty(wxT("Colour"), std::string(originalName + "." + objectName).c_str(), wxColour(colourR, colourG, colourB)));
	addProperty(colourProperty, parent);

	wxPGProperty* floatProperty(new wxFloatProperty(wxT("Alpha"), std::string(objectName + "." + "a").c_str(), itr.GetValue<Float32>()));
	addProperty(floatProperty, parent);
}

void MetaEditorPanel::addMetaProperty(DynamicMetaParser& parser, wxPGProperty* parent, const PropertyType propertyType, Uint index) {

	(void) propertyType;
	const u32& parserTypeId(parser.GetTypeId());
	// we went off the end of sthing
	if (parserTypeId == METADATA_TYPE_END) { 
		return;
	}
	std::string parentName("");
	if (parent)
		parentName = std::string(parent->GetName());
	std::string displayName(parser.GetName());
	std::string parserName(parser.GetName());
	wxLogDebug("Adding %s ", parser.GetName());
	wxString propertyName(wxString::Format("%s_%s_%d", parentName.c_str(), parserName.c_str(), index));
	wxLogDebug("Property Name %s ", propertyName.c_str());

	if (parserTypeId == METADATA_TYPE_VARARRAY) {
		wxPropertyCategory* category(new wxPropertyCategory(parserName));
		addProperty(category, parent);
		wxPGProperty* intProperty(new wxIntProperty(wxT("Size"), propertyName, parser.GetArrayCount()));			
		addProperty(intProperty, category);
	}
	else if (parserTypeId == METADATA_TYPE_ARRAY) {
		wxPropertyCategory* category(new wxPropertyCategory(parserName));
		addProperty(category, parent);
		wxPGProperty* intProperty(new wxIntProperty(wxT("Size"), propertyName, parser.GetArrayCount()));			
		addProperty(intProperty, category);
	}
	else if ((parserTypeId == METADATA_TYPE_ICRC) || (parserTypeId == METADATA_TYPE_CRC)) { // HashString
		const char* string(parser.GetValue<const char *>()); // This seems to be how it works in the actual Meta Libraries, and this works so...
		if (string) {
			wxPGProperty* hashString(new wxStringProperty(displayName, propertyName, wxString::Format(string)));
			addProperty(hashString, parent);
		}
		else {
			wxPGProperty* hashString(new wxStringProperty(displayName, propertyName, wxString::Format(unsignedIntToHexString(parser.GetValue<u32>()).c_str())));
			addProperty(hashString, parent);
		}
	}
	else if (parserTypeId == METADATA_TYPE_STRING) {
		const char* string(parser.GetValue<const char *>()); // This seems to be how it works in the actual Meta Libraries, and this works so...
		if (string) {
			wxPGProperty* hashString(new wxStringProperty(displayName, propertyName, wxString::Format(string)));
			addProperty(hashString, parent);
		}
		else {
			wxPGProperty* hashString(new wxStringProperty(displayName, propertyName, wxString::Format(unsignedIntToHexString(parser.GetValue<u32>()).c_str())));
			addProperty(hashString, parent);
		}
	}
	else if (parserTypeId == METADATA_TYPE_F32) {
		wxPGProperty* floatProperty(new wxFloatProperty(displayName, propertyName, parser.GetValue<Float32>()));
		addProperty(floatProperty, parent);
	}
	else if (parserTypeId == METADATA_TYPE_U32) {
		wxPGProperty* intProperty(new wxIntProperty(displayName, propertyName, parser.GetValue<u32>()));
		addProperty(intProperty, parent);
	}
	else if (parserTypeId == METADATA_TYPE_S32) {
		wxPGProperty* intProperty(new wxIntProperty(displayName, propertyName, parser.GetValue<s32>()));
		addProperty(intProperty, parent);
	}
	else if (parserTypeId == METADATA_TYPE_BOOL) {
		wxPGProperty* boolProperty(new wxBoolProperty(displayName, propertyName, parser.GetValue<bool>()));
		addProperty(boolProperty, parent);
	} else if (parserTypeId == METADATA_TYPE_BINBLOCK) {
		wxPropertyCategory* category(new wxPropertyCategory(parserName, propertyName));
		addProperty(category, parent);
		wxPGProperty* sizeProperty( new wxIntProperty("size", propertyName + wxString("_size"), parser.Find("size").GetValue<u32>()));
		addProperty(sizeProperty, category);
									
	} else {
		// As per Meta Data madness.. see if it's a meta enum...
		const MetaData* metaData(static_cast<MetaParser>(parser).GetMetaData());
		const MetaEnum* metaEnum(metaData->FindEnum(parser.GetTypeId()));
		if (metaEnum) {
			wxArrayString enumInfo;
			wxArrayInt enumValues;
			for (Uint index(0U); index < metaEnum->m_entries.GetCount(); ++index) {
				const char* string(metaData->FindString(metaEnum->m_entries[index].m_id));
				if (string) {
					enumInfo.Add(wxString::Format(string));
					enumValues.Add(index);
				}
			}
			wxPGProperty* enumProperty(new wxEnumProperty(displayName, propertyName, enumInfo, enumValues, parser.GetValue<u32>()));
			addProperty(enumProperty, parent);
		}
		else {
			DynamicMetaParser::Iterator itr(parser);		
			wxPropertyCategory* category(new wxPropertyCategory(parserName, propertyName));
			addProperty(category, parent);										 
			int subindex = 0;
			while (itr.IsValid()) {
				addMetaProperty(itr, category, TYPE_NORMAL, subindex);
				itr.Next();
				++subindex;
			}			
		}
	}
	return;
}

/*
  void MetaEditorPanel::addMetaProperties(DynamicMetaParser& parser, wxPGProperty* parent, const PropertyType propertyType, Uint index, const std::string objectName)
  {
	
  DynamicMetaParser::Iterator parser(parser);
			
  while (parser.IsValid()) {
  std::string parentName("");
  std::string displayName(parser.GetName());
  if (parent)
  parentName = std::string(std::string(parent->GetName()));
  std::string parserName(parser.GetName());
  if (propertyType == TYPE_ARRAY) {
  if (parentName == parserName || wxString(parentName).AfterLast(wxT('.')) == parserName)
  parentName = objectName;
				
  if (wxString(parentName).BeforeLast(wxT('.')).IsEmpty())
  parentName = parentName + ".";
  displayName = "[" + unsignedIntToString(index) + "]";
  parserName = parentName + parserName + displayName;
  }
  else if (propertyType == TYPE_STRUCT) {
  if (parentName == parserName)
  parentName = objectName;
				
  if (wxString(parentName).AfterLast(wxT(']')).IsEmpty()) {
  parentName = objectName + parentName;
  }
			
  if (wxString(parentName).BeforeLast(wxT('.')).IsEmpty()) {
  parentName = parentName + ".";
  }
  }
  else {
  if (wxString(parentName).BeforeLast(wxT('.')).IsEmpty()) {
  parentName = parentName + ".";
  }
  }
		
  else	{
  // As per Meta Data madness.. see if it's a meta enum...
  const MetaData* metaData(static_cast<MetaParser>(parser).GetMetaData());
  const MetaEnum* metaEnum(metaData->FindEnum(parser.GetTypeId()));
  if (metaEnum) {
  wxArrayString enumInfo;
  wxArrayInt enumValues;
  for (Uint index(0U); index < metaEnum->m_entries.GetCount(); ++index) {
  const char* string(metaData->FindString(metaEnum->m_entries[index].m_id));
  if (string) {
  enumInfo.Add(wxString::Format(string));
  enumValues.Add(index);
  }
  }

  wxPGProperty* enumProperty(new wxEnumProperty(displayName, std::string(parentName + parserName).c_str(), enumInfo, enumValues, parser.GetValue<u32>()));
  addProperty(enumProperty, parent);
  }
  else { // Otherwise assume it's a struct
  wxPropertyCategory* category(new wxPropertyCategory(parserName));
  addProperty(category, parent);
  addProperties(parser, category, TYPE_STRUCT, 0U, parentName);
  }
  }
  parser.Next();
  ++index;
  }
  }
*/
