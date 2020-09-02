#ifndef ELEMENTS_H
#define ELEMENTS_H

template <typename LayerType, typename LayerElementType>
class GeometryElementLayer
{
private:
	LayerType* m_GeometryElement;
	int		   m_ElementCount;
	FbxNode*   m_Node;
	FbxMesh*   m_Mesh;
public:
	GeometryElementAccessor(FbxMesh   *pMesh,
							LayerType *elements,
							int elementCount) 
		: m_GeometryElement(elements), m_ElementCount(elementCount), 
		  m_Node(pMesh->GetNode), m_Mesh(pMesh)
	{
		
	}

	bool isValid() const
	{
		return (m_GeometryElement != NULL);
	}

	bool indexed() const 
	{
		return (m_GeometryElement->GetReferenceMode() != FbxLayerElement::eDirect);
	}

};
#endif
