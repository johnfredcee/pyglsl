#ifndef MODEL_VIEW_PROJECTION_PARAMETER_UPDATE_H_
#define MODEL_VIEW_PROJECTION_PARAMETER_UPDATE_H_

#include <PbSystemTypes.h>
#include <Shader/ShaderParameterUpdater.h>
#include <Utils/HashString.h>

namespace aqua {
	class Material;
}

class ModelViewProjectionParameterUpdater : public aqua::ShaderParameterUpdater<aqua::PbMatrix4>
{
	public:
		ModelViewProjectionParameterUpdater()
		: aqua::ShaderParameterUpdater<aqua::PbMatrix4>(aqua::HashString("u_modelViewProj"), aqua::ShaderParameter::FLOAT_MATRIX4_PARAMETER, aqua::ShaderParameterUpdater<aqua::PbMatrix4>::EVERY_CALL)
		{
		}

		//! Update function
		void update(const aqua::shared_ptr<aqua::PbMatrix4> transformMatrix, const aqua::shared_ptr<const aqua::Material> material);
};

#endif
