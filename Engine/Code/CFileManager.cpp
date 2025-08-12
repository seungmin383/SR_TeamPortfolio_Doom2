#include "CFileManager.h"

IMPLEMENT_SINGLETON(CFileManager)

CFileManager::CFileManager()
{
	ZeroMemory(&m_currentPath, sizeof(fs::path));
	ZeroMemory(&m_tConfig, sizeof(ConfigModel));

	GetConfig();
}

CFileManager::~CFileManager()
{
	Free();
}

fs::path CFileManager::GetCurrentDir()
{
	return fs::current_path();
}

fs::path CFileManager::GetCanonicalDir()
{
	fs::canonical(m_currentPath);
	return m_currentPath;
}

void CFileManager::GetConfig()
{
	if (!fs::exists(m_ConfigPath))
	{
		MSG_BOX("not found config file");
		return;
	}

	//ifstream read;
	//read.open(m_ConfigPath);

	//if (read.is_open())
	//{
	//	char arr[256];
	//	while (!read.eof())
	//	{
	//		read.getline(arr, 256);
	//	}
	//}

	//read.close();

	if (!fs::exists(m_ConfigPath))
	{
		MSG_BOX("not found config");
		return;
	}

	ifstream read(m_ConfigPath);
	json j = json::parse(read);
	
	m_tConfig = j.get<ConfigModel>();
}

void CFileManager::GetResourceDir()
{
}

void CFileManager::GetTerrainResourceDir()
{
}

vector<fs::path> CFileManager::GetListCurrentDir()
{
	vector<fs::path> result;

	fs::directory_iterator iter(GetCurrentDir());
	while (iter != fs::end(iter))
	{
		result.push_back((*iter).path());
		iter++;
	}

	return result;
}

void CFileManager::SaveTerrain()
{
	//write = j.dump()
}

void CFileManager::LoadTerrain()
{
	fs::path p(m_tConfig.TerrainSavePath);

	if (!fs::exists(p))
	{
		MSG_BOX("not found terrain save");
		return;
	}

	ifstream read(p);
	json j = json::parse(read);

	TerrainModels tm = j.get<TerrainModels>();
	//return tm;
}

void CFileManager::Free()
{
}
