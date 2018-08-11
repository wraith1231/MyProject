#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>	//map보다 이게 더 빠름
//이건 hash 코드 이용하는거라 데이터가 정렬 안된 상태로 다룰떄 빠름
//근데 iter 만들고 하면 더 복잡하다고 뺴라십니다!
#include <functional>
using namespace std;


//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
//#include <d3dxGlobal.h>
//#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//Fbx SDK
#define FBXSDK_SHARED
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

//Json Cpp
#include <json/json.h>
#pragma comment(lib, "jsoncpp.lib")

//Directx Tex
#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

//이거 회사에선 절대로 죽어도 쓰면 안됨
//#pragma warning(disable : 4996)
//#pragma warning(disable : 4099)
//과제할땐 어지간하면 warning은 최대한 없애는 방향으로 프로그래밍 해줘야 한다

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

//이거 추가하면 Path에 GetFileLocalPath에도 추가해주는걸 추천
const wstring Assets = L"../_Assets/";
const wstring Contents = L"../_Contents/";
const wstring Textures = L"../_Contents/Textures/";
const wstring Models = L"../_Contents/Models/";
const wstring FbxModels = L"../_Contents/FbxModels/";
const wstring Shaders = L"../_Shaders/";
const wstring Landscapes = L"../_Contents/Landscape/";
const wstring Jsons = L"../_Json/";

#include "./Systems/D3D.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"
#include "./Renders/Texture.h"
#include "./Renders/Material.h"
#include "./Renders/RenderTarget.h"

#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"

#include "./Executes/GuiSettings.h"
#include "./Executes/Execute.h"

#include "./Utilities/Json.h"
#include "./Utilities/Math.h" 
#include "./Utilities/String.h"
#include "./Utilities/Path.h"

#include "./Bounding/Frustum.h"

typedef VertexTextureNormalBlend ModelVertexType;