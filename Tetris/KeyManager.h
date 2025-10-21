#pragma once

class KeyManager {
public:
    static bool leftKeyPressed;
    static bool rightKeyPressed;
    static bool downKeyPressed;
    static bool qKeyPressed;
    static bool eKeyPressed;

    static KeyManager& GetInstance();

    void LeftKeyPressed();
    void RightKeyPressed();
    void DownKeyPressed();
    void QKeyPressed();
    void EKeyPressed();

    bool IsLeftKeyPressed();
    bool IsRightKeyPressed();
    bool IsDownKeyPressed();
    bool IsQKeyPressed();
    bool IsEKeyPressed();

    void Reset();
};