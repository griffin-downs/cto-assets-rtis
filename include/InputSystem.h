// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <memory>

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "InputStates.h"

namespace ctoAssetsRTIS
{
class InputSystem
{
public:
    struct InputSystemArguments
    {
        std::shared_ptr<GLFWwindow> window;
    };
    InputSystem(InputSystemArguments arguments)
    : window(arguments.window)
    {
        glfwSetKeyCallback(this->window.get(), keyCallback);
        glfwSetCursorPosCallback(this->window.get(), cursorPositionCallback);
        glfwSetMouseButtonCallback(this->window.get(), mouseButtonCallback);

#ifdef __EMSCRIPTEN__
        emscripten_set_touchmove_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            &this->states.mouseStates,
            EM_FALSE,
            touchMoveCallback);

        emscripten_set_touchend_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            &this->states.mouseStates,
            EM_FALSE,
            touchEndCallback);
#endif
    }

    InputSystem(const InputSystem&) = delete;
    InputSystem(InputSystem&&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

#ifndef __EMSCRIPTEN__
    bool isExitRequested() const
    {
        return glfwWindowShouldClose(this->window.get());
    }
#endif

    void pollEvents() const
    {
        glfwPollEvents();
    }

    InputStates& getStates()
    {
        return this->states;
    }

private:
    const std::shared_ptr<GLFWwindow> window;
    InputStates states;

#ifdef __EMSCRIPTEN__
    static int touchMoveCallback(
        int /* eventType */,
        const EmscriptenTouchEvent* event,
        void* userData)
    {
        auto& mouseStates = MouseStates::fromPointer(userData);
        if (event->numTouches <= 0)
        {
            mouseStates.touchActive = false;
            return EM_TRUE;
        }

        mouseStates.touchActive = true;

        const auto clientX =  event->touches[0].clientX;
        const auto clientY =  event->touches[0].clientY;

        if (mouseStates.firstMove)
        {
            mouseStates.lastCursorPosition.x = clientX;
            mouseStates.lastCursorPosition.y = clientY;
            mouseStates.firstMove = false;
            return EM_TRUE;
        }

        mouseStates.lastCursorPosition = mouseStates.currentCursorPosition;

        mouseStates.currentCursorPosition.x = clientX;
        mouseStates.currentCursorPosition.y = clientY;

        return EM_TRUE;
    }

    static int touchEndCallback(
        int /* eventType */,
        const EmscriptenTouchEvent* /* event */,
        void* userData)
    {
        MouseStates::fromPointer(userData).touchActive = false;
        return EM_TRUE;
    }
#endif

    static void cursorPositionCallback(
        GLFWwindow* window,
        double xPos,
        double yPos)
    {
        auto& mouseStates =
            ApplicationStateManager
                ::getStateFromWindow(window)
                .inputSystem
                .getStates()
                .mouseStates;

        if (mouseStates.touchActive)
        {
            return;
        }

        if (mouseStates.firstMove)
        {
            mouseStates.lastCursorPosition.x = xPos;
            mouseStates.lastCursorPosition.y = yPos;
            mouseStates.firstMove = false;
            return;
        }

        mouseStates.lastCursorPosition = mouseStates.currentCursorPosition;

        mouseStates.currentCursorPosition.x = xPos;
        mouseStates.currentCursorPosition.y = yPos;
    }

    static void mouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int /* mods */)
    {
        auto& mouseStates =
            ApplicationStateManager
                ::getStateFromWindow(window)
                .inputSystem
                .getStates()
                .mouseStates;

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            mouseStates.touchActive = false;
            mouseStates.buttonPressed = false;
        }

        if (mouseStates.touchActive)
        {
            return;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            mouseStates.buttonPressed = true;
        }
    }

    static void keyCallback(
        GLFWwindow* window,
        int key,
        int /* scancode */,
        int action,
        int /* mods */)
    {
#ifndef __EMSCRIPTEN__
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
#endif

        bool keyDown = action == GLFW_PRESS || action == GLFW_REPEAT;
        ApplicationStateManager
            ::getStateFromWindow(window)
            .inputSystem
            .getStates()
            .keyStates
            .setKeyState(key, keyDown);
    }
};
} // namespace ctoAssetsRTIS
