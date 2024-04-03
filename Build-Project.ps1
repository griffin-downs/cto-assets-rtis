# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


[CmdletBinding()]
param(
    [string]$BuildDirectory = 'build',

    [switch]$Clean,

    [ValidateSet('Native', 'WebAssembly')]
    [string]$Target = 'WebAssembly',

    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$CMakeBuildType = 'MinSizeRel'
)

$ErrorActionPreference = 'Stop'

$emscriptenToolchain = `
    Join-Path `
        (Get-Item .).FullName `
        'emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake'

function Restore-Emscripten {
    if ($Target -ne 'WebAssembly') {
        return
    }

    $emscriptenToolchainExists = Test-Path $emscriptenToolchain -PathType Leaf
    if (-not $emscriptenToolchainExists) {
        if (Test-Path 'emsdk' -PathType Container) {
            Remove-Item -Path 'emsdk' -Recurse -Force
        }
        git submodule update --init 'emsdk'
        emsdk/emsdk install latest
    }

    if (-not (Get-Item 'Env:EMSDK' -ErrorAction SilentlyContinue)) {
        emsdk/emsdk.ps1 activate latest
    }
}
Restore-Emscripten

function Restore-Vcpkg {
    $vcpkgExists = Test-Path 'vcpkg/vcpkg.exe' -PathType Leaf
    if ($vcpkgExists) {
        return
    }

    if (Test-Path 'vcpkg' -PathType Container) {
        Remove-Item -Path 'vcpkg' -Recurse -Force
    }

    git submodule update --init 'vcpkg'
    vcpkg/bootstrap-vcpkg.bat
}
Restore-Vcpkg

function Restore-PortDependencies {
    function Get-PortDependencies {
        $portDependencies = `
            Get-Content 'PortDependencies.json' | ConvertFrom-Json

        $defaultTriplet = `
            if ($IsWindows) { 'x64-windows' }
            elseif ($IsLinux) { 'x64-linux' }
            elseif ($IsMacOs) { 'x64-osx' }
            else { throw "Unknown platform" }

        $portDependencies.Tools | ForEach-Object {
            "$($_):$defaultTriplet"
        }

        if ($Target -eq 'WebAssembly') {
            $portDependencies.CTOAssetsRTIS.WebAssembly | ForEach-Object {
                "$($_):wasm32-emscripten"
            }
            return
        }

        $portDependencies.CTOAssetsRTIS.Native | ForEach-Object {
            "$($_):$defaultTriplet"
        }
    }
    Get-PortDependencies | ForEach-Object {
        $output = vcpkg/vcpkg.exe list $_ 2> $null
        if ($output -and $output -match $_) {
            return
        }

        vcpkg/vcpkg.exe install $_
    }
}
Restore-PortDependencies

function Get-BuildDirectoryPaths {
    $relativeBuildDirectoryPaths = @(
        'tools',
        'cto-assets-rtis'
    )

    $fullPaths = @{}
    $relativeBuildDirectoryPaths | ForEach-Object {
        $fullPath = Join-Path $BuildDirectory $_
        $fullPaths[$_] = $fullPath
    }

    $fullPaths
}
$buildPaths = Get-BuildDirectoryPaths

function Restore-ToolsAndApplication {
    if ($Clean) {
        Remove-Item `
            -Path $BuildDirectory `
            -Recurse -Force -ErrorAction SilentlyContinue
    }

    $assertExitCode = {
        if ($?) {
            return
        }
        exit $LastExitCode
    }

    $currentDirectory = (Get-Item .).FullName

    function Get-CommonCMakeConfigureArguments {
        $vcpkgToolchain = `
            Join-Path $currentDirectory 'vcpkg/scripts/buildsystems/vcpkg.cmake'

        $commonIncludeDirectory = Join-Path $currentDirectory 'common/include'

        $toolsBinaryDirectory = `
            Join-Path $currentDirectory $buildPaths['tools'] 'bin'

        @(
            '-G Ninja',
            '-DCMAKE_CXX_COMPILER=clang++',
            "-DCMAKE_BUILD_TYPE=$CMakeBuildType",
            "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain",
            "-DCOMMON_INCLUDE_DIRECTORY=${commonIncludeDirectory}"
            "-DTOOLS_BINARY_DIRECTORY=${toolsBinaryDirectory}"
        )
    }
    $commonCMakeConfigureArguments = Get-CommonCMakeConfigureArguments

    function Restore-Tools {
        $buildPath = $buildPaths['tools']

        $commonToolsIncludeDirectory = `
            Join-Path $currentDirectory 'common/tools/include'

        $cmakeConfigureArguments = @(
            "-DCOMMON_TOOLS_INCLUDE_DIRECTORY=${commonToolsIncludeDirectory}"
            "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${toolsBinaryDirectory}"
            "-B $buildPath",
            '-S tools'
        ) + $commonCMakeConfigureArguments

        if (-not (Test-Path $buildPath -PathType Container)) {
            cmake @cmakeConfigureArguments
            & $assertExitCode
        }

        cmake --build $buildPath --verbose
        & $assertExitCode
    }
    Restore-Tools

    function Restore-Application {
        $buildPath = $buildPaths['cto-assets-rtis']

        $cmakeConfigureArguments = @(
            "-B $buildPath",
            "-S ."
        ) + $commonCMakeConfigureArguments

        function Invoke-CMakeConfigure {
            if ($Target -eq 'WebAssembly') {
                emcmake cmake `
                    @cmakeConfigureArguments `
                    -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$emscriptenToolchain"
                & $assertExitCode

                return
            }

            cmake @cmakeConfigureArguments
            & $assertExitCode
        }
        if (-not (Test-Path $buildPath -PathType Container)) {
            Invoke-CMakeConfigure
        }

        cmake --build $buildPath --verbose
        & $assertExitCode
    }
    Restore-Application
}
Restore-ToolsAndApplication
