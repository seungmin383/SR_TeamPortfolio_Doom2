#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CUIObject.h"
#include <functional>
#include "CUISet.h"
#include "CDInputManager.h"

class CUIManager :
    public CBase
{
    DECLARE_SINGLETON(CUIManager)

private:
    explicit CUIManager();
    virtual ~CUIManager();

public:
    void                    InsertUISet(CUISet* uiSet);

    //void                    AddUIPhase() { UIPhase++; }
    //void                    SubTractUIPhase() { UIPhase--; }
    void                    ToggleUIPhase();
    void                    ExitUIPhase();
    void                    ActivateUIPhase(_int phaseNumber);
    void                    MappingKeyInput(_ubyte dInput, function<void()> func);

    void                    UpdateUI();

    CUISet*                 GetCurUISet() { return UISetList[CurUIPhase]; }

    void                    GetKeyInput();

    void                    UISetListClear();

private:
    vector<CUISet*>         UISetList;
    _int                    CurUIPhase = 0;
    vector<_bool>           Acts;
    map<_ubyte, function<void()>>       UIKeyInputMap;

private:
    virtual void        Free();
};

