#include "../stdafx.h"
#include "Path.h"
#include <string.h>
#include "String.h"

bool Path::ExistFile(string path)
{
	return ExistFile(String::ToWString(path));
}

bool Path::ExistFile(wstring path)
{
	DWORD fileValue = GetFileAttributes(path.c_str());

	return fileValue < 0xFFFFFFFF;
}

bool Path::ExistDirectory(string path)
{
	return ExistDirectory(String::ToWString(path));
}

bool Path::ExistDirectory(wstring path)
{
	DWORD attribute = GetFileAttributes(path.c_str());

	BOOL temp = (attribute != INVALID_FILE_ATTRIBUTES &&
		(attribute & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

string Path::Combine(string path1, string path2)
{
	return path1 + path2;
}

wstring Path::Combine(wstring path1, wstring path2)
{
	return path1 + path2;
}

string Path::Combine(vector<string> paths)
{
	string temp = "";
	for (string path : paths)
		temp += path;

	return temp;
}

wstring Path::Combine(vector<wstring> paths)
{
	wstring temp = L"";
	for (wstring path : paths)
		temp += path;

	return temp;
}

string Path::GetDirectoryName(string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('/');

	return path.substr(0, index + 1);
}

wstring Path::GetDirectoryName(wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('/');

	return path.substr(0, index + 1);
}

string Path::GetExtension(string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('.');

	return path.substr(index + 1, path.length());;
}

wstring Path::GetExtension(wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('.');

	return path.substr(index + 1, path.length());;
}

string Path::GetFileName(string path)
{
	String::Replace(&path, "\\", "/");
	size_t index = path.find_last_of('/');

	return path.substr(index + 1, path.length());
}

wstring Path::GetFileName(wstring path)
{
	String::Replace(&path, L"\\", L"/");
	size_t index = path.find_last_of('/');

	return path.substr(index + 1, path.length());
}

string Path::GetFileNameWithoutExtension(string path)
{
	string fileName = GetFileName(path);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

wstring Path::GetFileNameWithoutExtension(wstring path)
{
	wstring fileName = GetFileName(path);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

string Path::GetFileLocalPath(string path)
{
	string ret = path;

	string fileName = Path::GetFileName(path);
	string temp = GetDirectoryName(path);
	size_t index = temp.length();
	string temp2 = "";

	for (UINT i = 6; i < index; i++)
	{
		//Models
		temp2 = temp.substr(index - i, 6);
		if (temp2 == "Models")
		{
			string temp3 = temp.substr(index - i + 7, i);
			ret = String::ToString(Models) + temp3 + fileName;
			return ret;
		}
		//Jsons
		temp2 = temp.substr(index - i, 5);
		if (temp2 == "_Json")
		{
			string temp3 = temp.substr(index - i + 6, i);
			ret = String::ToString(Jsons) + temp3 + fileName;
			return ret;
		}
		//Assets
		temp2 = temp.substr(index - i, 7);
		if (temp2 == "_Assets")
		{
			string temp3 = temp.substr(index - i + 8, i);
			ret = String::ToString(Assets) + temp3 + fileName;
			return ret;
		}
		//Contents
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "_Contents")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(Contents) + temp3 + fileName;
			return ret;
		}
		//Textures
		temp2 = temp.substr(index - i, 8);
		if (temp2 == "Textures")
		{
			string temp3 = temp.substr(index - i + 9, i);
			ret = String::ToString(Textures) + temp3 + fileName;
			return ret;
		}
		//FbxModels
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "FbxModels")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(FbxModels) + temp3 + fileName;
			return ret;
		}
		//Shaders
		temp2 = temp.substr(index - i, 8);
		if (temp2 == "_Shaders")
		{
			string temp3 = temp.substr(index - i + 9, i);
			ret = String::ToString(Shaders) + temp3 + fileName;
			return ret;
		}
		//Landscapes
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "Landscape")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(Landscapes) + temp3 + fileName;
			return ret;
		}
	}


	return ret;
}

wstring Path::GetFileLocalPath(wstring path)
{
	wstring ret = path;

	wstring fileName = Path::GetFileName(path);
	wstring temp = GetDirectoryName(path);
	size_t index = temp.length();
	wstring temp2 = L"";

	for (UINT i = 6; i < index; i++)
	{
		//Models
		temp2 = temp.substr(index - i, 6);
		if (temp2 == L"Models")
		{
			wstring temp3 = temp.substr(index - i + 7, i);
			ret = Models + temp3 + fileName;
			return ret;
		}
		//Jsons
		temp2 = temp.substr(index - i, 5);
		if (temp2 == L"_Json")
		{
			wstring temp3 = temp.substr(index - i + 6, i);
			ret = Jsons + temp3 + fileName;
			return ret;
		}
		//Assets
		temp2 = temp.substr(index - i, 7);
		if (temp2 == L"_Assets")
		{
			wstring temp3 = temp.substr(index - i + 8, i);
			ret = Assets + temp3 + fileName;
			return ret;
		}
		//Contents
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"_Contents")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = Contents + temp3 + fileName;
			return ret;
		}
		//Textures
		temp2 = temp.substr(index - i, 8);
		if (temp2 == L"Textures")
		{
			wstring temp3 = temp.substr(index - i + 9, i);
			ret = Textures + temp3 + fileName;
			return ret;
		}
		//FbxModels
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"FbxModels")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = FbxModels + temp3 + fileName;
			return ret;
		}
		//Shaders
		temp2 = temp.substr(index - i, 8);
		if (temp2 == L"_Shaders")
		{
			wstring temp3 = temp.substr(index - i + 9, i);
			ret = Shaders + temp3 + fileName;
			return ret;
		}
		//Landscapes
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"Landscape")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = Landscapes + temp3 + fileName;
			return ret;
		}

	}

	return ret;
}

string Path::GetFileLocalPathDirectory(string path)
{
	string ret = path;

	string fileName = Path::GetFileName(path);
	string temp = GetDirectoryName(path);
	size_t index = temp.length();
	string temp2 = "";

	for (UINT i = 6; i < index; i++)
	{
		//Models
		temp2 = temp.substr(index - i, 6);
		if (temp2 == "Models")
		{
			string temp3 = temp.substr(index - i + 7, i);
			ret = String::ToString(Models) + temp3;
			return ret;
		}
		//Jsons
		temp2 = temp.substr(index - i, 5);
		if (temp2 == "_Json")
		{
			string temp3 = temp.substr(index - i + 6, i);
			ret = String::ToString(Jsons) + temp3;
			return ret;
		}
		//Assets
		temp2 = temp.substr(index - i, 7);
		if (temp2 == "_Assets")
		{
			string temp3 = temp.substr(index - i + 8, i);
			ret = String::ToString(Assets) + temp3;
			return ret;
		}
		//Contents
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "_Contents")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(Contents) + temp3;
			return ret;
		}
		//Textures
		temp2 = temp.substr(index - i, 8);
		if (temp2 == "Textures")
		{
			string temp3 = temp.substr(index - i + 9, i);
			ret = String::ToString(Textures) + temp3;
			return ret;
		}
		//FbxModels
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "FbxModels")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(FbxModels) + temp3;
			return ret;
		}
		//Shaders
		temp2 = temp.substr(index - i, 8);
		if (temp2 == "_Shaders")
		{
			string temp3 = temp.substr(index - i + 9, i);
			ret = String::ToString(Shaders) + temp3;
			return ret;
		}
		//Landscapes
		temp2 = temp.substr(index - i, 9);
		if (temp2 == "Landscape")
		{
			string temp3 = temp.substr(index - i + 10, i);
			ret = String::ToString(Landscapes) + temp3;
			return ret;
		}
	}


	return ret;
}

wstring Path::GetFileLocalPathDirectory(wstring path)
{
	wstring ret = path;

	wstring fileName = Path::GetFileName(path);
	wstring temp = GetDirectoryName(path);
	size_t index = temp.length();
	wstring temp2 = L"";

	for (UINT i = 6; i < index; i++)
	{
		//Models
		temp2 = temp.substr(index - i, 6);
		if (temp2 == L"Models")
		{
			wstring temp3 = temp.substr(index - i + 7, i);
			ret = Models + temp3;
			return ret;
		}
		//Jsons
		temp2 = temp.substr(index - i, 5);
		if (temp2 == L"_Json")
		{
			wstring temp3 = temp.substr(index - i + 6, i);
			ret = Jsons + temp3;
			return ret;
		}
		//Assets
		temp2 = temp.substr(index - i, 7);
		if (temp2 == L"_Assets")
		{
			wstring temp3 = temp.substr(index - i + 8, i);
			ret = Assets + temp3;
			return ret;
		}
		//Contents
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"_Contents")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = Contents + temp3;
			return ret;
		}
		//Textures
		temp2 = temp.substr(index - i, 8);
		if (temp2 == L"Textures")
		{
			wstring temp3 = temp.substr(index - i + 9, i);
			ret = Textures + temp3;
			return ret;
		}
		//FbxModels
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"FbxModels")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = FbxModels + temp3;
			return ret;
		}
		//Shaders
		temp2 = temp.substr(index - i, 8);
		if (temp2 == L"_Shaders")
		{
			wstring temp3 = temp.substr(index - i + 9, i);
			ret = Shaders + temp3;
			return ret;
		}
		//Landscapes
		temp2 = temp.substr(index - i, 9);
		if (temp2 == L"Landscape")
		{
			wstring temp3 = temp.substr(index - i + 10, i);
			ret = Landscapes + temp3;
			return ret;
		}

	}

	return ret;
}


const WCHAR* Path::ImageFilter = L"Image\0*.png;*.bmp;*.jpg";
const WCHAR* Path::BinModelFilter = L"Binary Model\0*.model";
const WCHAR* Path::FbxModelFilter = L"Fbx Model\0*.fbx;*.obj\0";
const WCHAR* Path::ShaderFilter = L"HLSL file\0*.hlsl";
const WCHAR* Path::TerrainFilter = L"Terrain file\0*.terrain";
const WCHAR* Path::JsonFilter = L"Json File\0*.json";

void Path::OpenFileDialog(wstring file, const WCHAR* filter, wstring folder, function<void(wstring)> func, HWND hwnd)
{
	WCHAR name[255];
	wcscpy_s(name, file.c_str());

	wstring tempFolder = folder;
	String::Replace(&tempFolder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = name;
	ofn.lpstrFileTitle = L"불러오기";
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = tempFolder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		if (func != NULL)
		{
			wstring loadName = name;
			String::Replace(&loadName, L"\\", L"/");

			func(loadName);
		}
	}
}

void Path::SaveFileDialog(wstring file, const WCHAR* filter, wstring folder, function<void(wstring)> func, HWND hwnd)
{
	WCHAR name[255];
	wcscpy_s(name, file.c_str());

	wstring tempFolder = folder;
	String::Replace(&tempFolder, L"/", L"\\");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = name;
	ofn.lpstrFileTitle = L"저장하기";
	ofn.nMaxFile = 255;
	ofn.lpstrInitialDir = tempFolder.c_str();
	ofn.Flags = OFN_NOCHANGEDIR;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		if (func != NULL)
		{
			wstring loadName = name;
			String::Replace(&loadName, L"\\", L"/");

			func(loadName);
		}
	}
}

void Path::GetFiles(vector<string>& files, string path, string filter, bool bFindSubFolder)
{
	vector<wstring> wFiles;
	wstring wPath = String::ToWString(path);
	wstring wFilter = String::ToWString(filter);

	GetFiles(wFiles, wPath, wFilter, bFindSubFolder);

	for (wstring str : wFiles)
		files.push_back(String::ToString(str));
}

//path : ../Temp/
//filter : *.txt
void Path::GetFiles(vector<wstring>& files, wstring path, wstring filter, bool bFindSubFolder)
{
	wstring file = path + filter;
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(file.c_str(), &findData);

	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bFindSubFolder == true && findData.cFileName[0] != '.')
				{
					wstring folder = path + wstring(findData.cFileName) + L"/";

					GetFiles(files, folder, filter, bFindSubFolder);
				}
			}
			else
			{
				wstring fileName = path + wstring(findData.cFileName);
				files.push_back(fileName);
			}

		} while (FindNextFile(handle, &findData));

		FindClose(handle);
	}
}
//player 이동, 이동에 맞춘 애니메이션
//지형에 오브젝트들이랑 충돌, box충돌로 하시오
//클릭하면 해당 방향으로 라인 그리고 맞으면 빨간색으로 하든지 식으로