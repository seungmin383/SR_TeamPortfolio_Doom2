#include "pch.h"
#include "CTextureManager.h"
#include "CFileManager.h"
#include "CPrototypeManager.h"

IMPLEMENT_SINGLETON(CTextureManager)

CTextureManager::CTextureManager()
{
}

CTextureManager::~CTextureManager()
{
	Free();
}

void CTextureManager::BulkLoadTexture(LPDIRECT3DDEVICE9 pGraphicDev, terrainDir eDir)
{
	vector<fs::path> list = CFileManager::GetInstance()->GetListCurrentDir();

	if (list.empty())
		return;

	_uint repoIdx = (_uint)eDir;

	auto pm = CPrototypeManager::GetInstance();
	for (auto item : list)
	{
		auto fileName = (const _tchar*)item.filename().string().c_str();
		auto filePath = (const _tchar*)item.string().c_str();
		pm->Ready_ComPrototype(fileName, CTexture::Create(pGraphicDev, filePath, TEX_NORMAL));
		textureRepo[repoIdx].insert({ fileName, dynamic_cast<CTexture*>(pm->Clone_ComPrototype(fileName)) });
	}
}

void CTextureManager::Free()
{
	for (auto m : textureRepo)
	{
		//PrototypeManagerø°º≠ «ÿ¡‹
		m.clear();
	}
}
