#include <StdAfx.h>
#include "ModelViewProjectionParameterUpdater.h"

#include <PbSystemTypes.h>
#include <aqua/Material.h>
#include <GraphicsSystem/GraphicsSystem.h>

using namespace aqua;

void ModelViewProjectionParameterUpdater::update(const shared_ptr<PbMatrix4> /*transformMatrix*/, const shared_ptr<const Material> /*material*/)
{
#if defined(PB_USE_GLSL)
	const Locator::Service<GraphicsSystem>& graphicsSystem(Locator::checkOut<GraphicsSystem>());
	const shared_ptr<RenderTargetOpenGlsl> currentRenderTarget(boost::static_pointer_cast<RenderTargetOpenGlsl, RenderTarget>(graphicsSystem->getCurrentRenderTarget()));
	const PbMatrix4& modelViewProjectionMatrix(currentRenderTarget->getModelViewProjectionMatrix());

	Locator::checkIn(graphicsSystem);
	setValue(modelViewProjectionMatrix);
#else
	setValue(PbMatrix4::IDENTITY);
#endif
}
