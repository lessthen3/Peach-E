##########################################################################
#                        Peach-E v0.0.1
#         Created by Ranyodh Singh Mandur - 🍑 2024-2026
#
#              Licensed under the MIT License (MIT).
#         For more details, see the LICENSE file or visit:
#               https://opensource.org/licenses/MIT
#
#           Peach-E is a free open source game engine
##########################################################################
import subprocess
import os
import argparse
import platform
import shutil
import sys
import zipfile
import re
import glob 

from shutil import which
from enum import Enum

############# Pretty Text Utility Function UwU #############

def CreateColouredText(fp_SampleText: str, fp_DesiredColour: str) -> str:

    fp_DesiredColour = fp_DesiredColour.lower()

    f_ListOfColours = {
        "black": '\033[30m', "red": '\033[31m', "green": '\033[32m',
        "yellow": '\033[33m', "blue": '\033[34m', "magenta": '\033[35m',
        "cyan": '\033[36m', "white": '\033[37m',

        "bright black": '\033[90m', "bright red": '\033[91m', "bright green": '\033[92m',
        "bright yellow": '\033[93m', "bright blue": '\033[94m', "bright magenta": '\033[95m',
        "bright cyan": '\033[96m', "bright white": '\033[97m'
    }

    if fp_DesiredColour not in f_ListOfColours:
        log_warning("no valid input detected for CreateColouredText, returned original text in all lower-case")
        return fp_SampleText
    
    else:
        return f"{f_ListOfColours.get(fp_DesiredColour, '')}{fp_SampleText}\033[0m"

############################################################################## Logging functions OwO ##############################################################################

def log_info(fp_Message: str) -> None:
    print(CreateColouredText(f"[INFO]: {fp_Message}", "bright green"))

def log_warning(fp_Message: str) -> None:
    print(CreateColouredText(f"[WARNING]: {fp_Message}", "yellow"))

def log_error(fp_Message: str) -> None:
    print(CreateColouredText(f"[ERROR]: {fp_Message}", "red"))

def log_success(fp_Message: str) -> None:
    print(CreateColouredText(f"[SUCCESS]: {fp_Message}", "cyan"))

def print_tip(fp_Message: str) -> None:
    print(CreateColouredText(f"[TIP]: {fp_Message}", "bright cyan"))

############# Utility for Validating Required Build Tools #############

def ensure_tool_installed(fp_ToolName: str) -> bool:

    if which(fp_ToolName) is None:
        log_error(f"Required tool '{fp_ToolName}' not found in PATH")
        return False
    else:
        return True

############# Build Session Accumulators #############

# populated during the build loop, dumped at the end if flags are set
g_ErrorLog:   dict[str, list[str]] = {}  # dep_name -> [error lines]
g_WarningLog: dict[str, list[str]] = {}  # dep_name -> [warning lines]
g_CurrentDep: str = "Peach-E"            # this is more for build_deps.py but w/e

############# Compiled Regex Patterns for Build Output Classification #############

_g_ErrorPatterns = [
    re.compile(r':\s*error\b',              re.IGNORECASE), # "error:" / ": error" — GCC, Clang, MSVC
    re.compile(r'\bfatal\s+error\b',        re.IGNORECASE), # "fatal error:" — preprocessor, linker
    re.compile(r'\bfailed:\b',              re.IGNORECASE),  # ninja "FAILED: CMakeFiles/..." / MSBuild "Build FAILED, need the : so func tests dont trigger it ;w;"

    re.compile(r'\blnk[12]\d{3}\b',         re.IGNORECASE), # MSVC linker errors: LNK1181, LNK2019 etc
    re.compile(r'\b[Cc][2356789]\d{3}\b',   re.IGNORECASE), # MSVC compiler errors: C2065, C3861 
    re.compile(r'\bld:\s+error\b',          re.IGNORECASE), # GNU ld errors
    re.compile(r'\bundefined\s+symbol\b',   re.IGNORECASE), # linker: undefined symbol
    re.compile(r'\bduplicate\s+symbol\b',   re.IGNORECASE), # linker: duplicate symbol
    re.compile(r'\bundefined\s+reference\b',re.IGNORECASE), # GCC linker variant
    re.compile(r'\breferenced\s+from\b',    re.IGNORECASE), # Apple ld variant
    
    re.compile(r'\bninja:\s+error\b',       re.IGNORECASE), # "ninja: error:" — only ninja errors, not every ninja line
    re.compile(r'\bcommand\s+failed\b',     re.IGNORECASE), # generic CMake command failure
    re.compile(r'cmake\s+error',            re.IGNORECASE), # CMake configure errors

    re.compile(r'\binternal\s+compiler\s+error\b', re.IGNORECASE), # GCC/Clang ICE
]

_g_WarningPatterns = [
    re.compile(r':\s*warning\b',            re.IGNORECASE), # "warning:" / ": warning" — GCC, Clang, MSVC
    re.compile(r'\b[Cc]4\d{3}\b',           re.IGNORECASE), # MSVC warnings are in the 4000s: C4100, C4244 etc
    re.compile(r'\blnk4\d{3}\b',            re.IGNORECASE), # linker warnings should be 4000s as well
    re.compile(r'\bcmake\s+warning\b',      re.IGNORECASE), # CMake configure warnings
]

############# Run command for live console feed #############

"""
    Runs a subprocess command and streams stdout live.
    Errors  → printed red  in real time, collected into g_ErrorLog
    Warnings → printed yellow in real time, collected into g_WarningLog
    Raises CalledProcessError if the command fails.
"""


def run_command_with_live_output(fp_Command, fp_WorkingDirectory=".") -> None:

    global g_ErrorLog, g_WarningLog, g_CurrentDep

    f_Process = subprocess.Popen(
        fp_Command,
        cwd=fp_WorkingDirectory,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        universal_newlines=True,
        encoding="utf-8",
        errors="replace"
    )

    f_OutputLines = []

    try:
        for line in f_Process.stdout:

            f_Stripped = line.rstrip('\n')

            if any(pat.search(line) for pat in _g_ErrorPatterns):
                sys.stdout.write(CreateColouredText(f_Stripped, "bright red") + '\n')
                if g_CurrentDep:
                    g_ErrorLog.setdefault(g_CurrentDep, []).append(f_Stripped)

            elif any(pat.search(line) for pat in _g_WarningPatterns):
                sys.stdout.write(CreateColouredText(f_Stripped, "yellow") + '\n')
                if g_CurrentDep:
                    g_WarningLog.setdefault(g_CurrentDep, []).append(f_Stripped)

            else:
                sys.stdout.write(line)

            f_OutputLines.append(line)

        f_Process.wait()

        if f_Process.returncode != 0:
            raise subprocess.CalledProcessError(
                f_Process.returncode,
                fp_Command,
                output=''.join(f_OutputLines)
            )

    finally:
        f_Process.stdout.close()

############# Markdown Summary Dump #############

def WriteBuildSummaryMarkdown(fp_BaseDir: str, fp_PrintErrors: bool, fp_PrintWarnings: bool) -> None:

    if fp_PrintErrors and g_ErrorLog:

        f_ErrorPath = os.path.join(fp_BaseDir, "build_errors.md")

        with open(f_ErrorPath, "w", encoding="utf-8") as f_File:
            f_File.write("# Peach-E Build Error Summary\n\n")

            for lv_Dep, lv_Lines in g_ErrorLog.items():
                f_File.write(f"## `{lv_Dep}`\n\n")
                f_File.write("```\n")
                for lv_Line in lv_Lines:
                    f_File.write(lv_Line + "\n")
                f_File.write("```\n\n")

        log_info(f"Error summary written to {f_ErrorPath}")

    if fp_PrintWarnings and g_WarningLog:

        f_WarnPath = os.path.join(fp_BaseDir, "build_warnings.md")

        with open(f_WarnPath, "w", encoding="utf-8") as f_File:
            f_File.write("# Peach-E Build Warning Summary\n\n")

            for lv_Dep, lv_Lines in g_WarningLog.items():
                f_File.write(f"## `{lv_Dep}`\n\n")
                f_File.write("```\n")
                for lv_Line in lv_Lines:
                    f_File.write(lv_Line + "\n")
                f_File.write("```\n\n")

        log_info(f"Warning summary written to {f_WarnPath}")

############# Main CMake Function #############

def run_cmake(fp_BuildType: str, fp_Generator: str, fp_TargetPlatform: str, fp_ExtraBuildArgs: list, fp_ExtraConfigs: list) -> bool:

    f_GeneratorMap = {
        "vs2026": "Visual Studio 18 2026",
        "vs2022": "Visual Studio 17 2022",
        "vs2019": "Visual Studio 16 2019",
        "vs2017": "Visual Studio 15 2017",
        "vs2015": "Visual Studio 14 2015",

        "xcode": "Xcode",

        "ninja": "Ninja", #everything under here is untested so uh goodluck w that uwu
        "ninja-mc": "Ninja Multi-Config",

        "unix": "Unix Makefiles",
        "unix-cb": "CodeBlocks - Unix Makefiles",
        "unix-eclipse": "Eclipse CDT4 - Unix Makefiles",

        "mingw": "MinGW Makefiles",
        "msys": "MSYS Makefiles",
        "nmake": "NMake Makefiles",
        "nmake-jom": "NMake Makefiles JOM"
    }

    ############# Ensure Valid Generator was Selected #############

    if fp_Generator not in f_GeneratorMap:
        log_error("Invalid Generator Selected, use -h to see what generators are available owo")
        return False
    
    ############# Determine if Generator is Single Config #############
    
    f_IsMultiConfig = fp_Generator in ["vs2026", "vs2022", "vs2019", "vs2017", "vs2015", "xcode", "ninja-mc"]

    f_CMakeConfigCommand = ['cmake', '-S', '.', '-B', 'build', '-G', f_GeneratorMap[fp_Generator]]
    
    if not f_IsMultiConfig:

        if fp_BuildType == "Release and Debug": #Don't allow "both" configs for single config generators uwu
            
            log_error("Invalid build type selected: YOU CANNOT USE BOTH WHEN GENERATING FOR A SINGLE CONFIG GENERATOR")
            return False
        
        else:
            f_CMakeConfigCommand += ['-DCMAKE_BUILD_TYPE=' + fp_BuildType.capitalize()]

    ############# Set Target Platform #############

    if fp_TargetPlatform == "":
        log_error("No target platform was selected, please specify which platform Peach-E is being built for uwu")
        return False
    
    f_CMakeConfigCommand += [
        "-DCMAKE_TOOLCHAIN_FILE=peach.toolchain.cmake",
        f"-DPEACH_TARGET_PLATFORM={fp_TargetPlatform}"
    ]

    ############# Generate CMake Project #############

    try:
        log_info(f"Running CMake project generation for {f_GeneratorMap[fp_Generator]}...")

        run_command_with_live_output(f_CMakeConfigCommand + fp_ExtraConfigs)

    except subprocess.CalledProcessError as err:
        log_error("CMake project generation failed!")
        return False

    log_success("CMake project generation completed!")

    ############# Run CMake Build Process for Single Config #############

    if not f_IsMultiConfig:
        try:
            log_info(f"Running CMake single config build for {fp_BuildType}...")

            run_command_with_live_output(['cmake', '--build', 'build'] + fp_ExtraBuildArgs)

        except subprocess.CalledProcessError as err:
            log_error(f"CMake single config {fp_BuildType} build process failed!")
            return False

        log_success(f"{fp_BuildType} build completed!")

        return True #return immediately since we don't need to go through the --config commands for single config generators

    ############# Run Debug Build #############

    if( fp_BuildType == "Debug" or fp_BuildType == "Release and Debug" ):
        try:
            log_info("Running CMake build for Debug...")

            run_command_with_live_output(['cmake', '--build', 'build', '--config', 'Debug'] + fp_ExtraBuildArgs)

        except subprocess.CalledProcessError as err:
            log_error("CMake debug build process failed!")
            return False

        log_success("Debug build completed!")

    ############# Run Release Build #############

    if( fp_BuildType == "Release" or fp_BuildType == "Release and Debug" ):
        try:
            log_info("Running CMake build for Release...")

            run_command_with_live_output(['cmake', '--build', 'build', '--config', 'Release'] + fp_ExtraBuildArgs)

        except subprocess.CalledProcessError as err:
            log_error("CMake release build process failed!")
            return False

        log_success("Release build completed!")

    ############# Success! #############

    log_info("Your CMake project should be good to go!")

    return True

############# Zip Dependency Function #############

def unpack_versioned_dep(fp_WorkingDirectory: str, fp_ArchivePatternName: str) -> bool:
        
        f_OriginalDir = os.getcwd() #get starting point so we can return back to it to avoid mutation inside this function
        
        os.chdir(fp_WorkingDirectory)

        # Find the zip
        zipped_dep_file = [f for f in os.listdir(fp_WorkingDirectory) if f.startswith(fp_ArchivePatternName) and f.endswith(".zip")]

        if not zipped_dep_file:
            log_error(f"No {fp_ArchivePatternName} found!, Cannot continue with building Peach Engine ;w;")
            return False
        
        # Assume only one versioned zip exists at a time
        latest_zip = zipped_dep_file[0]
        expected_folder = latest_zip.replace(".zip", "")  # e.g., "debug_v144"

        # Remove any folder that doesn't match the zip version
        for item in os.listdir(fp_WorkingDirectory):
            if item.startswith(fp_ArchivePatternName) and os.path.isdir(item) and item != expected_folder:
                log_info(f"Removing stale folder: {item}")
                shutil.rmtree(os.path.join(fp_WorkingDirectory, item))

        # Extract the zip if and only if the extracted dep doesn't already exist
        if not os.path.isdir(expected_folder): 
            log_info(f"Unzipping {latest_zip}")
            with zipfile.ZipFile(latest_zip, "r") as zip_ref:
                zip_ref.extractall("./") 
        
        os.chdir(f_OriginalDir) #return to the starting dir to reset state

        return True

############################################################################## Android Build ##############################################################################

def FindAndroidNdk():
    """
    Resolve the absolute path to the Android NDK directory.
    Returns the path string on success, or None if no NDK could be located.

    Lookup order:
        1. ANDROID_NDK_HOME env var (explicit override)
        2. ANDROID_NDK_ROOT env var (NDK's own scripts use this)
        3. ANDROID_NDK env var (CMake sometimes sets this internally)
        4. ANDROID_HOME / ANDROID_SDK_ROOT env vars, then walk into ndk/<version>/
        5. ANDROID_HOME / ANDROID_SDK_ROOT env vars, then walk into ndk-bundle/
    """

    ############# direct env vars pointing at the NDK itself #############

    f_DirectNdkEnvVars = ["ANDROID_NDK_HOME", "ANDROID_NDK_ROOT", "ANDROID_NDK"]

    for f_EnvVarName in f_DirectNdkEnvVars:
        f_Candidate = os.environ.get(f_EnvVarName)

        if not f_Candidate or not os.path.isdir(f_Candidate):
            log_info(f"android sdk env var: {f_Candidate} not found, continuing search...")
            continue

        log_info(f"found android sdk env var: {f_Candidate}!")
        return f_Candidate

    ############# SDK-relative fallbacks via ANDROID_HOME / ANDROID_SDK_ROOT #############

    log_info("could not find ndk, trying ANDROID_HOME...")

    f_SdkRoot = os.environ.get("ANDROID_HOME")

    if not f_SdkRoot:
        log_info("ANDROID_HOME not found, trying ANDROID_SDK_ROOT")

        f_SdkRoot = os.environ.get("ANDROID_SDK_ROOT")

        if not f_SdkRoot or not os.path.isdir(f_SdkRoot):
            log_error("ANDROID_SDK_ROOT not found!, could not find suitable tools for cross compiling peachy for android, please install and set env vars for Android Studio")
            return None

    ############# modern layout: $SDK/ndk/<version>/ #############

    f_VersionedNdkRoot = os.path.join(f_SdkRoot, "ndk")

    if os.path.isdir(f_VersionedNdkRoot):
        f_AvailableVersions = []

        for f_Entry in os.listdir(f_VersionedNdkRoot):
            f_EntryPath = os.path.join(f_VersionedNdkRoot, f_Entry)

            if os.path.isdir(f_EntryPath):
                f_AvailableVersions.append(f_Entry)

        if f_AvailableVersions:
            f_AvailableVersions.sort(reverse=True) #lexicographic descending picks the highest semver-style version
            f_FullPathToNdk = os.path.join(f_VersionedNdkRoot, f_AvailableVersions[0])

            log_info(f"found a viable NDK, choosing latest version at: {f_FullPathToNdk}")
            return f_FullPathToNdk

    ############# legacy layout: $SDK/ndk-bundle/ #############

    log_info(f"found {f_SdkRoot}, however unable to find NDK, trying legacy ndk-bundle instead")

    f_LegacyNdkBundle = os.path.join(f_SdkRoot, "ndk-bundle")

    if not os.path.isdir(f_LegacyNdkBundle):
        log_error("found android sdk, however unable to find suitable NDK, are you sure you've installed the NDK packages?")
        return None
    
    return f_LegacyNdkBundle

############################################################################## Android Helpers ##############################################################################

def IsWindows():
    return platform.system() == "Windows"

def GetGradlewName():
    """The gradle wrapper has two flavors. Windows uses gradlew.bat, every Un*x uses gradlew."""
    return "gradlew.bat" if IsWindows() else "gradlew"

def GetExeName(fp_BaseName):
    """Append .exe on Windows, leave bare on Un*x."""
    return fp_BaseName + ".exe" if IsWindows() else fp_BaseName

def FindAdb():
    """
    Locate adb. Same env-var search order as ANDROID_HOME — adb lives at <SDK>/platform-tools/adb[.exe]
    Returns full path or None. Does NOT rely on PATH because we want consistent behavior whether or not
    the user added platform-tools to PATH.
    """
    f_SdkRoot = os.environ.get("ANDROID_HOME") or os.environ.get("ANDROID_SDK_ROOT")

    if f_SdkRoot:
        f_AdbPath = os.path.join(f_SdkRoot, "platform-tools", GetExeName("adb"))

        if os.path.isfile(f_AdbPath):
            return f_AdbPath

    ############# fallback: maybe it's on PATH #############

    f_PathAdb = shutil.which("adb")

    if f_PathAdb:
        return f_PathAdb

    return None

def RunSubprocessLive(fp_Cmd, fp_Cwd = None, fp_EnvOverrides = None):
    """
    Run a subprocess streaming its output live to our stdout/stderr (no buffering).
    Returns the exit code. fp_Cmd is a list, not a string — never use shell=True with user-controlled paths.
    """
    f_Env = os.environ.copy()

    if fp_EnvOverrides:
        f_Env.update(fp_EnvOverrides)

    f_Process = subprocess.Popen(
        fp_Cmd,
        cwd=fp_Cwd,
        env=f_Env,
        stdout=None,    # inherit our stdout, outputs streams live
        stderr=None,    # same for stderr
    )

    return f_Process.wait()

def PackageAndroidApk(fp_BaseDir, fp_BuildType):
    """
    Copy the freshly-built libpeach_core.so into the gradle project's jniLibs dir,
    then invoke gradlew to assemble the APK.
    """

    ############# locate the .so we just built #############
    # adjust this path to wherever your android cmake build outputs land
    # cmake with ninja typically dumps shared libs at build/<config>/

    f_BuiltSo = os.path.join(fp_BaseDir, "build", "libpeach_core.so")

    if not os.path.isfile(f_BuiltSo):
        ############# fallback search — find it wherever cmake stuck it #############

        f_Found = None

        for f_Root, f_Dirs, f_Files in os.walk(os.path.join(fp_BaseDir, "build")):
            if "libpeach_core.so" in f_Files:
                f_Found = os.path.join(f_Root, "libpeach_core.so")
                break

        if not f_Found:
            log_error("could not find libpeach_core.so in build/ — did the native build actually succeed?")
            return False

        f_BuiltSo = f_Found

    ############# ensure jniLibs/arm64-v8a/ exists #############

    f_AndroidProjectDir = os.path.join(fp_BaseDir, "res", "android_project")
    f_JniLibsDir = os.path.join(f_AndroidProjectDir, "app", "src", "main", "jniLibs", "arm64-v8a")

    os.makedirs(f_JniLibsDir, exist_ok=True)

    f_DestSo = os.path.join(f_JniLibsDir, "libpeach_core.so")

    log_info(f"copying {f_BuiltSo} -> {f_DestSo}")
    shutil.copy2(f_BuiltSo, f_DestSo)

    ############# invoke gradlew #############

    f_GradlewPath = os.path.join(f_AndroidProjectDir, GetGradlewName())

    if not os.path.isfile(f_GradlewPath):
        log_error(f"gradlew wrapper not found at {f_GradlewPath} >w<")
        return False

    ############# Un*x systems need exec bit set on gradlew (lost when copied through windows etc) #############

    if not IsWindows():
        f_CurrentMode = os.stat(f_GradlewPath).st_mode
        os.chmod(f_GradlewPath, f_CurrentMode | 0o111) # add execute for u/g/o

    f_GradleTask = "assembleRelease" if fp_BuildType == "Release" else "assembleDebug"

    log_info(f"running gradlew {f_GradleTask} ~ nya~")

    f_ExitCode = RunSubprocessLive(
        [f_GradlewPath, f_GradleTask],
        fp_Cwd=f_AndroidProjectDir
    )

    if f_ExitCode != 0:
        log_error(f"gradlew {f_GradleTask} failed with exit code {f_ExitCode}")
        return False

    f_ApkSubdir = "release" if fp_BuildType == "Release" else "debug"
    f_ApkName = f"app-{f_ApkSubdir}.apk"
    f_ApkPath = os.path.join(f_AndroidProjectDir, "app", "build", "outputs", "apk", f_ApkSubdir, f_ApkName)

    if os.path.isfile(f_ApkPath):
        log_info(f"APK built successfully at {f_ApkPath}")
        return True
    else:
        log_error(f"gradlew reported success but APK not found at {f_ApkPath}")
        return False

def InstallAndroidApk(fp_BaseDir, fp_BuildType):
    """Install the APK onto whichever device adb sees first."""

    f_Adb = FindAdb()

    if not f_Adb:
        log_error("adb not found — is the Android SDK platform-tools installed?")
        return False

    f_ApkSubdir = "release" if fp_BuildType == "Release" else "debug"
    f_ApkName = f"app-{f_ApkSubdir}.apk"
    f_ApkPath = os.path.join(fp_BaseDir, "res", "android_project", "app", "build", "outputs", "apk", f_ApkSubdir, f_ApkName)

    log_info(f"installing {f_ApkName} via adb~")

    f_ExitCode = RunSubprocessLive([f_Adb, "install", "-r", f_ApkPath])

    if f_ExitCode != 0:
        log_error(f"adb install failed with exit code {f_ExitCode} — is a device connected? (run `{f_Adb} devices` to check)")
        return False

    return True

def LaunchAndroidApk():
    """Launch PeachActivity and tail logcat; Blocks until user kills it (Ctrl+C)"""

    f_Adb = FindAdb()

    if not f_Adb:
        log_error("adb not found")
        return False

    ############# clear stale log buffer first so we only see this run's output #############

    RunSubprocessLive([f_Adb, "logcat", "-c"])

    ############# launch the activity #############

    f_LaunchExitCode = RunSubprocessLive([
        f_Adb, "shell", "am", "start",
        "-n", "com.starlightbrew.peach/.PeachActivity"
    ])

    if f_LaunchExitCode != 0:
        log_error("failed to launch activity")
        return False

    log_info("tailing logcat — press Ctrl+C to stop ~ nya~")

    ############# tail filtered logcat — blocks until user interrupts #############

    try:
        RunSubprocessLive([
            f_Adb, "logcat",
            "-s", "SDL:V", "SDL/APP:V", "peach_core:V",
            "AndroidRuntime:E", "DEBUG:E", "libc:E"
        ])

    except KeyboardInterrupt:
        log_info("stopped tailing logcat")

    return True

############################################################################## Build Status Enum >///< ##############################################################################

class ToolStatus(Enum):
    BUILD_SUCCESS = 1
    BUILD_FAILED = 2
    CLEAN_OR_NUKE_REQUESTED = 3
    CLEAN_FAILED = 4
    NUKE_FAILED = 5
    ANDROID_INSTALL_FAILED = 6
    ANDROID_PACKAGE_FAILED = 7
    ANDROID_LAUNCH_FAILED = 8
    MISFORMED_BUILD_ARGUMENTS_PASSED = 9

############################################################################## Main Function ##############################################################################

def main() -> ToolStatus:

    ############# Check for Required Build Tools in PATH #############
    
    if not ensure_tool_installed("cmake"): 
        return ToolStatus.BUILD_FAILED   

    ############# Setup Parser #############

    usage_message =                                                                                     \
        CreateColouredText("init.py ", 'bright magenta') +                                              \
        CreateColouredText("--[release, debug or both] ", "bright blue") +                              \
        CreateColouredText("-G [desired_generator] ", "bright green") +                                 \
        CreateColouredText("...extra flags", "bright cyan")

    parser = argparse.ArgumentParser(
        description=CreateColouredText('Used for Building Peach-E from Source', 'bright green'), 
        usage=usage_message, 
        add_help=True,
        formatter_class=argparse.RawTextHelpFormatter
    )

    ############# Set Parser Arguments #############

    parser.add_argument(
        '--release', 
        action='store_true', 
        help=CreateColouredText('Used for a release build', 'bright magenta')
    )

    parser.add_argument(
        '--debug', 
        action='store_true', 
        help=CreateColouredText('Used for a debug build', 'bright magenta')
    )

    parser.add_argument(
        '--both', 
        action='store_true', 
        help=CreateColouredText('Used to build both a debug and release build', 'bright magenta')
    )

    parser.add_argument(
        '--clean', 
        action='store_true', 
        help=CreateColouredText('Used to clean peachy build artifacts from a previous run', 'bright magenta')
    )

    parser.add_argument(
        '--nuke', 
        action='store_true', 
        help=CreateColouredText('Used to completely wipe the build directory including all compiled dependencies', 'bright magenta')
    )

    parser.add_argument(
        '-G', 
        nargs=1,
        metavar="[generator]",
        help=
            CreateColouredText('Used to set the project file generator, options are as follows:', 'bright magenta') + "\n" +                                        
                "\t" + CreateColouredText('-G ', 'blue') +  CreateColouredText("vs2015 --> vs2026 ", 'bright green') + CreateColouredText('Generates solution for Visual Studio 2015 - 2026', 'cyan') + "\n" + 
                
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("xcode ", 'bright green') + CreateColouredText('Generates project files for Xcode', 'cyan') + "\n" +                            
                
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("ninja ", 'bright green') + CreateColouredText('Generates project files using Ninja', 'cyan') + "\n" +                          
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("ninja-mc ", 'bright green') + CreateColouredText('For Ninja Multi-Config', 'cyan') + "\n" +                                    
                
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("unix ", 'bright green') + CreateColouredText('For Unix Makefiles', 'cyan') + "\n" +                                            
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("unix-eclipse ", 'bright green') + CreateColouredText('Generate Unix Makefiles for Eclipse CDT', 'cyan') + "\n" +               
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("unix-cb ", 'bright green') + CreateColouredText('Generates Unix Makefiles for CodeBlocks', 'cyan') + "\n" +                    

                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("mingw ", 'bright green') + CreateColouredText('Generates MinGW Makefiles', 'cyan') + "\n" +                                    
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("msys ", 'bright green') + CreateColouredText('Generates MSYS Makefiles', 'cyan') + "\n" +                                      
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("nmake ", 'bright green') + CreateColouredText('Generates NMake Makefiles', 'cyan') + "\n" +                                    
                "\t" + CreateColouredText('-G ', 'blue') + CreateColouredText("nmake-jom ", 'bright green') + CreateColouredText('Generates JOM Makefiles', 'cyan')
    )   
    
    parser.add_argument(
        '-T',
        nargs=1,
        metavar="[target]",
        help=
            CreateColouredText("Valid toolchain keys are: ", 'bright magenta') + "\n" + 
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" windows-arm64 or windows-x64",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" macos-arm64 or macos-x64",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" linux-arm64 or linux-x64",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" freebsd-arm64 or freebsd-x64",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" haiku",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" ios",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" tvos",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" android",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" wasm",'bright green') + "\n" +
                "\t" + CreateColouredText("-T", "blue") + CreateColouredText(" psvita",'bright green') + "\n" 
    )

    parser.add_argument(
        '-J',
        nargs=1,
        metavar="[max_jobs]",
        help=CreateColouredText("Set max number of jobs the compiler can do at once owo",'bright magenta')
    )

    parser.add_argument(
        '--dump_errors',
        action='store_true',
        help=CreateColouredText('Dump all build errors to build_errors.md', 'bright magenta')
    )

    parser.add_argument(
        '--dump_warnings',
        action='store_true',
        help=CreateColouredText('Dump all build warnings to build_warnings.md', 'bright magenta')
    )

    parser.add_argument(
        '--dump_output',
        action='store_true',
        help=CreateColouredText('Dump all build warnings + errors', 'bright magenta')
    )

    parser.add_argument(
        '--export_commands',
        action='store_true',
        help=CreateColouredText('Export compile commands', 'bright magenta')
    )

    parser.add_argument(
        '--verbose',
        action='store_true',
        help=CreateColouredText('Adds verbose and will add additional flags depending on generator', 'bright magenta')
    )

    parser.add_argument(
        '--use_clang',
        action='store_true',
        help=CreateColouredText('Compiles with clang on compatible platforms', 'bright magenta')
    )

    parser.add_argument(
        '--use_gcc',
        action='store_true',
        help=CreateColouredText('Compiles with gcc on compatible platforms', 'bright magenta') + '\n' + #uwu
        CreateColouredText("======================================================= ANDROID ONLY =======================================================", 'bright red') 
    )

    parser.add_argument(
        "--package_apk",
        action="store_true",
        help=CreateColouredText("after a successful android build, copy the .so into the gradle project and run gradlew assembleDebug/Release", 'bright magenta')
    )

    parser.add_argument(
        "--install_apk",
        action="store_true",
        help=CreateColouredText("after --package_apk, install the resulting apk to the connected device via adb", 'bright magenta')
    )

    parser.add_argument(
        "--launch_apk",
        action="store_true",
        help=CreateColouredText("after --install_apk, launch the activity and tail logcat", 'bright magenta')
    )

    args = parser.parse_args()

    ############# KABOOOOOOOOOOOOOOOOOOOOOOOOOOM #############

    if args.nuke and args.clean:
        log_error("tried to pass --nuke and --clean, you can only pick one; with great power comes great responsibility einstein said that, are you saying you're smarter than einstein?")
        return ToolStatus.MISFORMED_BUILD_ARGUMENTS_PASSED

    if args.nuke:
        
        print(CreateColouredText("[THE HORROR]: 3..2..1.. clearance granted, firing 😔", "bright magenta"))

        try:
            shutil.rmtree('build')
        except FileNotFoundError:
            log_info("build directory doesn't exist, just ignoring --nuke call >w<")
        except PermissionError as f_Exception:
            log_error(f"permission denied for nuking ;w; what am i supposed to do now? what: {f_Exception}")
            return ToolStatus.NUKE_FAILED
        except Exception as f_Exception:
            log_error(f"unable to nuke build directory idk y, what: {f_Exception}")
            return ToolStatus.NUKE_FAILED

    elif args.clean:

        log_info("Scraping only peach components... leaving dependencies cozy ~nya~ 🍑✨")

        f_TargetBaseNames = ["peach_core", "Peach_Editor", "Peach_Engine", "Peach_Tests"]

        # Wipe out standard CMake intermediate directory layout states

        for f_Target in f_TargetBaseNames:
            shutil.rmtree(f"build/CMakeFiles/{f_Target}.dir", ignore_errors=True) 

        # Recursive Binary Extermination Pass

        for f_Target in f_TargetBaseNames:
            
            f_RecursivePattern = f"build/**/{f_Target}*"  # target both exact name matches and variants with extensions (.exe, .lib, .a, .pdb)
            
            for f_FilePath in glob.glob(f_RecursivePattern, recursive=True):

                if "third_party" in f_FilePath or "src" in f_FilePath:  # Avoid accidentally nuking live source root folders if something goes wild
                    continue
                    
                if os.path.isdir(f_FilePath): # Catch and delete directory matching artifacts (like target project folders in VS/Xcode)
                    
                    if f_FilePath.endswith(".dir") or f_FilePath.endswith(".framework") or f_FilePath.endswith(".bundle"):
                        shutil.rmtree(f_FilePath, ignore_errors=True)
                else: # Catch and delete individual binary files (.exe, .a, .lib, .so, .dylib, .pdb, .ninja)
                    try:
                        os.remove(f_FilePath)
                        log_info(f"successfully removed: '{f_FilePath}'")
                    except OSError:
                        log_error(f"failed to remove: '{f_FilePath}' during --clean")
                        return ToolStatus.CLEAN_FAILED

        log_success("Clean completed! Peach components completely purged, dependencies preserved")

    ############# Validate Build Config #############

    f_BuildType = "nothing"

    if(args.debug):
        f_BuildType = "Debug"

    elif(args.release):
        f_BuildType = "Release"

    elif(args.both):
        f_BuildType = "Release and Debug"
    
    elif args.clean or args.nuke:
        return ToolStatus.CLEAN_OR_NUKE_REQUESTED #just gonna assume if no build config was passed they just wanted a clean uwu

    else:
        log_error("No valid build type input detected, use -h or --help if you're unfamiliar")
        return ToolStatus.MISFORMED_BUILD_ARGUMENTS_PASSED

    ############# Detect Platform #############

    f_CurrentPlatform = platform.system()

    ############# Get Current Working Directory #############

    f_BaseDir = os.getcwd()

    ############# Extra args and cmake configs owo #############

    f_ExtraBuildArgs = []
    f_ExtraGenerationConfigs = []

    ############# Check for Generator #############
        
    if(not args.G):
        log_error("please specify cmake generator using -G [desired_generator] >w<")
        return ToolStatus.BUILD_FAILED

    f_DesiredGenerator = args.G[0].lower() #convert to all lower case for easier handling

    ############# Export compile commands? #############

    if args.verbose:
        if f_DesiredGenerator == "vs2022":
            f_ExtraBuildArgs += ['--verbose', '--', '-verbosity:diagnostic']

    ############# Thread Limiter #############

    if args.J:
        f_MaxNumberOfJobs = args.J[0]

        f_ExtraBuildArgs.extend(["--parallel", f_MaxNumberOfJobs])

    ############# Export compile commands? #############

    if args.export_commands:
        f_ExtraGenerationConfigs.append('-DCMAKE_EXPORT_COMPILE_COMMANDS=ON') 

    ############# Compiler Identification #############

    if args.use_clang:

        if f_CurrentPlatform == "Windows":
            if ensure_tool_installed("clang-cl"):
                f_ExtraGenerationConfigs.extend(
                    [
                        "-T", "ClangCL", 
                        "-DCMAKE_C_COMPILER=clang-cl", 
                        "-DCMAKE_CXX_COMPILER=clang-cl"
                    ]
                )
            else:
                print_tip("please make sure you have the llvm toolchain for visual studio installed before using --use_clang on windows owo")
                return ToolStatus.BUILD_FAILED
        
        elif not ensure_tool_installed("clang") and not ensure_tool_installed("clang++"):          
            return ToolStatus.BUILD_FAILED

        f_ExtraGenerationConfigs.extend(["-DCMAKE_C_COMPILER=clang", "-DCMAKE_CXX_COMPILER=clang++"])

    elif args.use_gcc:

        if f_CurrentPlatform == "Windows":
            log_error("can't use gcc/g++ on windows, aborting build process")

        if not ensure_tool_installed("gcc") and not ensure_tool_installed("g++"):
            return ToolStatus.BUILD_FAILED

        f_ExtraGenerationConfigs.extend(["-DCMAKE_C_COMPILER=gcc", "-DCMAKE_CXX_COMPILER=g++"])       

    ############# Target Platform Config #############

    f_ToolchainKey = ""

    if args.T:

        f_ToolchainKey = args.T[0].lower()

        f_ValidToolchainKeys = [
            "windows-arm64", "windows-x64", 
            "macos-arm64", "macos-x64", 
            "linux-arm64", "linux-x64", 
            "freebsd-arm64", "freebsd-x64", 
            "haiku",
            "android",
            "ios", "tvos",
            "wasm",
            "psvita"
        ]

        if f_ToolchainKey not in f_ValidToolchainKeys:
            log_error("invalid toolchain key was detected, please use -h to see the list of valid toolchain keys")
            return ToolStatus.BUILD_FAILED
        
    else:
        f_MachineArch = platform.machine().lower()

        if f_CurrentPlatform == "Windows":
            f_ToolchainKey = "windows-arm64" if "arm" in f_MachineArch else "windows-x64" #python is weird mang
        elif f_CurrentPlatform == "Darwin":
            f_ToolchainKey = "macos-arm64" if "arm" in f_MachineArch else "macos-x64" #python is weird mang
        elif f_CurrentPlatform == "Linux":
            f_ToolchainKey = "linux-arm64" if "arm" in f_MachineArch else "linux-x64" #python is weird mang
        elif f_CurrentPlatform == "FreeBSD":
            f_ToolchainKey = "freebsd-arm64" if "arm" in f_MachineArch else "freebsd-x64" #python is weird mang
        elif f_CurrentPlatform == "Haiku":
            f_ToolchainKey = "haiku" #arm64 is experimental atm apparently, also this shi gave my first PC BIOS cancer lmfao wasn't the same after that failed install lol
        else:
            log_error(f"Could not auto-detect platform: {f_CurrentPlatform}, please specify with -T uwu")
            return ToolStatus.BUILD_FAILED

        log_info(f"Auto-detected platform: {f_ToolchainKey} ~ nya~")

    ############# Android #############

    if f_ToolchainKey == "android":

        f_AndroidNdkAbsolutePath = FindAndroidNdk() # Android Studio reliably sets ANDROID_HOME or ANDROID_SDK_ROOT, NDK installs under $SDK/ndk/<version>/ (newer) or $SDK/ndk-bundle/ (old).
        
        if not f_AndroidNdkAbsolutePath:
            log_error("unable to locate required tools for Android cross compilation >w< stopping build immediately")
            return ToolStatus.BUILD_FAILED
        
        f_AndroidNdkAbsolutePath = f_AndroidNdkAbsolutePath.replace("\\", "/") #replace chars since on windows cmake is kinda stupid
        
        f_ExtraGenerationConfigs.extend(
            [
                f"-DCMAKE_ANDROID_NDK={f_AndroidNdkAbsolutePath}", #have to set this manually since cmake is kinda cooked ngl ¯\_(ツ)_/¯
                "-DANDROID_ABI=arm64-v8a",
                "-DANDROID_PLATFORM=android-24",
                "-DANDROID_STL=c++_static"
            ]
        )

        log_info(f"Hooked Android NDK Toolchain at {f_AndroidNdkAbsolutePath} ~ nya~")

    ############# WASM #############

    elif f_ToolchainKey == "wasm":
        if not os.environ.get("EMSDK"):
            log_error("unable to verify EMSDK env var, please set this environment variable and try again ;w;")

    ############# iPhone and iPad #############

    elif f_ToolchainKey == "ios":
        pass

    ############# TvOS #############

    elif f_ToolchainKey == "tvos":
        pass

    ############# Run Build Fingers Crossed >w< #############

    build_result = run_cmake(f_BuildType, f_DesiredGenerator, f_ToolchainKey, f_ExtraBuildArgs, f_ExtraGenerationConfigs)

    ############# Provide Printout #############

    if args.dump_output:
        WriteBuildSummaryMarkdown(".", True, True);
    elif args.dump_warnings:
        WriteBuildSummaryMarkdown(".", False, True);
    elif args.dump_errors:
        WriteBuildSummaryMarkdown(".", True, False);
    
    ############# Android Post-Build: Package, Install, Launch #############

    if f_ToolchainKey == "android":
        if (args.package_apk or args.install_apk or args.launch_apk) and not PackageAndroidApk(f_BaseDir, f_BuildType):
            return ToolStatus.ANDROID_PACKAGE_FAILED

        if (args.install_apk or args.launch_apk) and not InstallAndroidApk(f_BaseDir, f_BuildType):
            return ToolStatus.ANDROID_INSTALL_FAILED

        if args.launch_apk and not LaunchAndroidApk():
            return ToolStatus.ANDROID_LAUNCH_FAILED

    ############# return false on failed build ;w; #############

    if not build_result:
        return ToolStatus.BUILD_FAILED
    
    ############# Report Build Stats #############

    print(CreateColouredText(f"Final Build Summary: \n", "bright green"))
    print(CreateColouredText(f"\tGenerator: {f_DesiredGenerator}", "bright magenta"))
    print(CreateColouredText(f"\tBuild Type: {f_BuildType}", "bright magenta"))
    print(CreateColouredText(f"\tPlatform: {f_ToolchainKey}\n", "bright magenta"))

    return ToolStatus.BUILD_SUCCESS

############# Main Caller #############

if __name__ == "__main__":

    if platform.system() == "Windows": #enable ANSI colour codes for Windows Console
        os.system('color') 

    if main() == ToolStatus.BUILD_FAILED:
        log_error("execution of full build process was unsuccessful\n")
    else:
        print(CreateColouredText("done!\n", "magenta"))


#Rawr OwO
