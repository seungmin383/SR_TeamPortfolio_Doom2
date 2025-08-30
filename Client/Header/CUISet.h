#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CUIObject.h"
#include <functional>
#include "CDInputManager.h"

class CUISet :
    public CBase
{
private:
    explicit CUISet();
    virtual ~CUISet();

public:
    void                    InsertUIObject(CUIObject* uiObj, _bool isEnabledFocus);

    void                    AddUIElementIndex() { UIElementFocus++; }
    void                    SubTractUIElementIndex() { UIElementFocus--; }
    void                    ActivateUI();
    void                    MappingKeyInput(_ubyte dInput, function<void()> func);

    void                    ToggleActivate();

    void                    UpdateUI();

    _int                    FocusObserving() { return UIElementFocus; }
    void                    SetPosition(_uint num, _float fX, _float fY);

    void                    GetKeyInput();

private:
    //TODO: ÁßÃ¸±¸Á¶
    //CUISet*                 NestedUI;
    //_uint                   NestedStep = 0;

    vector<CUIObject*>      UIElements;
    vector<_bool>           FocusEnabled;
    _int                    UIElementFocus = 0;
    _int                    MaxCount = 0;
    map<_ubyte, function<void()>>       UIKeyInputMap;

private:
    virtual void            Free();
};

