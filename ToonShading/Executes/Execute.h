#pragma once

#include "../Viewer/Camera.h"
#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"

struct ExecuteValues
{
	class ViewProjectionBuffer* ViewProjection;

	class Perspective* Perspective;
	class Viewport* Viewport;
	class Camera* MainCamera;
	class LightBuffer* GlobalLight;

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

	virtual void PreRender() = 0;
	virtual void PreRender2() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;
	virtual void ImGuiRender() = 0;

	virtual void ResizeScreen() = 0;

protected:
	ExecuteValues* values;
};