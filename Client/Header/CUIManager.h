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

protected:
    explicit CUIManager();
    virtual ~CUIManager();

public:
    void                    InsertUISet(CUISet* uiSet, _bool act, _bool isEnabledFocus);

    void                    SetUIPhase(_int ival) { CurUIPhase = ival; }
    _int                    GetUIPhase() { return CurUIPhase; }

    _bool                    AddUIPhase();
    _bool                    SubTractUIPhase();

    _bool                    ToggleUIPhase();
    _bool                    ExitUIPhase();
    _bool                    ActivateUIPhase(_int phaseNumber);
    void                    MappingKeyInput(_ubyte dInput, function<_bool()> func);

    void                    UpdateUI();

    CUISet*                 GetCurUISet() { return UISetList[CurUIPhase]; }

    void                    GetKeyInput();

    void                    UISetListClear();

private:
    vector<CUISet*>         UISetList;
    _int                    CurUIPhase = 0;
    _int                    MaxCount = 0;
    vector<_bool>           FocusEnabled;
    vector<_bool>           Acts;
    map<_ubyte, function<_bool()>>       UIKeyInputMap;

private:
    virtual void        Free();
};

