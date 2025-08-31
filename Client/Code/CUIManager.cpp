#include "pch.h"
#include "CUIManager.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

CUIManager::~CUIManager()
{
	Free();
}

void CUIManager::InsertUISet(CUISet* uiSet)
{
	UISetList.push_back(uiSet);
	MaxCount++;
}

void CUIManager::ToggleUIPhase()
{
}

void CUIManager::ExitUIPhase()
{
}

void CUIManager::ActivateUIPhase(_int phaseNumber)
{
}

void CUIManager::MappingKeyInput(_ubyte dInput, function<void()> func)
{
}

void CUIManager::UpdateUI()
{
	UISetList[CurUIPhase]->UpdateUI();
	GetKeyInput();
}

void CUIManager::GetKeyInput()
{
}

void CUIManager::UISetListClear()
{
	for (auto uiSet : UISetList)
		uiSet->Release();

	UISetList.clear();

	//for (auto keymappair : UIKeyInputMap)
	//	Safe_Delete(keymappair.second);
	
	UIKeyInputMap.clear();

	Acts.clear();
}

void CUIManager::Free()
{
	UISetListClear();
}
