#include "pch.h"
#include "CControlPanel.h"
#include "CObjectFactory.h"
#include "CPrototypeManager.h"
#include "CDInputManager.h"
#include "CManagement.h"
#include "CDynamicCamera.h"


CControlPanel::CControlPanel()
	: m_pGraphicDev(nullptr),
	textureExpr(nullptr),
	curTex(nullptr),
	pickingtime(0.f),
	pickingdelay(0.25f),
	EnablePick(false)
{
}

CControlPanel::CControlPanel(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr)
	: m_pGraphicDev(pGraphicDev),
	textureExpr(expr),
	curTex(nullptr),
	pickingtime(0.f),
	pickingdelay(0.25f),
	EnablePick(false)
{
	m_pGraphicDev->AddRef();
	SetDefault();
	SetObjInfo();
	SetLoadSceneTag();
	m_pCalculatorCom = dynamic_cast<CCalculator*>(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
}

CControlPanel::~CControlPanel()
{
	Free();
}

CControlPanel* CControlPanel::Create(LPDIRECT3DDEVICE9 pGraphicDev, CTextureExplorer* expr)
{
	return new CControlPanel(pGraphicDev, expr);
}

_bool CControlPanel::ControlPanelWindow(const _float& timeDelta)
{
	pickingtime += timeDelta;

	if (CDInputManager::GetInstance()->Get_KeyDown(DIK_G))
		EnablePick = !EnablePick;

	if (ImGui::Button("Show Demo"))
		show_demo_window = !show_demo_window;

	//불러올 스테이지 선택
	static int load_current = 0;
	ImGui::Combo("load stage select", &load_current, loadSceneTags, (_uint)(SCENEID::SCENE_END));

	//지형 저장 모달
	if (ImGui::Button("Save terrain"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		//SaveModal();
		ImGui::OpenPopup("Save terrain?");
	}

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save terrain?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
	//지형 저장 끝

	ImGui::SameLine();
	if (ImGui::Button("Save wall and ceil"))
	{

	}

	//아이템 저장 모달
	ImGui::SameLine();
	if (ImGui::Button("Save item"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		//SaveModal();
		ImGui::OpenPopup("Save item?");
	}

	if (ImGui::BeginPopupModal("Save item?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
				SaveItem(selecteddir, filename);
				ImGui::CloseCurrentPopup();
				filename.clear();
			}
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
	//아이템 저장 끝

	//몬스터 저장 모달
	ImGui::SameLine();
	if (ImGui::Button("Save monster"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		//SaveModal();
		ImGui::OpenPopup("Save monster?");
	}

	if (ImGui::BeginPopupModal("Save monster?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
				SaveMonster(selecteddir, filename);
				ImGui::CloseCurrentPopup();
				filename.clear();
			}
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
	//아이템 저장 끝

	if (ImGui::Button("Load terrain"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		LoadTerrain(load_current);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load wall and ceil"))
	{

	}
	ImGui::SameLine();
	if (ImGui::Button("Load item"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		LoadItem(load_current);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load monster"))
	{
		prevPath = CFileManager::GetInstance()->GetCurrentDir();

		LoadMonster(load_current);
	}

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("ControlPanel", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Terrain"))
		{
			ImGui::Text("This is the Terrain tab!");

			TerrainController();
			InputMouse_Terrain();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Object"))
		{
			ImGui::Text("This is the Object tab!");

			ObjectController();
			InputMouse_Object();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Monster"))
		{
			ImGui::Text("This is the Monster tab!");

			ObjectController();
			InputMouse_Monster();

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

	tile_selected_idx = tCachingControlInfo.terrainIdx;

	static bool tile_highlight = false;
	int tile_highlighted_idx = -1;

	int tileidx = 0;

	if (ImGui::BeginListBox("tile list"))
	{

		for (size_t i = 0; i < vecSector.size(); i++)
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
	}
	ImGui::EndDisabled();

	if (ImGui::Button("Rolling"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationX(&a, D3DXToRadian(90));

		vecDegree.x += 90.f;
		if (fabsf(vecDegree.x) >= 360.f)
			vecDegree.x = fabsf(vecDegree.x) - 360.f;

		for (size_t i = 0; i < tCachingControlInfo.vertices.size(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();

	if (ImGui::Button("Pitching"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationY(&a, D3DXToRadian(90));

		vecDegree.y += 90.f;
		if (fabsf(vecDegree.y) >= 360.f)
			vecDegree.y = fabsf(vecDegree.y) - 360.f;

		for (size_t i = 0; i < tCachingControlInfo.vertices.size(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();

	if (ImGui::Button("Yaw"))
	{
		D3DXMATRIX a;
		D3DXMatrixIdentity(&a);
		D3DXMatrixRotationZ(&a, D3DXToRadian(90));

		vecDegree.z += 90.f;
		if (fabsf(vecDegree.z) >= 360.f)
			vecDegree.z = fabsf(vecDegree.z) - 360.f;

		for (size_t i = 0; i < tCachingControlInfo.vertices.size(); i++)
			D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &a);

		Apply();
	}
	ImGui::SameLine();
	ImGui::NewLine();

	if (ImGui::Button("roll++"))
		Rotate(_vec3(1.f, 0.f, 0.f));
	ImGui::SameLine();
	if (ImGui::Button("roll--"))
		Rotate(_vec3(-1.f, 0.f, 0.f));
	ImGui::SameLine();
	if (ImGui::Button("pitch++"))
		Rotate(_vec3(0.f, 1.f, 0.f));
	ImGui::SameLine();
	if (ImGui::Button("pitch--"))
		Rotate(_vec3(0.f, -1.f, 0.f));
	ImGui::SameLine();
	if (ImGui::Button("yaw++"))
		Rotate(_vec3(0.f, 0.f, 1.f));
	ImGui::SameLine();
	if (ImGui::Button("yaw--"))
		Rotate(_vec3(0.f, 0.f, -1.f));
	ImGui::SameLine();
	ImGui::NewLine();

	ImGui::BeginDisabled(true);
	if (ImGui::InputFloat3("rotation x y z", vecDegree));
	ImGui::EndDisabled();

	curTileIdx = ModifyChildWindow();

	ImGui::Checkbox("EnablePick", &EnablePick);

	static int terraintype_current = 0;

	if (!tControlInfo.empty())
		terraintype_current = (int)tControlInfo[tCachingControlInfo.terrainIdx].info;
	else
		terraintype_current = (int)tCachingControlInfo.info;
	if (ImGui::Combo("terraintype", &terraintype_current, terrainTags, 3))
	{
		tCachingControlInfo.info = (TILEINFO)terraintype_current;
		if (!tControlInfo.empty())
			tControlInfo[tCachingControlInfo.terrainIdx].info = (TILEINFO)terraintype_current;
	}
		

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

				tCachingControlInfo.vertices.push_back({ (_float)j
													, 0.f,
													(_float)i });
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

				tCachingControlInfo.vertices.push_back({ (_float)j
													, 0.f,
													(_float)i });
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
	}

	if (ImGui::InputFloat("tile interval X", &tCachingControlInfo.vertexIntervalX, 0.1f, 1.0f, "%.1f"))
	{
		tCachingControlInfo.vertices.clear();

		DWORD dwIndex = 0;
		_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
		for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
		{
			for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
			{
				dwIndex = tCachingControlInfo.vertexXCnt * i + j;

				tCachingControlInfo.vertices.push_back({ (_float)j
													, 0.f,
													(_float)i });
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
	}

	if (ImGui::InputFloat("tile interval Z", &tCachingControlInfo.vertexIntervalZ, 0.1f, 1.0f, "%.1f"))
	{
		tCachingControlInfo.vertices.clear();

		DWORD dwIndex = 0;
		_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
		for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
		{
			for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
			{
				dwIndex = tCachingControlInfo.vertexXCnt * i + j;

				tCachingControlInfo.vertices.push_back({ (_float)j
													, 0.f,
													(_float)i });
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
	}

	if (ImGui::InputFloat("tile interval Y", &tCachingControlInfo.vertexIntervalY, 0.1f, 1.0f, "%.1f"))
	{
		tCachingControlInfo.vertices.clear();

		DWORD dwIndex = 0;
		_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
		for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
		{
			for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
			{
				dwIndex = tCachingControlInfo.vertexXCnt * i + j;

				tCachingControlInfo.vertices.push_back({ (_float)j
													, 0.f,
													(_float)i });
			}
		}

		tCachingControlInfo.vertices.shrink_to_fit();

		if (isModifing)
			Apply();
	}

	//TODO
	//타일 선택, 선택한 타일 값
	if (ImGui::DragFloat3("Input tile pos", tCachingControlInfo.pos, 1.f, -50.f, 300.f, "%.1f") && isModifing)
		Apply();
	if (ImGui::InputFloat("tile pos x", &tCachingControlInfo.pos.x, 1.f, 0.5f, "%.1f"))
		Apply();
	if (ImGui::InputFloat("tile pos y", &tCachingControlInfo.pos.y, 1.f, 0.5f, "%.1f"))
		Apply();
	if (ImGui::InputFloat("tile pos z", &tCachingControlInfo.pos.z, 1.f, 0.5f, "%.1f"))
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
	ImGui::Combo("type", &type_current, typeTags, objPrototypeTags.size());

	//type2 오브젝트id
	static int id_current = 0;
	ImGui::Combo("id", &id_current, idTags, objIdTags.size());

	//type3 여분값

	//tag 기타 태그

	tObjectinfo.type1 = objPrototypeTags[type_current].value;
	tObjectinfo.type2 = objIdTags[id_current].value;
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
	tCachingControlInfo.info = TILEINFO::TERRAIN;
	tCachingControlInfo.tileColCnt = 1;
	tCachingControlInfo.tileRowCnt = 1;
	tCachingControlInfo.vertexIdx = 0;
	tCachingControlInfo.vertices.clear();
	tCachingControlInfo.vertexXCnt = 2;
	tCachingControlInfo.vertexZCnt = 2;
	tCachingControlInfo.vertexYCnt = 0;
	tCachingControlInfo.vertexIntervalX = 1.f;
	tCachingControlInfo.vertexIntervalZ = 1.f;
	tCachingControlInfo.vertexIntervalY = 1.f;
	tCachingControlInfo.terrainIdx = 0;
	//tCachingControlInfo.pos = _vec3(0.f, 0.f, 0.f);

	vecDegree = _vec3(0.f, 0.f, 0.f);

	DWORD dwIndex = 0;
	_int cnt = tCachingControlInfo.vertexZCnt * tCachingControlInfo.vertexXCnt;
	for (size_t i = 0; i < tCachingControlInfo.vertexZCnt; i++)
	{
		for (size_t j = 0; j < tCachingControlInfo.vertexXCnt; j++)
		{
			dwIndex = tCachingControlInfo.vertexXCnt * i + j;

			tCachingControlInfo.vertices.push_back({ (_float)j * tCachingControlInfo.vertexIntervalX
												, 0.f,
												(_float)i * tCachingControlInfo.vertexIntervalZ });
		}
	}

	tCachingControlInfo.vertices.shrink_to_fit();

	applied = false;
	isModifing = false;
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
			tCachingControlInfo.vertexIntervalX,
			tCachingControlInfo.vertexIntervalZ,
			tCachingControlInfo.vertexIntervalY,
			tCachingControlInfo.vertices);

		vecSector[tCachingControlInfo.terrainIdx]->Get_Transform()->Set_Pos(tCachingControlInfo.pos.x, tCachingControlInfo.pos.y, tCachingControlInfo.pos.z);

		if (curTex != nullptr)
			vecSector[tCachingControlInfo.terrainIdx]->Set_TextureCom(curTex);

		tControlInfo[tCachingControlInfo.terrainIdx] = tCachingControlInfo;
	}
	else
	{
		applied = true;
		isModifing = true;

		//오브젝트 생성
		auto pObj = CObjectFactory::GetInstance()->Create_Terrain(vecSector.size(),
			tCachingControlInfo.vertexXCnt,
			tCachingControlInfo.vertexZCnt,
			tCachingControlInfo.vertexIntervalX,
			tCachingControlInfo.vertexIntervalZ,
			tCachingControlInfo.vertexIntervalY,
			tCachingControlInfo.vertices,
			(_tchar*)tCachingControlInfo.currentTextureTag.filename().wstring().c_str(),
			tCachingControlInfo.pos);

		vecSector.push_back(dynamic_cast<CTerrainSector*>(pObj));

		tCachingControlInfo.terrainIdx = vecSector.size() - 1;

		tControlInfo.push_back(tCachingControlInfo);
	}
}

void CControlPanel::SaveTerrain(fs::path path, string filename)
{
	Apply();

	vecTerrainInfos.clear();
	vecTerrainInfos.shrink_to_fit();

	CFileManager::GetInstance()->MoveProjectDir();

	auto basePath = fs::current_path();

	for (auto terraininfo : tControlInfo)
	{
		TerrainModel model;
		model.idx = terraininfo.terrainIdx;
		model.type = (int)terraininfo.info;
		model.pos = terraininfo.pos;
		model.strTextureTag = fs::relative(terraininfo.currentTextureTag, basePath).relative_path().generic_string();
		model.vertexCountX = terraininfo.vertexXCnt;
		model.vertexCountZ = terraininfo.vertexZCnt;
		model.vertexIntervalX = terraininfo.vertexIntervalX;
		model.vertexIntervalZ = terraininfo.vertexIntervalZ;
		model.vertexIntervalY = terraininfo.vertexIntervalY;
		model.vertices = terraininfo.vertices;

		vecTerrainInfos.push_back(model);
	}

	CFileManager::GetInstance()->SaveTerrain(path, filename, vecTerrainInfos);

	CFileManager::GetInstance()->MoveDir(prevPath);

	SetDefault();
}

void CControlPanel::LoadTerrain(int sceneNumber)
{
	TerrainModels tm = CFileManager::GetInstance()->LoadTerrain(sceneNumber);

	vecSector.clear();
	vecSector.shrink_to_fit();

	tControlInfo.clear();
	tControlInfo.shrink_to_fit();

	for (auto m : tm.model)
	{
		tCachingControlInfo.terrainIdx = m.idx;
		tCachingControlInfo.pos = m.pos;
		tCachingControlInfo.currentTextureTag = m.strTextureTag;
		tCachingControlInfo.vertexXCnt = m.vertexCountX;
		tCachingControlInfo.vertexZCnt = m.vertexCountZ;
		tCachingControlInfo.vertexIntervalX = m.vertexIntervalX;
		tCachingControlInfo.vertexIntervalZ = m.vertexIntervalZ;
		tCachingControlInfo.vertexIntervalY = m.vertexIntervalY;
		tCachingControlInfo.vertices = m.vertices;

		//오브젝트 생성
		auto pObj = CObjectFactory::GetInstance()->Load_Terrain(vecSector.size(),
			tCachingControlInfo.vertexXCnt,
			tCachingControlInfo.vertexZCnt,
			tCachingControlInfo.vertexIntervalX,//tCachingControlInfo.vertexInterval,//1.f
			tCachingControlInfo.vertexIntervalZ,
			tCachingControlInfo.vertexIntervalY,
			tCachingControlInfo.vertices,
			tCachingControlInfo.currentTextureTag,
			tCachingControlInfo.pos);

		vecSector.push_back(dynamic_cast<CTerrainSector*>(pObj));

		tControlInfo.push_back(tCachingControlInfo);
	}

	tCachingControlInfo.terrainIdx = vecSector.size() - 1;

	applied = true;
	isModifing = true;

	CFileManager::GetInstance()->MoveDir(prevPath);
}

void CControlPanel::SaveWallAndCeil(fs::path path, string filename)
{
}

void CControlPanel::LoadWallAndCeil()
{
}

void CControlPanel::SaveItem(fs::path path, string filename)
{
	CFileManager::GetInstance()->SaveItem(path, filename, vecItemInfos);

	CFileManager::GetInstance()->MoveDir(prevPath);
}

void CControlPanel::LoadItem(int sceneNumber)
{
	vector<GameObjectSpawnInfo> loadData = CFileManager::GetInstance()->LoadItem(sceneNumber);;

	for (auto item : loadData)
		CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)item.type1, (OBJID)item.type2, item.position);

	for (auto item : loadData)
		vecItemInfos.push_back(item);

	CFileManager::GetInstance()->MoveDir(prevPath);
}

void CControlPanel::SaveMonster(fs::path path, string filename)
{
	CFileManager::GetInstance()->SaveMonster(path, filename, vecMonsterInfos);

	CFileManager::GetInstance()->MoveDir(prevPath);
}

void CControlPanel::LoadMonster(int sceneNumber)
{
	vector<GameObjectSpawnInfo> loadData = CFileManager::GetInstance()->LoadMonster(sceneNumber);;

	for (auto monster : loadData)
		CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)monster.type1, (OBJID)monster.type2, monster.position);

	for (auto monster : loadData)
		vecMonsterInfos.push_back(monster);

	CFileManager::GetInstance()->MoveDir(prevPath);
}

void CControlPanel::SetObjInfo()
{
	auto config = CFileManager::GetInstance()->GetConfig();

	objPrototypeTags = config.ObjPrototypeTags;
	typeTags = new char* [objPrototypeTags.size()];
	for (size_t i = 0; i < objPrototypeTags.size(); i++)
	{
		typeTags[i] = new char[objPrototypeTags[i].tag.size() + 1];
		strcpy_s(typeTags[i], (rsize_t)((objPrototypeTags[i].tag.size() + 1) * sizeof(char)), objPrototypeTags[i].tag.c_str());
	}

	objIdTags = config.ObjIdTags;
	idTags = new char* [objIdTags.size()];
	for (size_t i = 0; i < objIdTags.size(); i++)
	{
		idTags[i] = new char[objIdTags[i].tag.size() + 1];
		strcpy_s(idTags[i], (rsize_t)((objIdTags[i].tag.size() + 1) * sizeof(char)), objIdTags[i].tag.c_str());
	}

	//터레인타입 꼽사리

	terrainTags[0] = "Terrain";
	terrainTags[1] = "Wall";
	terrainTags[2] = "Ceil";
}

void CControlPanel::SetLoadSceneTag()
{
	loadSceneTags = new char* [(_uint)SCENEID::SCENE_END];
	for (size_t i = 0; i < (_uint)SCENEID::SCENE_END; i++)
	{
		string c = to_string(i);
		loadSceneTags[i] = new char[c.size() + 1];
		strcpy_s(loadSceneTags[i], (rsize_t)((c.size() + 1) * sizeof(char)), c.c_str());
	}
}

void CControlPanel::InputMouse_Terrain()
{
	if (!EnablePick)
		return;

	if (pickingtime < pickingdelay)
		return;
	else
		pickingtime = 0.f;

	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		_float nearest(99999.f);

		for (auto item : vecSector)
		{
			auto pTerrainBuf = item->Get_BufferCom();

			if (pTerrainBuf == nullptr)
				return;

			auto pTerrainTransform = item->Get_Transform();

			if (pTerrainTransform == nullptr)
				return;

			_vec3 pos;
			_float dist;
			if (m_pCalculatorCom->Picking_MapTool(g_hWnd, pTerrainBuf, pTerrainTransform, &pos, &dist))
			{
				if (nearest >= dist)
				{
					nearest = dist;
					tCachingControlInfo = tControlInfo[item->Get_Index()];
					curTex = vecSector[tCachingControlInfo.terrainIdx]->Get_TextureCom();

					applied = true;
					isModifing = true;
				}
			}
		}
	}
}

void CControlPanel::InputMouse_Object()
{
	if (!EnablePick)
		return;

	if (pickingtime < pickingdelay)
		return;
	else
		pickingtime = 0.f;

	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		if (tObjectinfo.type1 == 0)
			return;

		if (tObjectinfo.type2 == 0)
			return;

		_float nearest(99999.f);

		for (auto item : vecSector)
		{
			auto pTerrainBuf = item->Get_BufferCom();

			if (pTerrainBuf == nullptr)
				return;

			auto pTerrainTransform = item->Get_Transform();

			if (pTerrainTransform == nullptr)
				return;

			_float dist;
			_vec3 pos;
			if (m_pCalculatorCom->Picking_MapTool(g_hWnd, pTerrainBuf, pTerrainTransform, &pos, &dist))
			{
				if (nearest >= dist)
				{
					nearest = dist;
					tObjectinfo.position = pos;
				}
			}
		}

		if (nearest == 99999.f)
			return;

		CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)tObjectinfo.type1, (OBJID)tObjectinfo.type2, tObjectinfo.position);

		vecItemInfos.push_back(tObjectinfo);
	}
}

void CControlPanel::InputMouse_Monster()
{
	if (!EnablePick)
		return;

	if (pickingtime < pickingdelay)
		return;
	else
		pickingtime = 0.f;

	if (CDInputManager::GetInstance()->Get_DIMouseState(DIM_LB))
	{
		if (tObjectinfo.type1 == 0)
			return;

		if (tObjectinfo.type2 == 0)
			return;

		_float nearest(99999.f);

		for (auto item : vecSector)
		{
			auto pTerrainBuf = item->Get_BufferCom();

			if (pTerrainBuf == nullptr)
				return;

			auto pTerrainTransform = item->Get_Transform();

			if (pTerrainTransform == nullptr)
				return;

			_float dist;
			_vec3 pos;
			if (m_pCalculatorCom->Picking_MapTool(g_hWnd, pTerrainBuf, pTerrainTransform, &pos, &dist))
			{
				if (nearest >= dist)
				{
					nearest = dist;
					tObjectinfo.position = pos;
				}
			}
		}	

		if (nearest == 99999.f)
			return;

		CObjectFactory::GetInstance()->Create_Object((OBJPROTOTYPE)tObjectinfo.type1, (OBJID)tObjectinfo.type2, tObjectinfo.position);

		vecMonsterInfos.push_back(tObjectinfo);
	}
}

void CControlPanel::Rotate(_vec3 degree)
{
	vecDegree += degree;

	if (fabsf(vecDegree.x) >= 360.f)
		vecDegree.x = fabsf(vecDegree.x) - 360.f;
	if (fabsf(vecDegree.y) >= 360.f)
		vecDegree.y = fabsf(vecDegree.y) - 360.f;
	if (fabsf(vecDegree.z) >= 360.f)
		vecDegree.z = fabsf(vecDegree.z) - 360.f;

	D3DXMATRIX axis;
	D3DXMatrixIdentity(&axis);

	D3DXMATRIX x;
	D3DXMatrixIdentity(&x);
	D3DXMatrixRotationX(&x, D3DXToRadian(degree.x));

	D3DXMATRIX y;
	D3DXMatrixIdentity(&y);
	D3DXMatrixRotationY(&y, D3DXToRadian(degree.y));

	D3DXMATRIX z;
	D3DXMatrixIdentity(&z);
	D3DXMatrixRotationZ(&z, D3DXToRadian(degree.z));

	axis = x * y * z;

	for (size_t i = 0; i < tCachingControlInfo.vertices.size(); i++)
		D3DXVec3TransformNormal(&tCachingControlInfo.vertices[i], &tCachingControlInfo.vertices[i], &axis);

	Apply();
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

	if (loadSceneTags != nullptr)
	{
		for (size_t i = 0; i < (_uint)SCENEID::SCENE_END; i++)
			Safe_Delete(loadSceneTags[i]);
		Safe_Delete_Array(loadSceneTags);
	}

	m_pCalculatorCom->Release();

	m_pGraphicDev->Release();
}
