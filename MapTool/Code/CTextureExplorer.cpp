#include "pch.h"
#include "CTextureExplorer.h"
#include "CObjectFactory.h"

CTextureExplorer::CTextureExplorer()
	: TextureRepo(nullptr),
	m_pGraphicDev(nullptr)
{
	CFileManager::GetInstance()->GetConfig();
}

CTextureExplorer::CTextureExplorer(LPDIRECT3DDEVICE9 pGraphicDev)
	: TextureRepo(CTextureManager::GetInstance()),
	m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
	CFileManager::GetInstance()->GetConfig();
}

CTextureExplorer::~CTextureExplorer()
{
	Free();
}

CTextureExplorer* CTextureExplorer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CTextureExplorer(pGraphicDev);
}

void CTextureExplorer::DrawExplorerWindow()
{
	static bool drawTextureList = false;

	if (ImGui::Button("Move Resource Directory"))
	{
		MoveTerrainResourceDir();
		BulkLoadTexture();
	}
	ImGui::SameLine();

	if (ImGui::Button("Move Upper"))
	{
		MoveParentDir();
		BulkLoadTexture();
	}
	ImGui::SameLine();

	ImGui::NewLine();

	ChildListBox();
	FileListListBox();

	ImGui::BeginChild("Textures", ImVec2(0, 420.f));
	TextureList();
	ImGui::EndChild();
}

void CTextureExplorer::ChildListBox()
{
	//dir list box
	static int dir_selected_idx = 0;

	static bool dir_highlight = false;
	int dir_highlighted_idx = -1;

	int diridx = 0;

	if (curChildList.empty())
		curChildList = CFileManager::GetInstance()->GetListChildDir();

	if (ImGui::BeginListBox("Directory list"))
	{
		if (ImGui::Button("Enter") && curChildList.empty() == false)
		{
			MoveDir(curChildList[dir_selected_idx]);
			BulkLoadTexture();
		}

		for (auto p : curChildList)
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
}

void CTextureExplorer::FileListListBox()
{
	//file list box
	static int file_selected_idx = 0;

	static bool file_highlight = false;
	int file_highlighted_idx = -1;

	int fileidx = 0;

	if (curFileList.empty())
		curFileList = CFileManager::GetInstance()->GetListCurrentDir();

	if (ImGui::BeginListBox("File list"))
	{
		for (auto p : curFileList)
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
}

void CTextureExplorer::TextureList()
{
	if (curTextures.empty())
		GetTextures();

	_uint col = 3;

	for (auto t : curTextures)
	{
		if (col < 3)
			ImGui::SameLine();

		auto texRef = (IDirect3DTexture9*)t.second->GetTextureHandle();
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
		if (ImGui::ImageButton(t.first.string().c_str(), my_tex_id, size, uv0, uv1, bg_col, tint_col))
			selectedTexture = t.first;

		col--;
		if (col == 0)
			col = 3;
	}
}

void CTextureExplorer::MoveParentDir()
{
	CFileManager::GetInstance()->MoveParentDir();
	GetChildList();
	GetFileList();
}

void CTextureExplorer::MoveDir(fs::path p)
{
	CFileManager::GetInstance()->MoveDir(p);
	GetChildList();
	GetFileList();
}

void CTextureExplorer::MoveTerrainResourceDir()
{
	CFileManager::GetInstance()->MoveDir(CFileManager::GetInstance()->GetTerrainResourceDir());

	GetChildList();
	GetFileList();
}

void CTextureExplorer::GetChildList()
{
	curChildList = CFileManager::GetInstance()->GetListChildDir();
}

void CTextureExplorer::GetFileList()
{
	curFileList = CFileManager::GetInstance()->GetListCurrentDir();
}

void CTextureExplorer::BulkLoadTexture()
{
	curDir = CFileManager::GetInstance()->GetCurrentDir();

	string curDirName = curDir.filename().string();

	if (curDirName == "DoomTex")
		return;

	_uint cachedIdx = (_uint)CTextureManager::GetInstance()->StringConversion(curDirName);

	if (cacheCheck[cachedIdx] && cachedIdx != (_uint)CTextureManager::terrainDir::ETC)
	{
		curTextures = CTextureManager::GetInstance()->GetTextureGroup(curDir.filename().string());
		return;
	}

	CTextureManager::GetInstance()->BulkLoadTexture(m_pGraphicDev, curDirName);

	cacheCheck[cachedIdx] = true;

	curTextures = CTextureManager::GetInstance()->GetTextureGroup(curDirName);
}

void CTextureExplorer::GetTextures()
{
	curDir = CFileManager::GetInstance()->GetCurrentDir();

	string curDirName = curDir.filename().string();
	_uint cachedIdx = (_uint)CTextureManager::GetInstance()->StringConversion(curDirName);

	if (cacheCheck[cachedIdx] == false || cachedIdx != (_uint)CTextureManager::terrainDir::ETC)
		return;

	curTextures = CTextureManager::GetInstance()->GetTextureGroup(curDirName);
}

string CTextureExplorer::WideToUtf8(const _tchar* wc)
{
	if (!wc) return {};
	int len = WideCharToMultiByte(CP_UTF8, 0, wc, -1, nullptr, 0, nullptr, nullptr);
	std::string out(len ? len - 1 : 0, '\0');
	if (len > 1) WideCharToMultiByte(CP_UTF8, 0, wc, -1, out.data(), len, nullptr, nullptr);
	return out;
}

void CTextureExplorer::Free()
{
	//for_each(curTextures.begin(), curTextures.end(), CDeleteMap());

	m_pGraphicDev->Release();
	CFileManager::GetInstance()->DestroyInstance();
}
