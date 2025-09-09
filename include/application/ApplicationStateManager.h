// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <functional>

#include <GLFW/glfw3.h>


namespace ctoAssetsRTIS
{
class ProjectionMatrixManager;
class InputSystem;

class ApplicationStateManager
{
public:
    struct ApplicationState
    {
        ProjectionMatrixManager& projectionMatrixManager;
        InputSystem& inputSystem;
    };
    ApplicationStateManager(ApplicationState state)
    : state{ state }
    {
    }

    struct WindowProvider
    {
        GLFWwindow* window;
    };
    void setWindowUserDataToHeldState(WindowProvider windowProvider)
    {
        glfwSetWindowUserPointer(windowProvider.window, &this->state);
    }

    static ApplicationState& getStateFromWindow(GLFWwindow* window)
    {
        auto applicationStatePointer =
            static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));

        if (!applicationStatePointer)
        {
            throw std::runtime_error("Failed to retrieve ApplicationState");
        }

        return *applicationStatePointer;
    }

    static ApplicationState& getStateFromWindow(void* window)
    {
        auto windowPointer = static_cast<GLFWwindow*>(window);

        if (!windowPointer)
        {
            throw std::runtime_error("Failed to convert void* to GLFWwindow");
        }


        return getStateFromWindow(windowPointer);
    }

private:
    ApplicationState state;
};
} // namespace ctoAssetsRTIS