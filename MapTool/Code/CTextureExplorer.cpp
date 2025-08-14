#include "pch.h"
#include "CTextureExplorer.h"

CTextureExplorer::CTextureExplorer()
	: TextureRepo(CTextureManager::GetInstance())
{
	CFileManager::GetInstance()->GetConfig();
	CFileManager::GetInstance()->MoveDir(CFileManager::GetInstance()->GetTerrainResourceDir(););
	

}

CTextureExplorer::~CTextureExplorer()
{
	Free();
}

CTextureExplorer* CTextureExplorer::Create()
{
	return new CTextureExplorer();
}

void CTextureExplorer::DrawExplorerWindow()
{
	ChildListBox();
	FileListListBox();
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

void CTextureExplorer::MoveParentDir()
{
	CFileManager::GetInstance()->MoveParentDir();

}

void CTextureExplorer::MoveDir(fs::path p)
{
	CFileManager::GetInstance()->MoveDir(p);
}

void CTextureExplorer::GetChildList()
{
	curChildList = CFileManager::GetInstance()->GetListChildDir();
}

void CTextureExplorer::GetFileList()
{
	curFileList = CFileManager::GetInstance()->GetListCurrentDir();
}

void CTextureExplorer::Free()
{
}
