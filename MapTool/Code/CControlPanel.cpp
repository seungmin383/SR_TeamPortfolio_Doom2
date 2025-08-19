#include "pch.h"
#include "CControlPanel.h"
#include "CObjectFactory.h"
#include "CPrototypeManager.h"
#include "CDInputManager.h"

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
	SetDefault();
	SetObjInfo();
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

	if (ImGui::Button("Save terrain"))
	{
		//SaveModal();
		ImGui::OpenPopup("Save?");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("check save type and file name");
		ImGui::Separator();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PopStyleVar();

		fs::path selecteddir = SaveExplorer();

		ImGui::InputText("file name", &filename);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			if (!filename.empty())
			{
				SaveTerrain(selecteddir, filename);
				ImGui::CloseCurrentPopup();
				filename.clear();
			}
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	if (ImGui::Button("Save wall and ceil"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Save item"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Save monster"))
	{

	}

	if (ImGui::Button("Load terrain"))
	{
		LoadTerrain();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load wall and ceil"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Load item"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Load monster"))
	{

	}

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("ControlPanel", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Terrain"))
		{
			ImGui::Text("This is the Terrain tab!");

			TerrainController();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Object"))
		{
			ImGui::Text("This is the Object tab!");

			ObjectController();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Temp"))
		{
			ImGui::Text("This is the Temp tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	return show_demo_window;
}

_uint CControlPanel::ModifyChildWindow()
{
	ImGui::BeginChild("Modify item select", ImVec2(0.f, 130.f));

	static int tile_selected_idx = 0;

	static bool tile_highlight = false;
	int tile_highlighted_idx = -1;

	int tileidx = 0;

	if (ImGui::BeginListBox("tile list"))
	{

		for (size_t i = 0; i < vecSector.capacity(); i++)
		{
			const bool tile_is_selected = (tile_selected_idx == tileidx);
			if (ImGui::Selectable(to_string(i).c_str(), tile_is_selected))
				tile_selected_idx = tileidx;

			if (tile_highlight && ImGui::IsItemHovered())
				tile_highlighted_idx = tileidx;

			if (tile_is_selected)
				ImGui::SetItemDefaultFocus();

			tileidx++;
		}

		ImGui::EndListBox();
	}

	ImGui::EndChild();

	return tile_selected_idx;
}

fs::path CControlPanel::SaveExplorer()
{
	if (ImGui::Button("Move Asset Directory"))
	{
		fs::path assetpath = CFileManager::GetInstance()->GetAssetDir();
		CFileManager::GetInstance()->MoveProjectDir();
		CFileManager::GetInstance()->MoveDir(assetpath);
	}
	ImGui::SameLine();

	if (ImGui::Button("Move Upper"))
		CFileManager::GetInstance()->MoveParentDir();

	//dir list box
	static int dir_selected_idx = 0;

	static bool dir_highlight = false;
	int dir_highlighted_idx = -1;

	int diridx = 0;

	auto dir = CFileManager::GetInstance()->GetListChildDir();

	if (ImGui::Button("Enter"))
		CFileManager::GetInstance()->MoveDir(dir[dir_selected_idx]);

	if (ImGui::BeginListBox("Directory list"))
	{
		for (auto p : dir)
		{
			const bool dir_is_selected = (dir_selected_idx == diridx);
			if (ImGui::Selectable((const char*)p.filename().string().c_str(), dir_is_selected))
				dir_selected_idx = diridx;

			if (dir_highlight && ImGui::IsItemHovered())
				dir_highlighted_idx = diridx;

			if (dir_is_selected)
				ImGui::SetItemDefaultFocus();

			diridx++;
		}
		ImGui::EndListBox();
	}
	//dir box end

	//file list box
	static int file_selected_idx = 0;

	static bool file_highlight = false;
	int file_highlighted_idx = -1;

	int fileidx = 0;

	auto file = CFileManager::GetInstance()->GetListCurrentDir();

	if (ImGui::BeginListBox("File list"))
	{
		for (auto p : file)
		{
			const bool file_is_selected = (file_selected_idx == fileidx);
			if (ImGui::Selectable((const char*)p.filename().string().c_str(), file_is_selected))
				file_selected_idx = fileidx;

			if (file_highlight && ImGui::IsItemHovered())
				file_highlighted_idx = fileidx;

			if (file_is_selected)
				ImGui::SetItemDefaultFocus();

			fileidx++;
		}
		ImGui::EndListBox();
	}
	//list box end

	if (dir.empty())
	{
		return CFileManager::GetInstance()->GetCurrentDir();
	}
	else
	{
		return dir[dir_selected_idx];
	}
}

void CControlPanel::TerrainController()
{
	ImGui::BeginDisabled(!applied);
	if (ImGui::Button("New"))
	{
		//new할 뭐시기 저시기
		SetDefault();
		applied = false;
		isModifing = false;
	}
	ImGui::EndDisabled();

	if (ImGui::Button("Rolling"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationX(&a, D3DXToRadian(90));

		for (size_t i = 0; i < tCachingControlInfo.vertices.capacity(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();

	if (ImGui::Button("Pitching"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationY(&a, D3DXToRadian(90));

		for (size_t i = 0; i < tCachingControlInfo.vertices.capacity(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();

	if (ImGui::Button("Yaw"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationZ(&a, D3DXToRadian(90));

		for (size_t i = 0; i < tCachingControlInfo.vertices.capacity(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();
	ImGui::NewLine();

	curTileIdx = ModifyChildWindow();

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

				tCachingControlInfo.vertices.push_back({ (_float)j * tCachingControlInfo.vertexInterval
													, 0.f,
													(_float)i * tCachingControlInfo.vertexInterval });

				//tCachingControlInfo.vertices[dwIndex] = { j * tCachingControlInfo.vertexInterval
				//										, 0.f,
				//										i * tCachingControlInfo.vertexInterval };
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
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

				tCachingControlInfo.vertices.push_back({ (_float)j * tCachingControlInfo.vertexInterval
													, 0.f,
													(_float)i * tCachingControlInfo.vertexInterval });

				//tCachingControlInfo.vertices[dwIndex] = { j * tCachingControlInfo.vertexInterval
				//										, 0.f,
				//										i * tCachingControlInfo.vertexInterval };
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
	}

	if (ImGui::InputInt("tile interval", &tCachingControlInfo.vertexInterval))
		Apply();

	//TODO
	//버텍스 선택, 선택한 버택스 값
	if (ImGui::DragFloat3("Input vertex pos", vecCachingVtx))
		tCachingControlInfo.vertices[tCachingControlInfo.vertexIdx] = vecCachingVtx;

	//TODO
	//타일 선택, 선택한 타일 값
	if (ImGui::DragFloat3("Input tile pos", tCachingControlInfo.pos, tCachingControlInfo.vertexInterval, -50.f, 300.f, "%.1f") && isModifing)
		Apply();
	if (ImGui::InputFloat("tile pos x", &tCachingControlInfo.pos.x, tCachingControlInfo.vertexInterval, 0.5f, "%.1f"))
		Apply();
	if (ImGui::InputFloat("tile pos y", &tCachingControlInfo.pos.y, tCachingControlInfo.vertexInterval, 0.5f, "%.1f"))
		Apply();
	if (ImGui::InputFloat("tile pos z", &tCachingControlInfo.pos.z, tCachingControlInfo.vertexInterval, 0.5f, "%.1f"))
		Apply();

	if (ImGui::Button("Select Texture"))
	{
		GetSelectTexture();
		if (tCachingControlInfo.currentTextureTag != L"")
		{
			string curDir = CFileManager::GetInstance()->GetCurrentDir().filename().string();
			curTex = CTextureManager::GetInstance()->GetTexture(curDir, tCachingControlInfo.currentTextureTag);
		}
	}

	if (ImGui::Button("Apply"))
	{
		if (curTex == nullptr)
		{
			GetSelectTexture();
			if (tCachingControlInfo.currentTextureTag != L"")
			{
				string curDir = CFileManager::GetInstance()->GetCurrentDir().filename().string();
				curTex = CTextureManager::GetInstance()->GetTexture(curDir, tCachingControlInfo.currentTextureTag);
			}
		}
		Apply();
	}
}

void CControlPanel::ObjectController()
{
	//type1 프로토타입
	static int type_current = 0;
	ImGui::Combo("type", &type_current, typeTags, IM_ARRAYSIZE(*typeTags));

	//type2 오브젝트id
	static int id_current = 0;
	ImGui::Combo("id", &id_current, idTags, IM_ARRAYSIZE(*idTags));

	//type3 여분값
}

void CControlPanel::ChangeTileInfo()
{
	//tControlInfo = tCachingControlInfo;
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
	//tCachingControlInfo.pos = _vec3(0.f, 0.f, 0.f);

	DWORD dwIndex = 0;
	_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
	for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
	{
		for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
		{
			dwIndex = tCachingControlInfo.vertexXCnt * i + j;

			tCachingControlInfo.vertices.push_back({ (_float)j * tCachingControlInfo.vertexInterval
												, 0.f,
												(_float)i * tCachingControlInfo.vertexInterval });

			//tCachingControlInfo.vertices[dwIndex] = { j * tCachingControlInfo.vertexInterval
			//										, 0.f,
			//										i * tCachingControlInfo.vertexInterval };
		}
	}

	tCachingControlInfo.vertices.shrink_to_fit();
}

void CControlPanel::Apply()
{
	if (tCachingControlInfo.currentTextureTag == "")
		return;

	if (isModifing && applied)
	{
		vecSector[tCachingControlInfo.terrainIdx]->Mod_BufferCom(
			tCachingControlInfo.vertexXCnt,
			tCachingControlInfo.vertexZCnt,
			tCachingControlInfo.vertexInterval,
			tCachingControlInfo.vertices);

		vecSector[tCachingControlInfo.terrainIdx]->Get_Transform()->Set_Pos(tCachingControlInfo.pos.x, tCachingControlInfo.pos.y, tCachingControlInfo.pos.z);

		vecSector[tCachingControlInfo.terrainIdx]->Set_TextureCom(curTex);

		tControlInfo[tCachingControlInfo.terrainIdx] = tCachingControlInfo;
	}
	else
	{
		applied = true;
		isModifing = true;

		//오브젝트 생성
		auto pObj = CObjectFactory::GetInstance()->Create_Terrain(tCachingControlInfo.vertexXCnt,
			tCachingControlInfo.vertexZCnt,
			tCachingControlInfo.vertexInterval,
			tCachingControlInfo.vertices,
			(_tchar*)tCachingControlInfo.currentTextureTag.filename().wstring().c_str(),
			tCachingControlInfo.pos);

		vecSector.push_back(dynamic_cast<CTerrainSector*>(pObj));

		tCachingControlInfo.terrainIdx = vecSector.capacity() - 1;

		tControlInfo.push_back(tCachingControlInfo);
	}
}

void CControlPanel::SaveTerrain(fs::path path, string filename)
{
	CFileManager::GetInstance()->MoveProjectDir();

	auto basePath = fs::current_path();

	for (auto terraininfo : tControlInfo)
	{
		TerrainModel model;
		model.idx = terraininfo.terrainIdx;
		model.pos = terraininfo.pos;
		model.strTextureTag = fs::relative(terraininfo.currentTextureTag, basePath).relative_path().generic_string();
		model.vertexCountX = terraininfo.vertexXCnt;
		model.vertexCountZ = terraininfo.vertexZCnt;
		model.vertexInterval = terraininfo.vertexInterval;
		model.vertices = terraininfo.vertices;

		vecTerrainInfos.push_back(model);
	}

	CFileManager::GetInstance()->SaveTerrain(path, filename, vecTerrainInfos);
}

void CControlPanel::LoadTerrain()
{
	TerrainModels tm = CFileManager::GetInstance()->LoadTerrain();

	vecSector.clear();
	vecSector.shrink_to_fit();

	for (auto m : tm.model)
	{
		tCachingControlInfo.terrainIdx = m.idx;
		tCachingControlInfo.pos = m.pos;
		tCachingControlInfo.currentTextureTag = m.strTextureTag;
		tCachingControlInfo.vertexXCnt = m.vertexCountX;
		tCachingControlInfo.vertexZCnt = m.vertexCountZ;
		tCachingControlInfo.vertexInterval = m.vertexInterval;
		tCachingControlInfo.vertices = m.vertices;

		//오브젝트 생성
		auto pObj = CObjectFactory::GetInstance()->Load_Terrain(tCachingControlInfo.vertexXCnt,
			tCachingControlInfo.vertexZCnt,
			tCachingControlInfo.vertexInterval,
			tCachingControlInfo.vertices,
			tCachingControlInfo.currentTextureTag,
			tCachingControlInfo.pos);

		vecSector.push_back(dynamic_cast<CTerrainSector*>(pObj));

		tControlInfo.push_back(tCachingControlInfo);
	}

	tCachingControlInfo.terrainIdx = vecSector.capacity() - 1;

	applied = true;
	isModifing = true;
}

void CControlPanel::SaveWallAndCeil(fs::path path, string filename)
{
}

void CControlPanel::LoadWallAndCeil()
{
}

void CControlPanel::SaveItem(fs::path path, string filename)
{
}

void CControlPanel::LoadItem()
{
}

void CControlPanel::SaveMonster(fs::path path, string filename)
{
}

void CControlPanel::LoadMonster()
{
}

void CControlPanel::SetObjInfo()
{
	auto config = CFileManager::GetInstance()->GetConfig();

	objPrototypeTags = config.ObjPrototypeTags;
	typeTags = new char*[objPrototypeTags.size()];
	for (size_t i = 0; i < objPrototypeTags.size(); i++)
	{
		typeTags[i] = new char[objPrototypeTags[i].tag.size() + 1];
		strcpy_s(typeTags[i], (rsize_t)((objPrototypeTags[i].tag.size() + 1) * sizeof(char)), objPrototypeTags[i].tag.c_str());
	}
		
	objIdTags = config.ObjIdTags;
	idTags = new char*[objIdTags.size()];
	for (size_t i = 0; i < objIdTags.size(); i++)
	{
		idTags[i] = new char[objIdTags[i].tag.size() + 1];
		strcpy_s(idTags[i], (rsize_t)((objIdTags[i].tag.size() + 1) * sizeof(char)), objIdTags[i].tag.c_str());
	}
}

void CControlPanel::InputMouse()
{
	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		m_pCalculatorCom->Picking_GameObject(g_hWnd);
	}
}

void CControlPanel::Free()
{
	if (typeTags != nullptr)
	{
		for (size_t i = 0; i < objPrototypeTags.size(); i++)
			Safe_Delete(typeTags[i]);
		Safe_Delete_Array(typeTags);
	}
	
	if (idTags != nullptr)
	{
		for (size_t i = 0; i < objIdTags.size(); i++)
			Safe_Delete(idTags[i]);
		Safe_Delete_Array(idTags);
	}

	m_pGraphicDev->Release();
}
