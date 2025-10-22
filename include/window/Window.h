// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================

#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "ViewportDimensions.h"
#include "graphics/rendering/ProjectionMatrixManager.h"
#include "input/InputStates.h"


namespace ctoAssetsRTIS
{
class Window
{
public:
    Window(ProjectionMatrixManager& projectionMatrixManager)
    : projectionMatrixManager{ projectionMatrixManager }
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW.");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 16);

        const auto [width, height] = ViewportDimensions::getDefault();

        this->glfwWindow =
            decltype(this->glfwWindow)(
                glfwCreateWindow(
                    width,
                    height,
                    "cto-assets-rtis",
                    nullptr,
                    nullptr));

        if (!this->glfwWindow)
        {
            throw std::runtime_error("Failed to create glfwWindow.");
        }

        const auto rawWindow = this->glfwWindow.get();
        glfwMakeContextCurrent(rawWindow);
        glfwSetWindowUserPointer(rawWindow, this);

        glfwSetFramebufferSizeCallback(rawWindow, framebufferSizeCallback);
        glfwSetCursorPosCallback(rawWindow, cursorPosCallback);
        glfwSetMouseButtonCallback(rawWindow, mouseButtonCallback);
        glfwSetKeyCallback(rawWindow, keyCallback);

#ifdef __EMSCRIPTEN__
        emscripten_set_resize_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            rawWindow,
            false,
            emscriptenResizeCallback);

        emscripten_set_touchmove_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            rawWindow,
            EM_FALSE,
            touchMoveCallback);

        emscripten_set_touchend_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            rawWindow,
            EM_FALSE,
            touchEndCallback);
#else
        if (glewInit() != GLEW_OK)
        {
            throw std::runtime_error("Failed to initialize GLEW.");
        }
#endif
    }

    ~Window()
    {
        this->glfwWindow.reset();
        glfwTerminate();
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    struct WindowDeleter
    {
        void operator()(GLFWwindow* glfwWindow) const
        {
            if (!glfwWindow)
            {
                return;
            }

            glfwDestroyWindow(glfwWindow);
        }
    };

    const auto& getInputStates()
    {
        return this->inputStates;
    }

    auto isExitRequested() const
    {
        return glfwWindowShouldClose(this->glfwWindow.get());
    }

    void startFrame()
    {
        this->inputStates
            .mouseStates
            .startFrame();

        glfwPollEvents();
    }

    void swapBuffers() const
    {
        glfwSwapBuffers(this->glfwWindow.get());
    }

private:
    std::unique_ptr<GLFWwindow, WindowDeleter> glfwWindow;
    ProjectionMatrixManager& projectionMatrixManager;
    InputStates inputStates;

    static auto& selfFromGlfwWindow(GLFWwindow* glfwWindow)
    {
        auto window =
            static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));

        if (!window)
        {
            throw std::runtime_error("Failed to retrieve Window");
        }

        return *window;
    }

    static auto& selfFromGlfwWindow(void* untypedGlfwWindow)
    {
        auto glfwWindow = static_cast<GLFWwindow*>(untypedGlfwWindow);

        if (!glfwWindow)
        {
            throw std::runtime_error("Failed to convert void* to GLFWwindow*");
        }

        return selfFromGlfwWindow(glfwWindow);
    }

    static void framebufferSizeCallback(
        GLFWwindow* glfwWindow,
        int width,
        int height)
    {
        auto& self = selfFromGlfwWindow(glfwWindow);

        self.projectionMatrixManager
            .setViewportDimensions({{
                .width  = width,
                .height = height
            }});

        glViewport(0, 0, width, height);
    }

    static void cursorPosCallback(
        GLFWwindow* glfwWindow,
        double xPosition,
        double yPosition)
    {
        auto& self = selfFromGlfwWindow(glfwWindow);

        const auto viewport =
            self.projectionMatrixManager
                .getViewportDimensions();

        self.inputStates
            .mouseStates
            .updateDevicePosition(
                xPosition,
                yPosition,
                viewport);
    }

    static void mouseButtonCallback(
        GLFWwindow* glfwWindow,
        int button,
        int action,
        int /*mods*/)
    {
        auto& self = selfFromGlfwWindow(glfwWindow);
        auto& mouseStates = self.inputStates.mouseStates;

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
        GLFWwindow* glfwWindow,
        int key,
        int /*scancode*/,
        int action,
        int /*mods*/)
    {
        auto& self = selfFromGlfwWindow(glfwWindow);

#ifndef __EMSCRIPTEN__
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }
#endif

        const auto pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
        self.inputStates.keyStates.setKeyState(key, pressed);
    }

#ifdef __EMSCRIPTEN__
    static EM_BOOL emscriptenResizeCallback(
        int /* eventType */,
        const EmscriptenUiEvent* /* uiEvent */,
        void* /* user */)
    {
        const auto [width, height] = ViewportDimensions::fromCanvasSize();

        glfwSetWindowSize(
            glfwGetCurrentContext(),
            width,
            height);

        return EM_TRUE;
    }

    static int touchMoveCallback(
        int /* eventType */,
        const EmscriptenTouchEvent* event,
        void* userData)
    {
        auto& self = selfFromGlfwWindow(userData);
        auto& mouseStates = self.inputStates.mouseStates;

        const auto& projectionMatrixManager = self.projectionMatrixManager;

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

        std::cout << "X: " << clientX << std::endl;
        std::cout << "Y: " << clientY << std::endl;

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
        selfFromGlfwWindow(userData)
            .inputStates
            .mouseStates
            .endContact();

        return EM_TRUE;
    }
#endif
};
} // namespace ctoAssetsRTIS