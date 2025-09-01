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
	write << j.dump(4);

	write.close();
}

TerrainModels CFileManager::LoadTerrain(_int stageNumber)
{
	MoveProjectDir();

	TerrainModels tm;

	vector<StageSet>::iterator stage = find_if(m_tConfig.StageSettings.begin(), m_tConfig.StageSettings.end(), [&stageNumber](StageSet item) {return item.StageNumber == stageNumber; });

	fs::path p(m_tConfig.TerrainSavePath + "/" + (*stage).terrainFileName);

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

void CFileManager::SaveItem(fs::path path, string filename, vector<GameObjectSpawnInfo> datas)
{
	MoveProjectDir();

	SceneSpawnInfo dataSet;
	dataSet.objects = datas;

	auto a = path.string() + "/" + filename;

	ofstream write(path.string() + "/" + filename);

	json j = dataSet;
	write << j.dump(4);

	write.close();
}

vector<GameObjectSpawnInfo> CFileManager::LoadItem(_int stageNumber)
{
	MoveProjectDir();

	SceneSpawnInfo s;

	vector<StageSet>::iterator stage = find_if(m_tConfig.StageSettings.begin(), m_tConfig.StageSettings.end(), [&stageNumber](StageSet item) {return item.StageNumber == stageNumber; });

	fs::path p(m_tConfig.ObjectSpawnSavePath + "/" + (*stage).ObjectFileName);

	if (!fs::exists(p))
	{
		MSG_BOX("not found item save");
		return s.objects;
	}

	ifstream read(p);
	json j = json::parse(read);

	s = j.get<SceneSpawnInfo>();

	read.close();

	return s.objects;
}

vector<GameObjectSpawnInfo> CFileManager::LoadAdditionalItem(_int stageNumber, _int phaseNumber)
{
	MoveProjectDir();

	SceneSpawnInfo s;

	vector<StageSet>::iterator stage = find_if(m_tConfig.StageSettings.begin(), m_tConfig.StageSettings.end(), [&stageNumber](StageSet item) { return item.StageNumber == stageNumber; });

	fs::path p(m_tConfig.ObjectSpawnSavePath + "/" + (*stage).AdditionalObject[phaseNumber]);

	if (!fs::exists(p))
	{
		MSG_BOX("not found item save");
		return s.objects;
	}

	ifstream read(p);
	json j = json::parse(read);

	s = j.get<SceneSpawnInfo>();

	read.close();

	return s.objects;
}

void CFileManager::SaveMonster(fs::path path, string filename, vector<GameObjectSpawnInfo> datas)
{
	MoveProjectDir();

	SceneSpawnInfo dataSet;
	dataSet.objects = datas;

	auto a = path.string() + "/" + filename;

	ofstream write(path.string() + "/" + filename);

	json j = dataSet;
	write << j.dump(4);

	write.close();
}

vector<GameObjectSpawnInfo> CFileManager::LoadMonster(_int stageNumber)
{
	MoveProjectDir();

	SceneSpawnInfo s;

	vector<StageSet>::iterator stage = find_if(m_tConfig.StageSettings.begin(), m_tConfig.StageSettings.end(), [&stageNumber](StageSet item) {return item.StageNumber == stageNumber; });

	fs::path p(m_tConfig.MonsterSpawnSavePath + "/" + (*stage).MonsterFileName);

	if (!fs::exists(p))
	{
		MSG_BOX("not found monster save");
		return s.objects;
	}

	ifstream read(p);
	json j = json::parse(read);

	s = j.get<SceneSpawnInfo>();

	read.close();

	return s.objects;
}

vector<GameObjectSpawnInfo> CFileManager::LoadAdditionalMonster(_int stageNumber, _int phaseNumber)
{
	MoveProjectDir();

	SceneSpawnInfo s;

	vector<StageSet>::iterator stage = find_if(m_tConfig.StageSettings.begin(), m_tConfig.StageSettings.end(), [&stageNumber](StageSet item) { return item.StageNumber == stageNumber; });

	fs::path p(m_tConfig.MonsterSpawnSavePath + "/" + (*stage).AdditionalMonster[phaseNumber]);

	if (!fs::exists(p))
	{
		MSG_BOX("not found additional monster save");
		return s.objects;
	}

	ifstream read(p);
	json j = json::parse(read);

	s = j.get<SceneSpawnInfo>();

	read.close();

	return s.objects;
}

void CFileManager::Free()
{
}
