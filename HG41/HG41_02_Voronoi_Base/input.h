#pragma once


class CInput
{
private:
	static BYTE m_OldKeyState[256];
	static BYTE m_KeyState[256];
	static POINT m_MousePos;
	static bool isMouseClick;


public:
	static void Init();
	static void Uninit();
	static void Update();

	static bool GetKeyPress( BYTE KeyCode );
	static bool GetKeyTrigger( BYTE KeyCode );
	static XMFLOAT2 GetMousePos() { return { (float)m_MousePos.x,(float)m_MousePos.y}; }

	static void SetMouseState(bool isclick) { isMouseClick = isclick; };
	static bool IsMouseClick() { return isMouseClick; }

};
