#include "pch.h"
#include "CControlPanel.h"
#include "CObjectFactory.h"
#include "CPrototypeManager.h"

CControlPanel::CControlPanel()
	: m_pGraphicDev(nullptr),
	textureExpr(nullptr),
	curTex(nullptr)
{
}

CControlPanel::CControlPanel(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr)
	: m_pGraphicDev(pGraphicDev),
	textureExpr(expr),
	curTex(nullptr)
{
	m_pGraphicDev->AddRef();
}

CControlPanel::~CControlPanel()
{
	Free();
}

CControlPanel* CControlPanel::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr)
{
	return new CControlPanel(pGraphicDev, expr);
}

_bool CControlPanel::ControlPanelWindow()
{
	if (ImGui::Button("Show Demo"))
		show_demo_window = !show_demo_window;

	if (curTex != nullptr)
	{
		auto texRef = (IDirect3DTexture9*)curTex->GetTextureHandle();
		D3DSURFACE_DESC surfaceDesc;
		texRef->GetLevelDesc(0, &surfaceDesc);

		float my_tex_w = (float)surfaceDesc.Width;
		float my_tex_h = (float)surfaceDesc.Height;
		ImTextureRef my_tex_id = texRef;

		ImVec2 size = ImVec2(my_tex_w, my_tex_h);                         // Size of the image we want to make visible
		ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
		ImVec2 uv1 = ImVec2(my_tex_w / my_tex_w, my_tex_h / my_tex_h);    // UV coordinates for (32,32) in our texture
		ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

		ImGui::ImageWithBg(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv0, uv1, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	//타일열갯수
	if (ImGui::InputInt("Tile column count", &tCachingControlInfo.tileColCnt))
	{
		if (tCachingControlInfo.tileColCnt < 1)
			tCachingControlInfo.tileColCnt = 1;
		tCachingControlInfo.vertexXCnt = tCachingControlInfo.tileColCnt + 1;

		tCachingControlInfo.vertices.clear();

		DWORD dwIndex = 0;
		_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
		for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
		{
			for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
			{
				dwIndex = tCachingControlInfo.vertexXCnt * i + j;

				tCachingControlInfo.vertices.push_back({ j * tCachingControlInfo.vertexInterval
													, 0.f,
													i * tCachingControlInfo.vertexInterval });

				//tCachingControlInfo.vertices[dwIndex] = { j * tCachingControlInfo.vertexInterval
				//										, 0.f,
				//										i * tCachingControlInfo.vertexInterval };
			}
		}
	}

	//타일행갯수
	if (ImGui::InputInt("Tile row count", &tCachingControlInfo.tileRowCnt))
	{
		if (tCachingControlInfo.tileRowCnt < 1)
			tCachingControlInfo.tileRowCnt = 1;
		tCachingControlInfo.vertexZCnt = tCachingControlInfo.tileRowCnt + 1;

		tCachingControlInfo.vertices.clear();

		DWORD dwIndex = 0;
		_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
		for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
		{
			for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
			{
				dwIndex = tCachingControlInfo.vertexXCnt * i + j;

				tCachingControlInfo.vertices.push_back({ j * tCachingControlInfo.vertexInterval
													, 0.f,
													i * tCachingControlInfo.vertexInterval });

				//tCachingControlInfo.vertices[dwIndex] = { j * tCachingControlInfo.vertexInterval
				//										, 0.f,
				//										i * tCachingControlInfo.vertexInterval };
			}
		}
	}

	//TODO
	//버텍스 선택, 선택한 버택스 값
	if (ImGui::InputFloat3("Input vertex pos", tCachingControlInfo.pos));

	//TODO
	//타일 선택, 선택한 타일 값
	if (ImGui::InputFloat3("Input tile pos", vecCachingVtx))
		tCachingControlInfo.vertices[tCachingControlInfo.vertexIdx] = vecCachingVtx;

	if (ImGui::Button("Select Texture"))
	{
		GetSelectTexture();
		string curDir = CFileManager::GetInstance()->GetCurrentDir().filename().string();
		curTex = CTextureManager::GetInstance()->GetTexture(curDir, tCachingControlInfo.currentTextureTag);
	}

	if (ImGui::Button("Apply"))
		Apply();

	return show_demo_window;
}

void CControlPanel::ControlerWindow()
{
}

void CControlPanel::ChangeTileInfo()
{
	//tControlInfo = tCachingControlInfo;
}

void CControlPanel::AddXTile()
{
}

void CControlPanel::AddZTile()
{
}

void CControlPanel::AddYTile()
{
}

void CControlPanel::SetVertexInfo(_int tileIdx, _int vertexIdx, _vec3 vertexInfo)
{
}

void CControlPanel::GetSelectTexture()
{
	tCachingControlInfo.currentTextureTag = textureExpr->GetSelectTexture();
}

void CControlPanel::SetDefault()
{
	tCachingControlInfo.info = TILEINFO::FLOOR;
	tCachingControlInfo.tileColCnt = 1;
	tCachingControlInfo.tileRowCnt = 1;
	tCachingControlInfo.vertexIdx = 0;
	tCachingControlInfo.vertices.clear();
	tCachingControlInfo.vertexXCnt = 2;
	tCachingControlInfo.vertexZCnt = 2;
	tCachingControlInfo.vertexYCnt = 0;
	tCachingControlInfo.vertexInterval = 1.f;
	tCachingControlInfo.terrainIdx = 0;
	tCachingControlInfo.currentTextureTag = "";
	tCachingControlInfo.pos = _vec3(0.f, 0.f, 0.f);

}

void CControlPanel::Apply()
{
	//오브젝트 생성
	auto pObj = CObjectFactory::GetInstance()->Create_Terrain(tCachingControlInfo.vertexXCnt,
		tCachingControlInfo.vertexZCnt,
		tCachingControlInfo.vertexInterval,
		tCachingControlInfo.vertices,
		(_tchar*)tCachingControlInfo.currentTextureTag.wstring().c_str(),
		tCachingControlInfo.pos);

	vecSector.push_back(dynamic_cast<CTerrainSector*>(pObj));

	tCachingControlInfo.terrainIdx = vecSector.capacity() - 1;

	tControlInfo.push_back(tCachingControlInfo);

	SetDefault();
}

void CControlPanel::Free()
{
	m_pGraphicDev->Release();
}
