#if !defined(SHADERCLIENTDATA_H_INCLUDED)
#define SHADERCLIENTDATA_H_INCLUDED

extern const wxEventType ShaderUpdateEventType;


//!< Data for client of Shader Event
class ShaderUpdateClientData : public wxClientData
{
public:
	ShaderUpdateClientData(aqua::shared_ptr<wxArrayString> shaders) : shaders_(shaders)
	{

	}

	aqua::shared_ptr<wxArrayString> getShaderList() {
		return shaders_;
	}
private:
	aqua::shared_ptr<wxArrayString> shaders_;
};



#endif

