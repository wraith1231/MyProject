#pragma once

class Shader
{
public:
	Shader(wstring shaderFile, string vsName = "VS", string psName = "PS");
	~Shader();

	wstring GetFile() { return shaderFile; }

	void Render();

	void CreateHullShader(wstring shaderFile, string hsName = "HS");
	void CreateDomainShader(wstring shaderFile, string dsName = "DS");
	void CreateGeometryShader(wstring shaderFile, string gsName = "GS");

	static void ClearShader();

private:
	void CreateVertexShader();
	void CreatePixelShader();
	
	
	void CheckShaderError(HRESULT hr, ID3DBlob* error);
	void CreateInputLayout();
	
private:
	wstring shaderFile;
	string vsName;
	string psName;
	string hsName;
	string dsName;
	string gsName;
	
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11GeometryShader* geometryShader;

	ID3D11InputLayout* inputLayout;

	ID3DBlob* vertexBlob;
	ID3DBlob* pixelBlob;
	ID3DBlob* hullBlob;
	ID3DBlob* domainBlob;
	ID3DBlob* geometryBlob;

	ID3D11ShaderReflection* reflection;
};