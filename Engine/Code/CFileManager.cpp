#include "CFileManager.h"

IMPLEMENT_SINGLETON(CFileManager)

CFileManager::CFileManager()
{
}

CFileManager::~CFileManager()
{
	Free();
}

fs::path CFileManager::GetCurrentDir()
{
	return fs::current_path();
}

void CFileManager::SetProjectDir()
{
	m_projectPath = fs::current_path();
	LoadConfig();

	MoveParentDir();
	MoveParentDir();
	//솔루션디렉토리라고믿음
	m_basePath = fs::current_path();
	//다시프로젝트로
	MoveProjectDir();
}

void CFileManager::MoveProjectDir()
{
	fs::current_path(m_projectPath);
}

void CFileManager::MoveParentDir()
{
	auto pp = fs::current_path().parent_path();
	fs::current_path(pp);
}

void CFileManager::MoveDir(fs::path p)
{
	fs::current_path(p);
}

void CFileManager::LoadConfig()
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

	read.close();
}

fs::path CFileManager::GetResourceDir()
{
	fs::path p(m_tConfig.ResourcePath);
	return p;
}

fs::path CFileManager::GetTerrainResourceDir()
{
	fs::path p(m_tConfig.TerrainResourcePath);
	return p;
}

vector<fs::path> CFileManager::GetListCurrentDir()
{
	vector<fs::path> result;

	fs::directory_iterator iter(GetCurrentDir());
	while (iter != fs::end(iter))
	{
		if ((*iter).is_directory() == false)
			result.push_back((*iter).path());

		iter++;
	}

	return result;
}

vector<fs::path> CFileManager::GetListChildDir()
{
	vector<fs::path> result;

	fs::directory_iterator iter(GetCurrentDir());
	while (iter != fs::end(iter))
	{
		if ((*iter).is_directory())
			result.push_back((*iter).path());

		iter++;
	}

	return result;
}

void CFileManager::SaveTerrain(fs::path path, string filename, vector<TerrainModel> datas)
{
	MoveProjectDir();

	TerrainModels dataSet;
	dataSet.model = datas;

	auto a = path.string() + "/" + filename;

	ofstream write(path.string() + "/" + filename);

	json j = dataSet;
	write << j.dump();

	write.close();
}

TerrainModels CFileManager::LoadTerrain()
{
	//스테이지마다개별로?
	//한번에 로드해서 들고있게?
	//vector<TerrainModels> ??

	MoveProjectDir();

	TerrainModels tm;

	//TODO: 파일 이름 하드코딩 수정
	fs::path p(m_tConfig.TerrainSavePath + "/" + "terrain");

	if (!fs::exists(p))
	{
		MSG_BOX("not found terrain save");
		return tm;
	}

	ifstream read(p);
	json j = json::parse(read);

	tm = j.get<TerrainModels>();

	read.close();

	return tm;
}

void CFileManager::Free()
{
}
