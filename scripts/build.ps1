# Builds duGL on Windows.
#
# What the build does lives in CMakePresets.json, which is what keeps it reproducible
# elsewhere; this script only adds the one thing Windows cannot supply on its own. MSVC needs
# its developer environment (INCLUDE, LIB, PATH) before cl.exe can find the standard and
# Windows SDK headers. Visual Studio applies that environment itself, so this is only needed
# when building from an ordinary shell.
#
# On Linux and macOS no such setup exists and the presets are used directly:
#
#     cmake --preset x64-Debug
#     cmake --build --preset x64-Debug

[CmdletBinding()]
param(
    # the preset to configure and build, as named in CMakePresets.json
    [string] $Preset = 'x64-Debug',

    # a single CMake target to build, instead of everything the preset builds
    [string] $Target
)

$ErrorActionPreference = 'Stop'

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot '..')

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw "vswhere.exe not found at '$vswhere'. Is Visual Studio installed?"
}

$installPath = & $vswhere -latest -prerelease -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath
if (-not $installPath) {
    throw 'No Visual Studio installation with the MSVC x64 toolset was found.'
}

$vcvars = Join-Path $installPath 'VC\Auxiliary\Build\vcvars64.bat'
if (-not (Test-Path $vcvars)) {
    throw "vcvars64.bat not found at '$vcvars'."
}

# Ninja re-runs CMake by itself when the build files go stale, so configuring is only needed
# when the tree does not exist yet.
$configure = ''
if (-not (Test-Path (Join-Path $repoRoot "out\build\$Preset\CMakeCache.txt"))) {
    $configure = "cmake --preset $Preset && "
}

$build = "cmake --build --preset $Preset"
if ($Target) {
    $build += " --target $Target"
}

# The compiler writes diagnostics to stderr, and PowerShell would otherwise promote those to
# terminating errors under the 'Stop' preference set above. The exit code is what decides
# whether the build succeeded.
$ErrorActionPreference = 'Continue'

# vcvars64.bat only sets variables in the cmd.exe process that runs it, so the build has to
# happen inside that same process. Its own output is discarded: it is noisy, and it reports a
# missing vswhere.exe on PATH that does not affect the environment it sets up.
cmd /c "`"$vcvars`" >nul 2>&1 && cd /d `"$repoRoot`" && $configure$build"
exit $LASTEXITCODE
