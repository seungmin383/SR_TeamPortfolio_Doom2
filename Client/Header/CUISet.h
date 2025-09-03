#pragma once
#include "CBase.h"
#include "Engine_Define.h"
#include "CUIObject.h"
#include <functional>
#include "CDInputManager.h"

class CUISet :
    public CBase
{
public:
    explicit CUISet();
    virtual ~CUISet();

public:
    void                    InsertUIObject(CUIObject* uiObj, _bool isEnabledFocus);

    _bool                    AddUIElementIndex();
    _bool                    SubTractUIElementIndex();
    _bool                    ActivateUI();
    void                    MappingKeyInput(_ubyte dInput, function<_bool()> func);

    _bool                    ToggleActivate();

    void                    UpdateUI();

    _int                    FocusObserving() { return UIElementFocus; }
    void                    SetPosition(_uint num, _float fX, _float fY);

    void                    GetKeyInput();

    void                    SetUIElementFocus(_int ival) { UIElementFocus = ival; }
    _int                    GetUIElementFocus() { return UIElementFocus; }

    _bool                    SetElementsToggle(_bool bval);

private:
    //TODO: ÁßÃ¸±¸Á¶
    //CUISet*                 NestedUI;
    //_uint                   NestedStep = 0;

    vector<CUIObject*>      UIElements;
    vector<_bool>           FocusEnabled;
    _int                    UIElementFocus = 0;
    _int                    MaxCount = 0;
    map<_ubyte, function<_bool()>>       UIKeyInputMap;

private:
    virtual void            Free();
};

