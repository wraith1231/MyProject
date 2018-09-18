#pragma once

#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"
#include "../Bounding/Frustum.h"

struct ExecuteValues
{
	class ViewProjectionBuffer* ViewProjection;

	class Perspective* Perspective;
	class Viewport* Viewport;
	class Camera* MainCamera;
	class LightBuffer* GlobalLight;
	class Objects::Frustum* ViewFrustum;

	struct GuiSettings* GuiSettings;

	Json::Value* jsonRoot;
};

class Execute
{
public:
	Execute(ExecuteValues* settings)
	{
		this->values = settings;
	}

	virtual ~Execute() {}

	virtual void Update() = 0;

	virtual void ShadowRender() = 0;
	virtual void PreRender() = 0;
	virtual void LightRender() = 0;
	virtual void SSAORender() = 0;
	virtual void EdgeRender() = 0;
	virtual void AARender() = 0;

	virtual void ImGuiRender() = 0;

	virtual void ResizeScreen() = 0;

protected:
	ExecuteValues* values;
};