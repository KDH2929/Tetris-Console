#include "KeyManager.h"

bool KeyManager::leftKeyPressed = false;
bool KeyManager::rightKeyPressed = false;
bool KeyManager::qKeyPressed = false;
bool KeyManager::eKeyPressed = false;
bool KeyManager::downKeyPressed = false;

KeyManager& KeyManager::GetInstance() {
    static KeyManager instance;
    return instance;
}

void KeyManager::LeftKeyPressed()
{
    leftKeyPressed = true;
}

void KeyManager::RightKeyPressed()
{
    rightKeyPressed = true;
}

void KeyManager::DownKeyPressed()
{
    downKeyPressed = true;
}

void KeyManager::QKeyPressed()
{
    qKeyPressed = true;
}

void KeyManager::EKeyPressed()
{
    eKeyPressed = true;
}

bool KeyManager::IsLeftKeyPressed()
{
    return leftKeyPressed;
}

bool KeyManager::IsRightKeyPressed()
{
    return rightKeyPressed;
}

bool KeyManager::IsDownKeyPressed()
{
    return downKeyPressed;
}

bool KeyManager::IsQKeyPressed()
{
    return qKeyPressed;
}

bool KeyManager::IsEKeyPressed()
{
    return eKeyPressed;
}

void KeyManager::Reset() {
    leftKeyPressed = false;
    rightKeyPressed = false;
    downKeyPressed = false;
    qKeyPressed = false;
    eKeyPressed = false;
}
