#pragma once
class CGameUI_Face;

// C++17을 안 써도 되게 extern 선언만
namespace Client {
    extern CGameUI_Face* g_FaceUI;  // 프로그램 전체에서 1개만 쓸 포인터
}
