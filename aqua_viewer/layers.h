#if !defined(LAYERS_H_INCLUDED)
#define LAYERS_H_INCLUDED


void PrintLayerModes(FbxLayerElement* element);


//#define USE_FBX_REMAP 1

template <typename LayerType, typename LayerElementType>
class LayerElementAccessor
{
private:
	LayerType* elements_;
	FbxLayer* layer_;
	FbxNode*  pNode_;
	FbxMesh*  pMesh_;
public:

	//!
	//! Construct a functor that "flattens out" access to a layer, regardless of mapping modes
	//! @param pMesh <doc>
	//! @param layerIndex <doc>
	//! @param  <doc>
	//! @param layerKind <doc>
	//! @param isUV <doc>
	//! @return <doc>
	//!
	LayerElementAccessor(FbxMesh *pMesh, int layerIndex,
						 enum FbxLayerElement::EType layerKind,
						 bool isUV) :  pNode_(pMesh->GetNode()), pMesh_(pMesh)
	{
		layer_ =    NULL;
		elements_ = NULL;
		int layerCount = pMesh_->GetLayerCount();
		if (layerIndex < layerCount) {
			layer_ = pMesh_->GetLayer(layerIndex);
			elements_ = static_cast<LayerType*>( layer_->GetLayerElementOfType(layerKind, isUV ) );
			if ((elements_) && (isUV))
				wxLogVerbose("Layer Element name %s ", wxString(elements_->GetName(), wxConvUTF8));
		}
#ifdef USE_FBX_REMAP
		if ((elements_) && ((elements_->GetReferenceMode() == FbxLayerElement::eINDEX_TO_DIRECT) || (elements_->GetReferenceMode() == FbxLayerElement::eINDEX))) {
			if (elements_->RemapIndexTo(FbxLayerElement::eBY_CONTROL_POINT)) {
				wxLogError("Remapping failed on %s ", wxString(elements_->GetName(), wxConvUTF8));
			}
		}
#endif
	}
 
	//!
	//! Construct a functor that "flattens out" access to a layer, regardless of mapping modes.
	//! Unlike the previous constructor, this one searches for a uvchannel by name
	//! @param pMesh mesh that uses channel
	//! @param layerIndex layer uv channel was found on
	//! @param channelName name of uv channel to search for
	//!
	LayerElementAccessor(FbxMesh *pMesh, int& layerIndex, const KString& channelName) : pMesh_(pMesh) {
		int layerKind;
		bool foundLayer = false;

		layer_ = NULL;
		elements_ = NULL;
		int layerCount = pMesh_->GetLayerCount();

		
		for (layerIndex = 0; layerIndex < layerCount; ++layerIndex) {
			layer_ = pMesh_->GetLayer(layerIndex);
			for(layerKind = FbxLayerElement::sTypeTextureStartIndex;
				layerKind < FbxLayerElement::sTypeTextureEndIndex;
				++layerKind) {
				elements_ = static_cast<LayerType*>( layer_->GetLayerElementOfType(static_cast<FbxLayerElement::ELayerElementType>(layerKind), true) );
				if (elements_) {
					if ((KString(elements_->GetName()).Compare(channelName)) == 0) {
						//wxLogDebug("Layer Element name %s ", wxString(elements_->GetName(), wxConvUTF8));
#ifdef USE_FBX_REMAP
						// have a stab at remapping it
						if ((elements_->GetReferenceMode() == FbxLayerElement::eINDEX_TO_DIRECT) || (elements_->GetReferenceMode() == FbxLayerElement::eINDEX)) {
							if (elements_->RemapIndexTo(FbxLayerElement::eBY_CONTROL_POINT)) {
								wxLogError("Remapping failed on %s ", wxString(elements_->GetName(), wxConvUTF8));
							}
						}
#endif
						foundLayer = true;
						break;
					}
				}
			}
			if (foundLayer)
				break;
		}
		if (!foundLayer) {
			elements_ = 0;
			//wxLogDebug("Failed to find %s ", wxString(channelName.Buffer(), wxConvUTF8));
		}
		return;
	}


	bool isValid() const {
		return (layer_ != 0) && (elements_ != 0);
	}

	bool indexed() const {
		return (elements_->GetReferenceMode() != FbxLayerElement::eDirect);
	}

	const FbxLayerElement::EMappingMode mapping() const {
		PBASSERT(elements_ != NULL, "Invalid layer");
		return elements_->GetMappingMode();
	}

	const FbxLayerElement::EReferenceMode reference() const {
		PBASSERT(elements_ != NULL, "Invalid layer");
		return elements_->GetReferenceMode();
	}

	LayerElementType operator()(int polygonIndex, int vertexIndex) {

		PBASSERT(((polygonIndex >= 0) && (polygonIndex < pMesh_->GetPolygonCount())), "Polygon index out of range");
		PBASSERT(((vertexIndex >= 0) && (vertexIndex < pMesh_->GetPolygonSize(polygonIndex))), "Polygon index out of range");

		int lControlPointIndex = pMesh_->GetPolygonVertex(polygonIndex, vertexIndex);
		PBASSERT(((lControlPointIndex >= 0) && (lControlPointIndex < pMesh_->GetControlPointsCount())), "Polygon Vertex Index out of range");

		switch  (elements_->GetMappingMode())
		{

			case FbxLayerElement::eByControlPoint: {
				switch (elements_->GetReferenceMode()) {
					case FbxLayerElement::eDirect:
						return elements_->GetDirectArray().GetAt(lControlPointIndex);
						break;
					case FbxLayerElement::eIndexToDirect: {
						PBASSERT((elements_->GetIndexArray().GetCount() == pMesh_->GetControlPointsCount()), "Mesh and index array do not match up");
						PBASSERT(lControlPointIndex < elements_->GetIndexArray().GetCount(), "Control Point index out of range");
						int id = elements_->GetIndexArray().GetAt(lControlPointIndex);
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);
					}
						break;
					default:
						break;
				}
			}
				break;

			case FbxLayerElement::eByPolygonVertex:	{
				int vertexId = polygonIndex * 3 + vertexIndex;
				switch (elements_->GetReferenceMode()) {
					case FbxLayerElement::eDirect:
						PBASSERT(vertexIndex < elements_->GetDirectArray().GetCount(), "Vertex index out of range");
						return elements_->GetDirectArray().GetAt(vertexId);
						break;
					case FbxLayerElement::eIndexToDirect:
					{
						PBASSERT(vertexIndex < elements_->GetIndexArray().GetCount(), "Vertex index out of range");
						int id = elements_->GetIndexArray().GetAt(vertexId);
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);
					}
					break;
					default:
						break;
				}
			}
				break;

			case FbxLayerElement::eByPolygon:	{
				switch (elements_->GetReferenceMode())
				{
					case FbxLayerElement::eDirect:
						PBASSERT(polygonIndex < elements_->GetDirectArray().GetCount(), "Polygon index out of range");
						return elements_->GetDirectArray().GetAt(polygonIndex);
						break;
					case FbxLayerElement::eIndexToDirect:
					{
						PBASSERT(polygonIndex < elements_->GetIndexArray().GetCount(), "Polygon index out of range");
						int id = elements_->GetIndexArray().GetAt(polygonIndex);
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);

					}
					break;
				}
			}
				break;

			case FbxLayerElement::eAllSame: {
				switch (elements_->GetReferenceMode())
				{

					case FbxLayerElement::eDirect:
						return elements_->GetDirectArray().GetAt(0);
						break;
					case FbxLayerElement::eIndexToDirect:
					{
						int id = elements_->GetIndexArray().GetAt(0);
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);

					}
					break;
				}
			}
				break;

			default:
				wxLogError("LayerElementAccessor() - Unhandled mapping mode!");
		}
		wxLogError("Invalid Element Access!");
		PBINSTANTBREAK();
		return *((LayerElementType*)(0));
	}


	/**
	 * Treat the layer as an array and return the it-h entry allowing for indices
	 * @param vertexIndex Index of element to fetch
	 * @return value of the element
	 */
	LayerElementType at(int vertexIndex) {

		static LayerElementType dummy;

		PBASSERT(((vertexIndex >= 0) && (vertexIndex < pMesh_->GetControlPointsCount())), "Vertex index out of range");

		FbxLayerElement::EMappingMode mode = elements_->GetMappingMode();
		FbxLayerElement::EReferenceMode refmode = elements_->GetReferenceMode();
		PBASSERT(((mapping() == FbxLayerElement::eByControlPoint) || (mapping() == FbxLayerElement::eAllSame)), "Attribute mapped by polygon, not vertex");

		switch  (mode)
		{
			case FbxLayerElement::eByControlPoint: {
				switch (refmode) {
					case FbxLayerElement::eDirect:
						PBASSERT(vertexIndex < elements_->GetDirectArray().GetCount(), "index out of range");
						return elements_->GetDirectArray().GetAt(vertexIndex);
						break;
					case FbxLayerElement::eIndexToDirect: {
						PBASSERT(vertexIndex < elements_->GetIndexArray().GetCount(), "index out of range");
						int id = elements_->GetIndexArray().GetAt(vertexIndex);
						PBASSERT(id < elements_->GetDirectArray().GetCount(), "index out of range")
							if (id < elements_->GetDirectArray().GetCount())
								return elements_->GetDirectArray().GetAt(id);
					}
						break;
					default:
						break;
				}
			}

			case FbxLayerElement::eByPolygonVertex: {
				wxLogDebug("Polyvertex");
				break;
		    };

			case FbxLayerElement::eByPolygon: {
				wxLogDebug("Poly");
				break;
		    };

			case FbxLayerElement::eByEdge: {
				wxLogDebug("Edge");
				break;
 		    }		

			case FbxLayerElement::eAllSame: {
				return elements_->GetDirectArray().GetAt(vertexIndex);
				break;
			} 
			default:
				wxLogError("LayerElementAccessor at() - Unhandled mapping mode!");
		}
		wxLogError("No element available!");
		PBINSTANTBREAK();
		return *((LayerElementType*)(&dummy));
	}

	LayerElementType& operator[](int polygonIndex) {
		PBASSERT(((mapping() == FbxLayerElement::eByControlPoint) || (mapping() == FbxLayerElement::eByPolygon) || (mapping() == FbxLayerElement::eAllSame)), "Attribute mapped by vertex, not polygon.");
		PBASSERT(((polygonIndex >= 0) && (polygonIndex < pMesh_->GetPolygonCount())), "Polygon index out of range");

		switch  (elements_->GetMappingMode())
		{
			case FbxLayerElement::eByControlPoint: {
				switch (elements_->GetReferenceMode()) {
					case FbxLayerElement::eDirect:
						PBASSERT(polygonIndex < elements_->GetDirectArray().GetCount(), "index out of range");
						return elements_->GetDirectArray().GetAt(polygonIndex);
						break;
					case FbxLayerElement::eIndexToDirect: {
						int id = elements_->GetIndexArray().GetAt(polygonIndex);
						PBASSERT(id < elements_->GetDirectArray().GetCount(), "index out of range")
							if (id < elements_->GetDirectArray().GetCount())
								return elements_->GetDirectArray().GetAt(id);
					}
						break;
					default:
						break;
				}
			}
			case FbxLayerElement::eByPolygon:	{
				switch (elements_->GetReferenceMode())
				{
					case FbxLayerElement::eDirect:
						PBASSERT(polygonIndex < elements_->GetDirectArray().GetCount(), "Polygon index out of range");
						return elements_->GetDirectArray().GetAt(polygonIndex);
						break;
					case FbxLayerElement::eIndexToDirect:
					{
						PBASSERT(polygonIndex < elements_->GetIndexArray().GetCount(), "Polygon index out of range");
						int id = elements_->GetIndexArray().GetAt(polygonIndex);
						PBASSERT(id < elements_->GetDirectArray().GetCount(), "Index Out of range");
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);

					}
					break;
				}
			}
				break;

			case FbxLayerElement::eAllSame: {
				switch (elements_->GetReferenceMode())
				{

					case FbxLayerElement::eDirect:
						return elements_->GetDirectArray().GetAt(0);
						break;
					case FbxLayerElement::eIndexToDirect:
					{
						int id = elements_->GetIndexArray().GetAt(0);
						PBASSERT(id < elements_->GetDirectArray().GetCount(), "Index Out of range");
						if (id < elements_->GetDirectArray().GetCount())
							return elements_->GetDirectArray().GetAt(id);

					}
					break;
				}
			}
				break;

			default:
				wxLogError("LayerElementAccessor() - Unhandled mapping mode!");
				PBBREAK("LayerElementAccessor() - Unhandled mapping mode!");
				break;
		}
		PBASSERT(2+2==5, "Invalid element access");
		wxLogError("No element available!");
		PBINSTANTBREAK();
		return *((LayerElementType*)(0));
	}

	/// return the index of this element rather than the element itself
	int index(int polygonIndex, int vertexIndex) {
		PBASSERT(((polygonIndex >= 0) && (polygonIndex < pMesh_->GetPolygonCount())), "Polygon index out of range");
		PBASSERT(((vertexIndex >= 0) && (vertexIndex < pMesh_->GetPolygonSize(polygonIndex))), "Polygon index out of range");

		int lControlPointIndex = pMesh_->GetPolygonVertex(polygonIndex, vertexIndex);
		PBASSERT(((lControlPointIndex >= 0) && (lControlPointIndex < pMesh_->GetControlPointsCount())), "Polygon Vertex Index out of range");
		int result = -1;

		if ((elements_->GetReferenceMode() == FbxLayerElement::eIndexToDirect) || (elements_->GetReferenceMode() == FbxLayerElement::eIndex)) {
			switch  (elements_->GetMappingMode())
			{
				case FbxLayerElement::eByControlPoint:
					PBASSERT((elements_->GetIndexArray().GetCount() == pMesh_->GetControlPointsCount()), "Mesh and index array do not match up");
					PBASSERT(lControlPointIndex < elements_->GetIndexArray().GetCount(), "Control Point index out of range");
					result = elements_->GetIndexArray().GetAt(lControlPointIndex);
					break;

				case FbxLayerElement::eByPolygonVertex: {
					int vertexId = polygonIndex * 3 + vertexIndex;
					PBASSERT(vertexIndex < elements_->GetIndexArray().GetCount(), "Vertex index out of range");
					result = elements_->GetIndexArray().GetAt(vertexId);
					break;
				}

				case FbxLayerElement::eByPolygon:
					PBASSERT(polygonIndex < elements_->GetIndexArray().GetCount(), "Polygon index out of range");
					result = elements_->GetIndexArray().GetAt(polygonIndex);
					break;

				case FbxLayerElement::eAllSame:
					result = elements_->GetIndexArray().GetAt(0);
					break;

				default:
					wxLogError("LayerElementAccessor() - Unhandled mapping mode!");
					break;
			}
		} else {

			switch  (elements_->GetMappingMode())
			{
				case FbxLayerElement::eByControlPoint:
					result = lControlPointIndex;
					break;
				case FbxLayerElement::eByPolygonVertex:
					result = polygonIndex * 3 + vertexIndex;
					break;
				case FbxLayerElement::eByPolygon:
					result = polygonIndex;
					break;
				case FbxLayerElement::eAllSame:
					result = 0;
					break;
				default:
					wxLogError("LayerElementAccessor() - Unhandled mapping mode!");
					PBBREAK("LayerElementAccessor() - Unhandled mapping mode!");
					break;
			}
		}
		return result;
	}

	int index(int polygonIndex) {
		PBASSERT(((mapping() == FbxLayerElement::eByPolygon) || (mapping() == FbxLayerElement::eAllSame)), "Attribute mapped by vertex, not polygon.");
		PBASSERT(polygonIndex >= 0, "Polygon index must be +ve");
		PBASSERT(((mapping() == FbxLayerElement::eAllSame) || (reference() != FbxLayerElement::eDirect)), "No index array available");
		int result = -1;
		switch  (elements_->GetMappingMode())
		{

			case FbxLayerElement::eByPolygon:
				PBASSERT(polygonIndex < elements_->GetIndexArray().GetCount(), "Polygon index out of range");
				result = elements_->GetIndexArray().GetAt(polygonIndex);
				break;

			case FbxLayerElement::eAllSame:
				result = elements_->GetIndexArray().GetAt(0);
				break;

			default:
				wxLogError("LayerElementAccessor() - Unhandled mapping mode!");

		}
		return result;
	}

};
#endif
