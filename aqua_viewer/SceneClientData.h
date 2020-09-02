#ifndef SCENECLIENTDATA_H_INCLUDED
#define SCENECLIENTDATA_H_INCLUDED

extern const wxEventType SceneEventType;
extern const wxEventType SceneClearEventType;
extern const wxEventType SceneLoadEventType;
extern const wxEventType SceneUnLoadEventType;


class FbxParse;

class SceneClientData : public wxClientData {
public:
	SceneClientData(FbxParse* parser, FbxObject* node) : parser_(parser), object_(node) {
	}

	KFbxNode* getNode() {
		if(object_->Is(FbxNode::ClassId))
			return FbxCast<FbxNode>(object_);
		else
			return NULL;
	}

	FbxDisplayLayer* getLayer() {
		if(object_->Is(FbxDisplayLayer::ClassId))
			return FbxCast<FbxDisplayLayer>(object_);
		else
			return NULL;
	}

	bool isNode() const {
		return ((object_!=NULL) && (object_->Is(FbxNode::ClassId)));
	}

	bool isLayer() const {
		return ((object_ != NULL) && (object_->Is(FbxDisplayLayer::ClassId)));
	}

	FbxObject* getObject()	{
		return object_;
	}

	FbxParse* getParser() {
		return parser_;
	}

private:
	FbxObject* object_;
	FbxParse*   parser_;
};




#endif

