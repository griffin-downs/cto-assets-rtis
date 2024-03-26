// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace ctoAssetsRTIS
{
class Shader
{
public:
    struct SourcePaths
    {
        const char* vertex;
        const char* fragment;
    };
    Shader(SourcePaths shaderSourcePaths)
    : shaderProgramId(
    [&]()
    {
#ifdef DEBUG
        auto logBuffer = std::array<char, 512>{};
#endif
        int success = -1;

        struct SourceInfo
        {
            const char* code;
            GLenum shaderType;
        };
        const auto compileShaderCompilationUnit =
        [&](SourceInfo sourceInfo)
        {

            const auto [code, shaderType] = sourceInfo;
            const auto shaderCompilationUnitID =
                glCreateShader(shaderType);

            glShaderSource(
                shaderCompilationUnitID,
                1,
                &code,
                nullptr);

            glCompileShader(shaderCompilationUnitID);

            const auto checkCompilation =
            [&]()
            {
                const auto isCompilationSuccessful =
                [&]
                {
                    glGetShaderiv(
                        shaderCompilationUnitID,
                        GL_COMPILE_STATUS,
                        &success);
                    return static_cast<bool>(success) == true;
                };

                if (isCompilationSuccessful())
                {
                    return;
                }

#ifdef DEBUG
                glGetShaderInfoLog(
                    shaderCompilationUnitID,
                    logBuffer.size(),
                    nullptr,
                    logBuffer.data());

                throw std::runtime_error(
                    std::format(
                        "Shader compiler error.\nType: {}\nLog:\n{}",
                        shaderType,
                        logBuffer.data()));
#endif
            };
            checkCompilation();

            return shaderCompilationUnitID;
        };

        struct ShaderCompilationUnitIds
        {
            GLuint vertex, fragment;
        };
        const auto compileShaderProgram =
        [&](ShaderCompilationUnitIds shaderCompilationIds)
        {
            const auto shaderProgramId = glCreateProgram();

            glAttachShader(
                shaderProgramId,
                shaderCompilationIds.vertex);
            glAttachShader(
                shaderProgramId,
                shaderCompilationIds.fragment);

            glLinkProgram(shaderProgramId);

            const auto checkCompilation =
            [&]
            {
                const auto isCompilationSuccessful =
                [&]
                {
                    glGetProgramiv(
                        shaderProgramId,
                        GL_LINK_STATUS,
                        &success);
                    return static_cast<bool>(success) == true;
                };
                if (isCompilationSuccessful())
                {
                    return;
                }

#ifdef DEBUG
                glGetProgramInfoLog(
                    shaderProgramId,
                    logbuffer.size(),
                    nullptr,
                    logBuffer.data());

                throw std::runtime_error(
                    std::format(
                        "Shader compiler error.\nLog:\n{}"),
                        logBuffer.data());
#endif
            };
            checkCompilation();

            const auto cleanUpResources =
            [&]
            {
                glDetachShader(
                    shaderProgramId,
                    shaderCompilationIds.vertex);
                glDetachShader(
                    shaderProgramId,
                    shaderCompilationIds.fragment);

                glDeleteShader(shaderCompilationIds.vertex);
                glDeleteShader(shaderCompilationIds.fragment);
            };
            cleanUpResources();

            return shaderProgramId;
        };

        return
            compileShaderProgram(
                ShaderCompilationUnitIds
                {
                    .vertex =
                        compileShaderCompilationUnit({
                            .code = shaderSourcePaths.vertex,
                            .shaderType = GL_VERTEX_SHADER
                        }),
                    .fragment =
                        compileShaderCompilationUnit({
                            .code = shaderSourcePaths.fragment,
                            .shaderType = GL_FRAGMENT_SHADER
                        })
                });
    }())
    {
    }

    void use() const
    {
        glUseProgram(this->shaderProgramId);
    }

    auto getId() const { return this->shaderProgramId; };

    auto getUniform(const char* uniformName) const
    {
        return glGetUniformLocation(this->shaderProgramId, uniformName);
    }

    void set(const char* uniformName, const glm::mat4& value) const
    {
        const auto count = 1;
        const auto transpose = GL_FALSE;

        glUniformMatrix4fv(
            getUniform(uniformName),
            count,
            transpose,
            &value[0][0]);
    }

    void set(const char* uniformName, const glm::vec4& value) const
    {
        const auto count = 1;

        glUniform4fv(
            getUniform(uniformName),
            count,
            &value[0]);
    }

    void set(const char* uniformName, float value) const
    {
        constexpr auto count = 1;
        glUniform1fv(getUniform(uniformName), count, &value);
    }

private:
    const GLuint shaderProgramId;
};
} // namespace ctoAssetsRTIS
