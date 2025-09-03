// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


namespace ctoAssetsRTIS
{
struct KeyStates
{
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;
    bool qPressed = false;
    bool ePressed = false;

    void setKeyState(int key, bool pressed)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                wPressed = pressed;
                break;
            case GLFW_KEY_A:
                aPressed = pressed;
                break;
            case GLFW_KEY_S:
                sPressed = pressed;
                break;
            case GLFW_KEY_D:
                dPressed = pressed;
                break;
            case GLFW_KEY_Q:
                qPressed = pressed;
                break;
            case GLFW_KEY_E:
                ePressed = pressed;
                break;
            default:
                break;
        }
    }
};

struct MouseStates
{
    glm::vec2 lastCursorPosition = glm::vec2();
    glm::vec2 currentCursorPosition = glm::vec2();
    bool buttonPressed = false;
    bool touchActive = false;
    bool firstMove = true;

    static MouseStates& fromPointer(void* pointer)
    {
        auto mouseStatesPointer = static_cast<MouseStates*>(pointer);
        if (!mouseStatesPointer)
        {
            throw std::runtime_error("Failed to retrieve MouseStates");
        }

        return *mouseStatesPointer;
    }
};

struct InputStates
{
    MouseStates mouseStates;
    KeyStates keyStates;
};
} // namespace ctoAssetsRTIS
