#include "StdAfx.h"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/progdlg.h>

#include "FbxParse.h"
#include "FbxPropertyPanel.h"
#include "SceneClientData.h"

// TO DO -- id will have to be assigned dynamically, and events set up with Connect

const wxWindowID SCENE_PROPERTIES_ID      = wxNewId();

const wxEventType FbxPropertyDoubleClickEventType = wxNewEventType();
const wxEventType FbxNodeChangedEventType         = wxNewEventType();

BEGIN_EVENT_TABLE(FbxPropertyPanel, wxPanel)
	EVT_PG_DOUBLE_CLICK(SCENE_PROPERTIES_ID,	FbxPropertyPanel::PropertyDoubleClick)
	EVT_PG_CHANGED(SCENE_PROPERTIES_ID,		    FbxPropertyPanel::PropertyChanged)
END_EVENT_TABLE()

FbxPropertyPanel::FbxPropertyPanel(wxWindow* parent,
				   wxWindowID id,
				   const wxPoint& pos,
				   const wxSize& size,
				   long style,
				   const wxString& name) : wxPanel(parent, id, pos, size, style, name)
{
	sizer_ = new wxBoxSizer(wxVERTICAL);
	sceneProperties_->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS | wxPG_EX_NATIVE_DOUBLE_BUFFERING);
	sizer_->Add(sceneProperties_, 1, wxEXPAND);
	SetSizer(sizer_);
	Layout();
}
				   
		 
void FbxPropertyPanel::DisplayNode(FbxNode* pNode)
{
	sceneProperties_->Freeze();
	sceneProperties_->Clear();
	wxPGProperty *prop;
	prop = sceneProperties_->Append(new wxPropertyCategory(wxString(pNode->GetName(), wxConvUTF8)));
	sceneProperties_->DisableProperty(prop);
	prop = sceneProperties_->Append(new wxStringProperty(wxT("Name"), wxT("NodeName"), wxString(wxString(pNode->GetName(), wxConvUTF8))));
	sceneProperties_->DisableProperty(prop);
	prop = sceneProperties_->Append(new wxStringProperty(wxT("Type"), wxT("NodeType"), getNodeDescription(pNode)));
	sceneProperties_->DisableProperty(prop);
	prop = sceneProperties_->Append(new wxBoolProperty(wxT("Visible"), wxPG_LABEL, pNode->GetVisibility()));
	sceneProperties_->DisableProperty(prop);
	FbxAMatrix transform;
	// transform = pNode->GetGlobalFromDefault();
	transform = pNode->GetScene()->GetEvaluator()->GetNodeGlobalTransform(pNode);
	FbxVector4 translation;
	translation = transform.GetT();
	FbxVector4 rotation;
	rotation = transform.GetR();
	FbxVector4 scale;
	scale = transform.GetS();
	wxPGProperty *txProp = sceneProperties_->Append(new wxFloatProperty(wxT("Tlt X"), wxPG_LABEL, translation[0]));
	txProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(txProp);
	wxPGProperty *tyProp = sceneProperties_->Append(new wxFloatProperty(wxT("Tlt Y"), wxPG_LABEL, translation[1]));
	tyProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(tyProp);
	wxPGProperty *tzProp = sceneProperties_->Append(new wxFloatProperty(wxT("Tlt Z"), wxPG_LABEL, translation[2]));
	tzProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(tzProp);
	wxPGProperty *rxProp = sceneProperties_->Append(new wxFloatProperty(wxT("Rot X"), wxPG_LABEL,  rotation[0]));
	rxProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(rxProp);
	wxPGProperty *ryProp = sceneProperties_->Append(new wxFloatProperty(wxT("Rot Y"), wxPG_LABEL,  rotation[1]));
	ryProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(ryProp);
	wxPGProperty *rzProp = sceneProperties_->Append(new wxFloatProperty(wxT("Rot Z"), wxPG_LABEL,  rotation[2]));
	rzProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(rzProp);
	wxPGProperty *sxProp = sceneProperties_->Append(new wxFloatProperty(wxT("Scl X"), wxPG_LABEL, scale[0]));
	sxProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(sxProp);
	wxPGProperty *syProp = sceneProperties_->Append(new wxFloatProperty(wxT("Scl Y"), wxPG_LABEL, scale[1]));
	syProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(syProp);
	wxPGProperty *szProp = 	sceneProperties_->Append(new wxFloatProperty(wxT("Scl Z"), wxPG_LABEL, scale[2]));
	szProp->SetClientData(static_cast<void*>(pNode));
	sceneProperties_->DisableProperty(szProp);
	FbxNodeAttribute::EType lAttributeType = pNode->GetNodeAttribute() ? pNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::eUnknown;
	switch (lAttributeType) {
		case FbxNodeAttribute::eMarker:
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		case FbxNodeAttribute::eMesh: {
			sceneProperties_->Append(new wxPropertyCategory(wxT("Mesh")));
			FbxMesh *pMesh = (FbxMesh*) pNode->GetNodeAttribute();
			sceneProperties_->Append(new wxIntProperty(wxT("Layers"), wxT("Layer Count"), pMesh->GetLayerCount()));
			// extract layer properties
			for (int layerIndex = 0; layerIndex < pMesh->GetLayerCount(); ++layerIndex) {
				wxString layerText(wxString::Format(wxT("Layer %02d"), layerIndex));
				sceneProperties_->Append(new wxPropertyCategory(layerText));
				FbxLayer* layer = pMesh->GetLayer(layerIndex);
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Normals"), layerText + wxString("Normals"), layer->GetLayerElementOfType(FbxLayerElement::eNormal) != NULL));
				sceneProperties_->DisableProperty(prop);
				//				sceneProperties_->Append(new wxBoolProperty(wxT("Binormals"), layerText + wxString("Binormals"), layer->GetLayerElementOfType( FbxLayerElement::eBINORMAL ) != NULL ) );
				//				sceneProperties_->Append(new wxBoolProperty(wxT("Tangents"), layerText + wxString("Tangents"), layer->GetLayerElementOfType( FbxLayerElement::eTANGENT ) != NULL ) );
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Materials"), layerText + wxString("Materials"), layer->GetLayerElementOfType(FbxLayerElement::eMaterial) != NULL));
				sceneProperties_->DisableProperty(prop);
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Diffuse UVS"), layerText + wxString("Diffuse UVS"), layer->GetLayerElementOfType(FbxLayerElement::eTextureDiffuse, true) != NULL));
				sceneProperties_->DisableProperty(prop);
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Emissive UVS"), layerText + wxString("Emissive UVS"), layer->GetLayerElementOfType(FbxLayerElement::eTextureEmissive, true) != NULL));
				sceneProperties_->DisableProperty(prop);
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Polygon Groups"), layerText + wxString("Polygon Groups"), layer->GetLayerElementOfType(FbxLayerElement::ePolygonGroup) != NULL));
				sceneProperties_->DisableProperty(prop);
				prop = sceneProperties_->Append(new wxBoolProperty(wxT("Colours"), layerText + wxString("Colours"), layer->GetLayerElementOfType(FbxLayerElement::eVertexColor)  != NULL));
				sceneProperties_->DisableProperty(prop);
			}
			// extract material properties
			wxString materialText(wxString::Format(wxT("Materials %02d"), pNode->GetMaterialCount()));
			sceneProperties_->DisableProperty(sceneProperties_->Append(new wxPropertyCategory(materialText)));
			for (int materialIndex = 0; materialIndex < pNode->GetMaterialCount(); ++materialIndex) {
				FbxDouble3 lFbxDouble3;
				wxPGProperty *prop;
				FbxSurfaceMaterial* mat = pNode->GetMaterial(materialIndex);
				// name property
				wxString nameText(wxString::Format(wxT("Mat %02d Name"), materialIndex));
				wxString matName(wxString(mat->GetName(), wxConvUTF8));
				prop = sceneProperties_->Append(new wxStringProperty(nameText, wxPG_LABEL, matName));
				sceneProperties_->DisableProperty(prop);				
				if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
					wxString matText(wxString::Format(wxT("Mat %02d Type"), materialIndex));
					prop = sceneProperties_->Append(new wxStringProperty(matText, wxPG_LABEL, wxT("Lambert")));
					// We found a Lambert material. Display its properties.
					// Display the Ambient Color
					lFbxDouble3=((FbxSurfaceLambert *)mat)->Ambient;
					wxString ambientText(wxString::Format(wxT("Mat %02d Ambient"), materialIndex));
					prop = sceneProperties_->Append(
						new wxColourProperty(ambientText, wxPG_LABEL,
											 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
													  aqua::Uint8(lFbxDouble3[1] * 256.0),
													  aqua::Uint8(lFbxDouble3[2] * 256.0))));
					sceneProperties_->DisableProperty(prop);
					// Display the Diffuse Color
					lFbxDouble3=((FbxSurfaceLambert *)mat)->Diffuse;
					wxString diffuseText(wxString::Format(wxT("Mat %02d Diffuse"), materialIndex));
					prop = sceneProperties_->Append(
						new wxColourProperty(diffuseText, wxPG_LABEL,
											 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
													  aqua::Uint8(lFbxDouble3[1] * 256.0),
													  aqua::Uint8(lFbxDouble3[2] * 256.0))));
					sceneProperties_->DisableProperty(prop);
					// Display the Emissive Color
					lFbxDouble3=((FbxSurfaceLambert *)mat)->Emissive;
					wxString emissiveText(wxString::Format(wxT("Mat %02d Emissive"), materialIndex));
					prop = sceneProperties_->Append(
						new wxColourProperty(emissiveText, wxPG_LABEL,
											 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
													  aqua::Uint8(lFbxDouble3[1] * 256.0),
													  aqua::Uint8(lFbxDouble3[2] * 256.0))));
					sceneProperties_->DisableProperty(prop);
				} else {

					if (mat->GetClassId().Is(FbxSurfacePhong::ClassId)) {
						wxString matText(wxString::Format(wxT("Mat %02d Type"), materialIndex));
						prop = sceneProperties_->Append(new wxStringProperty(matText, wxPG_LABEL, wxT("Phong")));
						sceneProperties_->DisableProperty(prop);
						// Display the Ambient Color
						lFbxDouble3=((FbxSurfacePhong *)mat)->Ambient;
						wxString ambientText(wxString::Format(wxT("Mat %02d Ambient"), materialIndex));
						prop = sceneProperties_->Append(
							new wxColourProperty(ambientText, wxPG_LABEL,
												 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
														  aqua::Uint8(lFbxDouble3[1] * 256.0),
														  aqua::Uint8(lFbxDouble3[2] * 256.0))));
						sceneProperties_->DisableProperty(prop);
						// Display the Diffuse Color
						lFbxDouble3=((FbxSurfacePhong *)mat)->Diffuse;
						wxString diffuseText(wxString::Format(wxT("Mat %02d Diffuse"), materialIndex));
						prop = sceneProperties_->Append(
							new wxColourProperty(diffuseText, wxPG_LABEL,
												 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
														  aqua::Uint8(lFbxDouble3[1] * 256.0),
														  aqua::Uint8(lFbxDouble3[2] * 256.0))));
						sceneProperties_->DisableProperty(prop);
						// Display the Emissive Color
						lFbxDouble3=((FbxSurfacePhong *)mat)->Emissive;
						wxString emissiveText(wxString::Format(wxT("Mat %02d Emissive"), materialIndex));
						prop = sceneProperties_->Append(
							new wxColourProperty(emissiveText, wxPG_LABEL,
												 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
														  aqua::Uint8(lFbxDouble3[1] * 256.0),
														  aqua::Uint8(lFbxDouble3[2] * 256.0))));
						sceneProperties_->DisableProperty(prop);
						// Display the Specular Color
						lFbxDouble3=((FbxSurfacePhong *)mat)->Specular;
						wxString specularText(wxString::Format(wxT("Mat %02d Specular"), materialIndex));
						prop = sceneProperties_->Append(
							new wxColourProperty(specularText, wxPG_LABEL,
												 wxColour(aqua::Uint8(lFbxDouble3[0] * 256.0),
														  aqua::Uint8(lFbxDouble3[1] * 256.0),
														  aqua::Uint8(lFbxDouble3[2] * 256.0))));
						sceneProperties_->DisableProperty(prop);
						// ambient factor
						wxString ambientFactorText(wxString::Format(wxT("Mat %02d Ambient Factor"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(ambientFactorText, wxPG_LABEL, ((FbxSurfacePhong*)(mat))->AmbientFactor));
						sceneProperties_->DisableProperty(prop);
						// diffuse factor
						wxString diffuseFactorText(wxString::Format(wxT("Mat %02d Diffuse Factor"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(diffuseFactorText, wxPG_LABEL, ((FbxSurfacePhong*)(mat))->DiffuseFactor));
						sceneProperties_->DisableProperty(prop);
						// specular factor
						wxString specularFactorText(wxString::Format(wxT("Mat %02d Specular Factor"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(specularFactorText, wxPG_LABEL, ((FbxSurfacePhong*)(mat))->SpecularFactor));
						sceneProperties_->DisableProperty(prop);
						// transparency
						wxString opaqueText(wxString::Format(wxT("Mat %02d Opacity"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(opaqueText, wxPG_LABEL, 1.0 - ((FbxSurfacePhong*)(mat))->TransparencyFactor));
						sceneProperties_->DisableProperty(prop);
						// shininess property
						wxString shinyText(wxString::Format(wxT("Mat %02d Shininess"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(shinyText, wxPG_LABEL, ((FbxSurfacePhong*)(mat))->Shininess));
						sceneProperties_->DisableProperty(prop);
						prop->SetClientData(static_cast<void*>(pNode));
						// reflectivity property
						wxString reflectText(wxString::Format(wxT("Mat %02d Reflectivity"), materialIndex));
						prop = sceneProperties_->Append(new wxFloatProperty(reflectText, wxPG_LABEL, ((FbxSurfacePhong*)(mat))->ReflectionFactor));
						sceneProperties_->DisableProperty(prop);
					} else {
						wxString matText(wxString::Format(wxT("Mat %02d Type"), materialIndex));
						prop = sceneProperties_->Append(new wxStringProperty(matText, wxPG_LABEL, wxT("Unknown")));
					} // if phong
				} 
			} // for each material
		} // mesh case
			break;
		case FbxNodeAttribute::eCamera: {
			FbxCamera *pCamera = (FbxCamera*) pNode->GetNodeAttribute();
			// position
			FbxDouble3 position = pCamera->Position;
			wxPGProperty *sxProp = sceneProperties_->Append(new wxFloatProperty(wxT("Pos X"), wxPG_LABEL, position[0]));
			sxProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(sxProp);
			wxPGProperty *syProp = sceneProperties_->Append(new wxFloatProperty(wxT("Pos Y"), wxPG_LABEL, position[1]));
			syProp->SetClientData(static_cast<void*>
								  (pNode));
			sceneProperties_->DisableProperty(syProp);
			wxPGProperty *szProp = 	sceneProperties_->Append(new wxFloatProperty(wxT("Pos Z"), wxPG_LABEL, position[2]));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			// up
			FbxDouble3 upVector = pCamera->UpVector.Get();
			sxProp = sceneProperties_->Append(new wxFloatProperty(wxT("Up X"), wxPG_LABEL, upVector[0]));
			sxProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(sxProp);
			syProp = sceneProperties_->Append(new wxFloatProperty(wxT("Up Y"), wxPG_LABEL, upVector[1]));
			syProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(syProp);
			szProp = 	sceneProperties_->Append(new wxFloatProperty(wxT("Up Z"), wxPG_LABEL, upVector[2]));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			// look at
			FbxDouble3 interestPosition = pCamera->InterestPosition.Get();
			sxProp = sceneProperties_->Append(new wxFloatProperty(wxT("Look X"), wxPG_LABEL, interestPosition[0]));
			sxProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(sxProp);
			syProp = sceneProperties_->Append(new wxFloatProperty(wxT("Look Y"), wxPG_LABEL, interestPosition[1]));
			syProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(syProp);
			szProp = sceneProperties_->Append(new wxFloatProperty(wxT("Look Z"), wxPG_LABEL, interestPosition[2]));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			// roll
			FbxDouble roll = pCamera->Roll;
			szProp = sceneProperties_->Append(new wxFloatProperty(wxT("Roll"), wxPG_LABEL, roll));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			// clip planes
			FbxDouble nearPlane = pCamera->NearPlane;
			szProp = sceneProperties_->Append(new wxFloatProperty(wxT("Near "), wxPG_LABEL, nearPlane));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			FbxDouble farPlane = pCamera->FarPlane;
			szProp = sceneProperties_->Append(new wxFloatProperty(wxT("Far "), wxPG_LABEL, farPlane));
			szProp->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(szProp);
			if ((pCamera->GetApertureMode() == FbxCamera::eHorizontal) ||
				(pCamera->GetApertureMode() == FbxCamera::eVertical)) {
				FbxDouble fov = pCamera->FieldOfView;
				szProp = sceneProperties_->Append(new wxFloatProperty(wxT("FOV "), wxPG_LABEL, fov));
				szProp->SetClientData(static_cast<void*>(pNode));
				sceneProperties_->DisableProperty(szProp);
			}
		}
			break;
		case FbxNodeAttribute::eLight: {
			FbxLight *pLight = (FbxLight*) pNode->GetNodeAttribute();
			//FbxLight::ELightType kind = pLight->LightType.Get();
			FbxDouble3 colour = pLight->Color;
			wxPGProperty* prop = sceneProperties_->Append(
				new wxColourProperty(
					wxT("Colour"), wxPG_LABEL,
					wxColour(aqua::Uint8(colour[0] * 256.0),
							 aqua::Uint8(colour[1] * 256.0),
							 aqua::Uint8(colour[2] * 256.0))));
			sceneProperties_->DisableProperty(prop);
			FbxDouble intensity = pLight->Intensity;
			prop = sceneProperties_->Append(new wxFloatProperty(wxT("Intensity "), wxPG_LABEL, intensity));
			prop->SetClientData(static_cast<void*>(pNode));
			sceneProperties_->DisableProperty(prop);
		}
			break;
		case FbxNodeAttribute::eBoundary:
			break;
		case FbxNodeAttribute::eOpticalMarker:
			break;
		case FbxNodeAttribute::eOpticalReference:
			break;
		case FbxNodeAttribute::eCameraSwitcher:
			break;
		case FbxNodeAttribute::eNull:
			// spline?
			break;
		case FbxNodeAttribute::ePatch:
			break;
		case FbxNodeAttribute::eNurbs:
			break;
		case FbxNodeAttribute::eNurbsSurface:
			break;
		case FbxNodeAttribute::eNurbsCurve:
			break;
		case FbxNodeAttribute::eTrimNurbsSurface:
			break;
		case FbxNodeAttribute::eShape:
			break;
		case FbxNodeAttribute::eLODGroup:
			break;
		case FbxNodeAttribute::eSubDiv:
			break;
		case FbxNodeAttribute::eUnknown:
			break;
	}
	sceneProperties_->Thaw();
	return;
}

//!
//! Process the user double clicking in the property grid
//! @param evt event details
//!
void FbxPropertyPanel::PropertyDoubleClick(wxPropertyGridEvent& evt)
{
	wxPGProperty* property = evt.GetProperty();
	if (property) {
		wxLogVerbose(wxT("Property Label %s"), property->GetLabel());
		wxLogVerbose(wxT("Property Name %s"), property->GetName());
		void *clientPtr = property->GetClientData();
		if (clientPtr) {
			FbxNode *node = static_cast<FbxNode*>(clientPtr);
			wxString nodeName(node->GetName(), wxConvUTF8);
			wxLogVerbose(wxT("Property Node %s"), nodeName);
			wxCommandEvent sceneEvent(FbxPropertyDoubleClickEventType, GetId());
			// TO DO : Sort this
			//SceneClientData* nodeObject(new SceneClientData(node));
			//sceneEvent.SetClientObject(nodeObject);
			//ProcessEvent(sceneEvent);
		}
	} else {
		wxLogVerbose(wxT("No Property!"));
	}
	return;
}

//!
//! Function called when property changes on property grid
//! @param evt Event that happens on property grid
//!
void FbxPropertyPanel::PropertyChanged(wxPropertyGridEvent& evt)
{
	wxPGProperty* property = evt.GetProperty();
	if (property) {
		const wxString objectName(property->GetParent()->GetName());
		wxLogVerbose(wxT("CHANGED %s: Property Label %s"), objectName, property->GetLabel());
		wxLogVerbose(wxT("CHANGED %s: Property Name %s"), objectName, property->GetName());
		void *clientPtr = property->GetClientData();
//		FbxNode *node = NULL;
		if (clientPtr) {
			FbxNode *node = static_cast<FbxNode*>(clientPtr);
			wxString nodeName(node->GetName(), wxConvUTF8);
			wxLogVerbose(wxT("Property Node %s"), nodeName);
		}
		// now send a scene update message
// TO DO: Sort this out
//		wxCommandEvent sceneEvent(FbxNodeChangedEventType, GetId());
//		SceneClientData* nodeObject(new SceneClientData(node));
//		sceneEvent.SetClientObject(nodeObject);
//		ProcessEvent(sceneEvent);
	} else {
		wxLogVerbose(wxT("No Property!"));
	}
	return;
}
