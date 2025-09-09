// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <memory>

#include <GLFW/glfw3.h>

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
            this->window.get(),
            EM_FALSE,
            touchMoveCallback);

        emscripten_set_touchend_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            this->window.get(),
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
#else
    bool isExitRequested() const
    {
        return false;
    }
#endif

    void startFrame()
    {
        this->states
            .mouseStates
            .startFrame();

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
        auto& [
            projectionMatrixManager,
            inputSystem
        ] = ApplicationStateManager::getStateFromWindow(userData);
            
        auto& mouseStates =
            inputSystem
                .getStates()
                .mouseStates;

        if (event->numTouches <= 0)
        {
            mouseStates.endContact();
            return EM_TRUE;
        }

        if (!mouseStates.isContactActive())
        {
            mouseStates.beginContact();
        }

        const auto clientX = event->touches[0].clientX;
        const auto clientY = event->touches[0].clientY;
        const auto viewportDimensions =
            projectionMatrixManager.getViewportDimensions();

        mouseStates.updateDevicePosition(
            clientX,
            clientY,
            viewportDimensions);

        return EM_TRUE;
    }

    static int touchEndCallback(
        int /* eventType */,
        const EmscriptenTouchEvent* /* event */,
        void* userData)
    {
        auto& [
            _,
            inputSystem
        ] = ApplicationStateManager::getStateFromWindow(userData);

        inputSystem
            .getStates()
            .mouseStates
            .endContact();

        return EM_TRUE;
    }
#endif

    static void cursorPositionCallback(
        GLFWwindow* window,
        double xPos,
        double yPos)
    {
        auto& [
            projectionMatrixManager,
            inputSystem
        ] = ApplicationStateManager::getStateFromWindow(window);

        auto& mouseStates =
            inputSystem
                .getStates()
                .mouseStates;

        const auto viewportDimensions =
            projectionMatrixManager.getViewportDimensions();

        mouseStates.updateDevicePosition(
            xPos,
            yPos,
            viewportDimensions);
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

        if (button != GLFW_MOUSE_BUTTON_LEFT)
        {
            return;
        }

        if (action == GLFW_PRESS)
        {
            mouseStates.beginContact();
            return;
        }

        if (action == GLFW_RELEASE)
        {
            mouseStates.endContact();
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

        bool keyPressed = action == GLFW_PRESS || action == GLFW_REPEAT;
        ApplicationStateManager
            ::getStateFromWindow(window)
            .inputSystem
            .getStates()
            .keyStates
            .setKeyState(key, keyPressed);
    }
};
} // namespace ctoAssetsRTIS
