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

$vcpkgExists = Test-Path 'vcpkg/vcpkg.exe' -PathType Leaf
if (-not $vcpkgExists) {
    if (Test-Path 'vcpkg' -PathType Container) {
        Remove-Item -Path 'vcpkg' -Recurse -Force | Out-Null
    }
    git submodule update --init 'vcpkg'
    vcpkg/bootstrap-vcpkg.bat
}

$emscriptenToolchain = `
    Join-Path `
        (Get-Item .).FullName `
        'emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake'

if ($Target -eq 'WebAssembly') {
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

$portDependencies = Get-Content 'PortDependencies.json' | ConvertFrom-Json
$portDependencies = `
    if ($Target -eq 'WebAssembly') {
        $portDependencies.WebAssembly | ForEach-Object {
            "$($_):wasm32-emscripten"
        }
    } else {
        $defaultTriplet = `
            if ($IsWindows) { 'x64-windows' }
            elseif ($IsLinux) { 'x64-linux' }
            elseif ($IsMacOs) { 'x64-osx' }
            else { throw "Unknown platform" }

        $portDependencies.Native | ForEach-Object {
            "$($_):$defaultTriplet"
        }
    }

foreach ($port in $portDependencies) {
    $output = vcpkg/vcpkg.exe list $port 2> $null

    if (-not $output -or $output -notmatch $port) {
        vcpkg/vcpkg.exe install $port
    }
}

function Invoke-CMakeConfigure {
    $vcpkgToolchain = `
        Join-Path `
            (Get-Item .).FullName `
            'vcpkg/scripts/buildsystems/vcpkg.cmake'

    $commonCMakeArguments = @(
        '-G Ninja',
        '-DCMAKE_CXX_COMPILER=clang++',
        "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain",
        "-DCMAKE_BUILD_TYPE=$CMakeBuildType",
        "-B $($BuildDirectory)",
        '-S .'
    )

    if ($Target -eq 'WebAssembly') {
        emcmake cmake `
            @commonCMakeArguments `
            -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$emscriptenToolchain"
    } else {
        cmake @commonCMakeArguments
    }
}

$createBuildDirectory = {
    New-Item -ItemType Directory -Path $BuildDirectory -Force | Out-Null
}

$buildDirectoryExists = Test-Path $BuildDirectory -PathType Container
if ($Clean) {
    if ($buildDirectoryExists) {
        Remove-Item -Path $BuildDirectory -Recurse -Force | Out-Null
    }
    & $createBuildDirectory
    Invoke-CMakeConfigure
} elseif (-not $buildDirectoryExists) {
    & $createBuildDirectory
    Invoke-CMakeConfigure
}

cmake --build $BuildDirectory --verbose
