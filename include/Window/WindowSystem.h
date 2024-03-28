// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <iostream>
#include <memory>
#include <span>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "Application/ApplicationStateManager.h"
#include "Graphics/Rendering/ProjectionMatrixManager.h"
#include "ViewportDimensions.h"


namespace ctoAssetsRTIS
{
#ifdef DEBUG
void GLAPIENTRY MessageCallback(
    GLenum,
    GLenum,
    GLuint,
    GLenum,
    GLsizei,
    const GLchar*,
    const void*);
#endif

#ifdef __EMSCRIPTEN__
static EM_BOOL onCanvasSizeChanged(int, const EmscriptenUiEvent*, void*);
#endif

class WindowSystem
{
public:
    struct Properties
    {
        const char* title;
        ViewportDimensions viewportDimensions;
    };
    WindowSystem(Properties properties)
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 16);

        const auto [width, height] = properties.viewportDimensions;

        this->window =
            std::shared_ptr<GLFWwindow>(
                glfwCreateWindow(
                    width,
                    height,
                    properties.title,
                    nullptr,
                    nullptr),
                [](auto* w) { glfwDestroyWindow(w); });

        if (!this->window)
        {
            glfwTerminate();
            std::cerr << "Failed to create GLFW window" << std::endl;
            std::exit(EXIT_FAILURE);
        }

#ifdef __EMSCRIPTEN__
        emscripten_set_resize_callback(
            EMSCRIPTEN_EVENT_TARGET_WINDOW,
            glfwGetCurrentContext(),
            false,
            onCanvasSizeChanged);
#endif

        glfwSetFramebufferSizeCallback(
            this->window.get(),
            []([[maybe_unused]] auto* window, int width, int height)
            {
                ApplicationStateManager
                    ::getStateFromWindow(window)
                    .projectionMatrixManager
                    .setViewportDimensions({{
                        .width = width,
                        .height = height
                    }});

                glViewport(0, 0, width, height);
            });

        glfwMakeContextCurrent(this->window.get());

#ifndef __EMSCRIPTEN__
        if (glewInit() != GLEW_OK)
        {
            glfwTerminate();
            std::cerr << "Failed to initialize GLEW" << std::endl;
            std::exit(EXIT_FAILURE);
        }
#endif

#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
#endif

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
    }

    ~WindowSystem()
    {
        glfwTerminate();
    }

    std::shared_ptr<GLFWwindow> getWindow() const
    {
        return window;
    }

    void clearScreen() const
    {
        glClearColor(0.08f, 0.467f, 0.741f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void swapBuffers() const
    {
        glfwSwapBuffers(this->window.get());
    }

private:
    std::shared_ptr<GLFWwindow> window;
};

#ifdef DEBUG
void GLAPIENTRY MessageCallback(
    GLenum /* source */,
    GLenum type,
    GLuint /* id */,
    GLenum severity,
    GLsizei /* length */,
    const GLchar* message,
    const void* /* userParam */)
{
    fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type,
        severity,
        message);
}
#endif

#ifdef __EMSCRIPTEN__
static EM_BOOL onCanvasSizeChanged(
    int /* event_type */,
    const EmscriptenUiEvent* /* ui_event */,
    void* /* user_data */)
{
    auto [width, height] = ViewportDimensions::fromCanvasSize();
    glfwSetWindowSize(glfwGetCurrentContext(), width, height);
    return true;
}
#endif
} // namespace ctoAssetsRTIS
