#include "StdAfx.h"

#include "Layers.h"

using namespace aqua;


void PrintLayerModes(FbxLayerElement* element)
{
	FbxLayerElement::EMappingMode mapMode = element->GetMappingMode();
	switch (mapMode) {
		case FbxLayerElement::eNone:
			wxLogVerbose("The mapping is undetermined.");
			break;
		case FbxLayerElement::eByControlPoint:
			wxLogVerbose("There will be one mapping coordinate for each surface control point/vertex.");
			break;
		case FbxLayerElement::eByPolygonVertex:
			wxLogVerbose("There will be one mapping coordinate for each vertex, for every polygon of which it is a part. This means that a vertex will have as many mapping coordinates as polygons of which it is a part.");
			break;
		case FbxLayerElement::eByPolygon:
			wxLogVerbose("There can be only one mapping coordinate for the whole polygon.");
			break;
		case FbxLayerElement::eByEdge:
			wxLogVerbose("There will be one mapping coordinate for each unique edge in the mesh. This is meant to be used with smoothing layer elements.");
			break;
		case FbxLayerElement::eAllSame:
			wxLogVerbose("There can be only one mapping coordinate for the whole surface.");
			break;
		default:
			break;
	}

	FbxLayerElement::EReferenceMode refMode = element->GetReferenceMode();
	switch (refMode)
	{
		case FbxLayerElement::eDirect:
			wxLogVerbose("Direct");
			break;
		case FbxLayerElement::eIndex:
			wxLogVerbose("Index");
			break;
		case FbxLayerElement::eIndexToDirect:
			wxLogVerbose("Index TO Direct");
			break;
		default:
			break;
	}
	return;
}


