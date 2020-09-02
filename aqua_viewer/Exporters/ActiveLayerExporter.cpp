#include <StdAfx.h>

#include <PbSystemTypes.h>
#include <Locator.h>
#include <Utils/HashString.h>
#include <Utils/Logger.h>
#include <MetaData/Dynamic/DynamicMetaData.h>
#include <aqua/AnimationData.h>

#include <wx/glcanvas.h>

#include "../FbxParse.h"
#include "../SceneManager.h"
#include "../LayerManager.h"
#include "../Rtti.h"
#include "../Config.h"
#include "../IntermediateMesh.h"
#include "../IMesh.h"
#include "../IGraphicObject.h"
#include "../GraphicObject.h"
#include "../MeshObject.h"
#include "../AnimationTimeStamp.h"
#include "../AnimationTranslationKey.h"
#include "../AnimationRotationKey.h"
#include "../AnimationKey.h"
#include "../AnimationTrack.h"
#include "../AnimationContainer.h"
#include "../MetaSerializer.h"
#include "../Animation.h"

#include "ILayerExporter.h"
#include "SimpleLayerExporter.h"
#include "ActiveLayerExporter.h"


using namespace aqua;

// serialise a single animation track container to metadata (parser is an animation joint that contains tracks)
template<>	void MetaSerialiser<AnimationTrackContainer>(const AnimationTrackContainer& tracks, DynamicMetaParser& parser)
{
	std::string jointName(parser.GetName());
	DynamicMetaData* metaData = parser.GetDynamicMetaData();	
	parser.SetArrayCount(tracks.trackCount());
	DynamicMetaParser trackArray = parser.Find("tracks_");
	PBASSERT(trackArray.IsValid(), "Invalid metadata");
	trackArray.SetArrayCount(tracks.trackCount());
	for(Uint32 trackIndex = 0; trackIndex < tracks.trackCount(); ++trackIndex) {
		trackArray.Find(trackIndex, MetaParser::FIND_BY_INDEX).SetValue((jointName + "_" + tracks.trackName(trackIndex)).c_str());		
	}
	Uint32 trackTypeCRC = CalcCRC("AnimationTrack"); 	
	for(Uint32 trackIndex = 0; trackIndex < tracks.trackCount(); ++trackIndex) {
		DynamicMetaParser trackParser = metaData->AllocateData((jointName + "_" + tracks.trackName(trackIndex)).c_str(), trackTypeCRC);
		AnimationTrackPointer track = tracks.track(trackIndex);
		trackParser.Find("trackType_").SetValue((Uint32) track.kind());	
		DynamicMetaParser keyParser = trackParser.Find("keys_");		
		PBASSERT(keyParser.IsValid(), "Invalid metadata");
		keyParser.SetArrayCount(track.size());
		for(Uint32 keyIndex = 0; keyIndex < track.size(); ++keyIndex) {
			aqua::AnimationKey thisKey;
			track.key(keyIndex, thisKey);
			DynamicMetaParser iKeyParser = keyParser.Find(keyIndex, MetaParser::FIND_BY_INDEX);
			PBASSERT(iKeyParser.IsValid(), "Invalid metadata");
			iKeyParser.Find("t").SetValue( thisKey.t );
			iKeyParser.Find("x").SetValue( thisKey.x );
			iKeyParser.Find("y").SetValue( thisKey.y );
			iKeyParser.Find("z").SetValue( thisKey.z );
			iKeyParser.Find("w").SetValue( thisKey.w );	
		} // for each key
	}	
}


bool ActiveLayerExporter::ExportLayer(aqua::Uint32 entityTypeId, DynamicMetaData& metaData, aqua::Array<SceneManager::Handle>& handles)  {
	bool result = SimpleLayerExporter::ExportLayer(entityTypeId, metaData, handles);
	const aqua::Locator::Service<LayerManager>& layerManager(aqua::Locator::checkOut<LayerManager>());
	const std::string layerName(layerManager->getCurrentLayerName());
	aqua::Locator::checkIn(layerManager);
	// for each object in the layer
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		// neeed these to access stuff
		const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());
		// fish out the name of the object for the entity
		aqua::shared_ptr<const aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::Locator::checkIn(sceneManager);
		aqua::HashString childId(object->getName());
		aqua::HashString parentId(object->getParent());
		DynamicMetaParser entityMetaData(metaData, childId.getHashValue());		
		DynamicMetaParser componentMetaData(entityMetaData.Find(CalcCRC("GraphicModelComponentData")));
		PBASSERT(componentMetaData.IsValid(), "Invalid componentMetaData");
		aqua::HashString componentId = componentMetaData.Find("id").GetValue<aqua::HashString>();
		PBASSERT(componentMetaData.IsValid(), "Invalid componentMetaData");
		componentMetaData = DynamicMetaParser(metaData, componentId.getHashValue());
		PBASSERT(componentMetaData.IsValid(), "Invalid componentMetaData");
		DynamicMetaParser matMetaData(componentMetaData.Find(CalcCRC("materialId")));
		PBASSERT(matMetaData.IsValid(), "Invalid matMetaData");
		DynamicMetaParser vBufferMetaData(componentMetaData.Find(CalcCRC("vBuffer")));
		PBASSERT(vBufferMetaData.IsValid(), "Invalid vBufferMetaData");
		DynamicMetaParser iBufferMetaData(componentMetaData.Find(CalcCRC("iBuffer")));
		PBASSERT(iBufferMetaData.IsValid(), "IsValid iBufferMetaData");
		aqua::Array<std::string> materials;
		aqua::Array<std::string> vBuffers;
		aqua::Array<std::string> iBuffers;		
		for(aqua::Uint32 i = 0; i < vBufferMetaData.GetArrayCount(); ++i) {
			materials.pushBack(std::string(matMetaData.Find(i, MetaParser::FIND_BY_INDEX).GetValue<const char*>()));
			vBuffers.pushBack(std::string(vBufferMetaData.Find(i, MetaParser::FIND_BY_INDEX).GetValue<const char*>()));	
			iBuffers.pushBack(std::string(iBufferMetaData.Find(i, MetaParser::FIND_BY_INDEX).GetValue<const char*>()));		
		}
		// do the "link"
		std::string linkName(object->getObjectName() + std::string("_link"));
		DynamicMetaParser linkMetaData(metaData.AllocateData(linkName.c_str(), CalcCRC("LinkData")));
		linkMetaData.Find(CalcCRC("linkedObject_")).SetValue(childId);
		DynamicMetaParser matrixParser(linkMetaData.Find("objectTransform_.mat"));
		PBASSERT(matrixParser.IsValid(), "Invalid matrix parser");
		// object transform
		// to do: SetValue really needs a PbMatrix specialisation..unrolling this is ludy
		const aqua::Float32  *transform(object->getTransformMatrixPtr()->data());
		for(Uint32 i = 0; i < 16; i++) {
			matrixParser.Find(i, MetaParser::FIND_BY_INDEX).SetValue(transform[i]);			
		}
		transform = object->getParentTransformMatrixPtr()->data();
		// parent transform
		DynamicMetaParser parentMatrixParser(linkMetaData.Find("parentTransform_.mat"));
		PBASSERT(parentMatrixParser.IsValid(), "Invalid matrix parser");
		for(Uint32 i = 0; i < 16; i++) {
			parentMatrixParser.Find(i, MetaParser::FIND_BY_INDEX).SetValue(transform[i]);
		}
		// materials
		matMetaData     = linkMetaData.Find(CalcCRC("materials_"));
		PBASSERT(matMetaData.IsValid(), "Invalid matMetaData");
		matMetaData.SetArrayCount(materials.size());
		for(aqua::Uint32 i = 0; i < vBuffers.size(); ++i) {
			matMetaData.Find(i, MetaParser::FIND_BY_INDEX).SetValue(materials[i].c_str());	
		}
		linkMetaData.SetArrayCount(materials.size());
		// vertex buffers
		vBufferMetaData = linkMetaData.Find(CalcCRC("vBuffers_"));
		PBASSERT(vBufferMetaData.IsValid(), "Invalid vBufferMetaData");
		vBufferMetaData.SetArrayCount(vBuffers.size());
		for(aqua::Uint32 i = 0; i < vBuffers.size(); ++i) {
			vBufferMetaData.Find(i, MetaParser::FIND_BY_INDEX).SetValue(vBuffers[i].c_str());	
		}
		// index buffers
		iBufferMetaData = linkMetaData.Find(CalcCRC("iBuffers_"));
		PBASSERT(iBufferMetaData.IsValid(), "Invalid iBufferMetaData");
		iBufferMetaData.SetArrayCount(iBuffers.size());
		for(aqua::Uint32 i = 0; i < vBuffers.size(); ++i) {
			iBufferMetaData.Find(i, MetaParser::FIND_BY_INDEX).SetValue(iBuffers[i].c_str());
		}		
	} // fore eash object in the layer
	// animations
	const aqua::Locator::Service<SceneManager>& sceneManager(aqua::Locator::checkOut<SceneManager>());								
	// for each object in the layer
	std::string jointTypeName("AnimationJoint");
	for(aqua::Array<SceneManager::Handle>::iterator h_it = handles.begin(); h_it != handles.end() ; ++h_it) {
		aqua::shared_ptr<aqua::IGraphicObject> object = sceneManager->getObject(*h_it);
		aqua::shared_ptr<aqua::MeshObject>     mesh(boost::dynamic_pointer_cast<aqua::MeshObject, aqua::IGraphicObject>(object));
		if (mesh) {
			AnimationTrackContainer tracks;
			extractAnimationTracks(tracks, sceneManager->sceneParser(), mesh->getFbxNode());
			std::string jointName(mesh->getObjectName() + "_anim");
			DynamicMetaParser jointParser = sceneManager->allocateData(jointName, jointTypeName);
			MetaSerialiser<AnimationTrackContainer>(tracks, jointParser);	
		}
	}
	aqua::Locator::checkIn(sceneManager);
	
	return result;
}
 
int ActiveLayerExporter::LayerEntityCount(aqua::Array<SceneManager::Handle>& handles) {
	return SimpleLayerExporter::LayerEntityCount(handles);
}
