import subprocess
import os
import argparse
import platform
import shutil
import sys
import zipfile
import re

from shutil import which

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
        print(CreateColouredText("[Warning]: no valid input detected for CreateColouredText, returned original text in all lower-case", "yellow"))
        return fp_SampleText
    
    else:
        return f"{f_ListOfColours.get(fp_DesiredColour, '')}{fp_SampleText}\033[0m"

############# Utility for Validating Required Build Tools #############

def ensure_tool_installed(fp_ToolName: str) -> bool:

    if which(fp_ToolName) is None:
        print(CreateColouredText(f"[ERROR]: Required tool '{fp_ToolName}' not found in PATH", "red"))
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
    # re.compile(r'\bfailed\b',              re.IGNORECASE),  # ninja "FAILED: CMakeFiles/..." / MSBuild "Build FAILED."

    re.compile(r'\blnk\d{4}\b',             re.IGNORECASE), # MSVC linker: LNK1181, LNK2019 etc — NOT bare "lnk"
    re.compile(r'\b[Cc][2-9]\d{3}\b'),                      # MSVC compiler: C2065, C3861 — NOT "vec2" / "vec3"
    re.compile(r'\bld:\s+error\b',          re.IGNORECASE), # GNU ld errors
    re.compile(r'\bundefined\s+symbol\b',   re.IGNORECASE), # linker: undefined symbol
    re.compile(r'\bduplicate\s+symbol\b',         re.IGNORECASE), # linker: duplicate symbol
    re.compile(r'\bundefined\s+reference\b',re.IGNORECASE), # GCC linker variant
    re.compile(r'\breferenced\s+from\b',    re.IGNORECASE), # Apple ld variant
    
    re.compile(r'\bninja:\s+error\b',       re.IGNORECASE), # "ninja: error:" — only ninja errors, not every ninja line
    re.compile(r'\bcommand\s+failed\b',     re.IGNORECASE), # generic CMake command failure
    re.compile(r'cmake\s+error',            re.IGNORECASE), # CMake configure errors

    re.compile(r'\binternal\s+compiler\s+error\b',re.IGNORECASE), # GCC/Clang ICE
]

_g_WarningPatterns = [
    re.compile(r':\s*warning\b',            re.IGNORECASE), # "warning:" / ": warning" — GCC, Clang, MSVC
    re.compile(r'\b[Cc]4\d{3}\b'),                          # MSVC warnings: C4100, C4244 etc
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

        print(CreateColouredText(f"\n[INFO]: Error summary written to {f_ErrorPath}", "bright cyan"))

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

        print(CreateColouredText(f"[INFO]: Warning summary written to {f_WarnPath}", "bright cyan"))

############# Main CMake Function #############

def run_cmake(fp_BuildType: str, fp_Generator: str, fp_TargetPlatform: str, fp_ExtraArgs: list, fp_ExtraConfigs: list) -> bool:

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
        print(CreateColouredText("[ERROR]: Invalid Generator Selected, PLEASE PICK A VALID GENERATOR", "red"))
        return False
    
    ############# Determine if Generator is Single Config #############
    
    f_IsMultiConfig = fp_Generator in ["vs2026", "vs2022", "vs2019", "vs2017", "vs2015", "xcode", "ninja-mc"]

    f_CMakeConfigCommand = ['cmake', '-S', '.', '-B', 'build', '-G', f_GeneratorMap[fp_Generator]]
    
    if not f_IsMultiConfig:

        if fp_BuildType == "Release and Debug": #Don't allow "both" configs for single config generators uwu
            
            print(CreateColouredText("[ERROR]: Invalid build type selected: YOU CANNOT USE BOTH WHEN GENERATING FOR A SINGLE CONFIG GENERATOR", "red"))
            return False
        
        else:
            f_CMakeConfigCommand += ['-DCMAKE_BUILD_TYPE=' + fp_BuildType.capitalize()]

    ############# Set Target Platform #############

    if fp_TargetPlatform == "":
        print(CreateColouredText("[ERROR]: No target platform was selected, please specify which platform Peach-E is being built for uwu"))
        return False
    
    f_CMakeConfigCommand += [
        "-DCMAKE_TOOLCHAIN_FILE=peach.toolchain.cmake",
        f"-DPEACH_TARGET_PLATFORM={fp_TargetPlatform}"
    ]

    ############# Generate CMake Project #############

    try:
        print(CreateColouredText(f"[INFO]: Running CMake project generation for {f_GeneratorMap[fp_Generator]}...", "green"))

        run_command_with_live_output(f_CMakeConfigCommand + fp_ExtraConfigs)

    except subprocess.CalledProcessError as err:
        print(CreateColouredText("[ERROR]: CMake project generation failed!", "red"))
        return False

    print(CreateColouredText("[SUCCESS]: CMake project generation completed!", "cyan"))

    ############# Run CMake Build Process for Single Config #############

    if not f_IsMultiConfig:
        try:
            print(CreateColouredText(f"[INFO]: Running CMake single config build for {fp_BuildType}...", "green"))

            run_command_with_live_output(['cmake', '--build', 'build'] + fp_ExtraArgs)

        except subprocess.CalledProcessError as err:
            print(CreateColouredText(f"[ERROR]: CMake single config {fp_BuildType} build process failed!", "red"))
            return False

        print(CreateColouredText(f"\n[SUCCESS]: {fp_BuildType} build completed!", "cyan"))

        return True #return immediately since we don't need to go through the --config commands for single config generators

    ############# Run Debug Build #############

    if( fp_BuildType == "Debug" or fp_BuildType == "Release and Debug" ):
        try:
            print(CreateColouredText("[INFO]: Running CMake build for Debug...", "green"))

            run_command_with_live_output(['cmake', '--build', 'build', '--config', 'Debug'] + fp_ExtraArgs)

        except subprocess.CalledProcessError as err:
            print(CreateColouredText("[ERROR]: CMake debug build process failed!", "red"))
            return False

        print(CreateColouredText("[SUCCESS]: Debug build completed!", "cyan"))

    ############# Run Release Build #############

    if( fp_BuildType == "Release" or fp_BuildType == "Release and Debug" ):
        try:
            print(CreateColouredText("[INFO]: Running CMake build for Release...", "green"))

            run_command_with_live_output(['cmake', '--build', 'build', '--config', 'Release'] + fp_ExtraArgs)

        except subprocess.CalledProcessError as err:
            print(CreateColouredText("[ERROR]: CMake release build process failed!", "red"))
            return False

        print(CreateColouredText("[SUCCESS]: Release build completed!", "cyan"))

    ############# Success! #############

    print(CreateColouredText("\n[INFO]: Your CMake project should be good to go!", "green"))

    return True

############# Zip Dependency Function #############

def unpack_versioned_dep(fp_WorkingDirectory: str, fp_ArchivePatternName: str) -> bool:
        
        f_OriginalDir = os.getcwd() #get starting point so we can return back to it to avoid mutation inside this function
        
        os.chdir(fp_WorkingDirectory)

        # Find the zip
        zipped_dep_file = [f for f in os.listdir(fp_WorkingDirectory) if f.startswith(fp_ArchivePatternName) and f.endswith(".zip")]

        if not zipped_dep_file:
            print(CreateColouredText(f"No {fp_ArchivePatternName} found!, Cannot continue with building Peach Engine ;w;", "bright yellow"))
            return False
        
        # Assume only one versioned zip exists at a time
        latest_zip = zipped_dep_file[0]
        expected_folder = latest_zip.replace(".zip", "")  # e.g., "debug_v144"

        # Remove any folder that doesn't match the zip version
        for item in os.listdir(fp_WorkingDirectory):
            if item.startswith(fp_ArchivePatternName) and os.path.isdir(item) and item != expected_folder:
                print(CreateColouredText(f"[INFO]: Removing stale folder: {item}", "bright yellow"))
                shutil.rmtree(os.path.join(fp_WorkingDirectory, item))

        # Extract the zip if and only if the extracted dep doesn't already exist
        if not os.path.isdir(expected_folder): 
            print(CreateColouredText(f"[INFO]: Unzipping {latest_zip}", "bright green"))
            with zipfile.ZipFile(latest_zip, "r") as zip_ref:
                zip_ref.extractall("./") 
        
        os.chdir(f_OriginalDir) #return to the starting dir to reset state

        return True

############# Main Function #############

def main() -> bool:

    ############# Check for Required Build Tools in PATH #############
    
    if not ensure_tool_installed("cmake"): 
        return False

    ############# Setup Parser #############

    usage_message = \
        CreateColouredText("init.py ", 'bright magenta') + \
        CreateColouredText("--[build_type: release, debug or both] ", "bright blue") + \
        CreateColouredText("-G [desired_generator] ", "blue")

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
        help=CreateColouredText('Used to clean build artifacts from a previous run', 'bright magenta')
    )

    parser.add_argument(
        '-G', 
        nargs=1,
        metavar="[generator]",
        help=CreateColouredText('Used to set the project file generator, options are as follows:', 'bright magenta') + "\n" + \
                "\t" + CreateColouredText('-G vs2015 --> vs2026 ', 'blue') + CreateColouredText('Generates solution for Visual Studio 2015 - 2026', 'cyan') + "\n" + \
                
                "\t" + CreateColouredText('-G xcode ', 'blue') + CreateColouredText('Generates project files for Xcode', 'cyan') + "\n" + \
                
                "\t" + CreateColouredText('-G ninja ', 'blue') + CreateColouredText('Generates project files using Ninja', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G ninja-mc ', 'blue') + CreateColouredText('For Ninja Multi-Config', 'cyan') + "\n" + \
                
                "\t" + CreateColouredText('-G unix ', 'blue') + CreateColouredText('For Unix Makefiles', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G unix-eclipse ', 'blue') + CreateColouredText('Generate Unix Makefiles for Eclipse CDT', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G unix-cb ', 'blue') + CreateColouredText('Generates Unix Makefiles for CodeBlocks', 'cyan') + "\n" + \

                "\t" + CreateColouredText('-G mingw ', 'blue') + CreateColouredText('Generates MinGW Makefiles', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G msys ', 'blue') + CreateColouredText('Generates MSYS Makefiles', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G nmake ', 'blue') + CreateColouredText('Generates NMake Makefiles', 'cyan') + "\n" + \
                "\t" + CreateColouredText('-G nmake-jom ', 'blue') + CreateColouredText('Generates JOM Makefiles', 'cyan')
    )   
    
    parser.add_argument(
        '-T',
        nargs=1,
        metavar="[target]",
        help=CreateColouredText("ios, tvos, android, wasm, psvita, or leave empty for native", 'cyan')
    )

    parser.add_argument(
        '-J',
        nargs=1,
        metavar="[max_jobs]",
        help=CreateColouredText("Set max number of jobs the compiler can do at once owo",'cyan')
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

    args = parser.parse_args()

    ############# Validate Build Config #############

    f_BuildType = "nothing"

    if(args.debug):
        f_BuildType = "Debug"

    elif(args.release):
        f_BuildType = "Release"

    elif(args.both):
        f_BuildType = "Release and Debug"

    else:
        print(CreateColouredText("[ERROR]: No valid build type input detected, use -h or --help if you're unfamiliar", "red"))
        return False

    ############# Check for --clean flag #############

    if args.clean:
        shutil.rmtree('build', ignore_errors=True)

    ############# Detect Platform #############

    f_CurrentPlatform = platform.system()

    ############# Extra args and cmake configs owo #############

    f_ExtraArgs = []
    f_ExtraBuildConfigs = []

    ############# Check for Generator #############
        
    if(not args.G):
        print(CreateColouredText("[ERROR]: YOU DIDN'T USE -G FLAG BROTHER", "red"))
        return False

    f_DesiredGenerator = args.G[0].lower() #convert to all lower case for easier handling

    ############# Export compile commands? #############

    if args.verbose:
        if f_DesiredGenerator == "vs2022":
            f_ExtraArgs += ['--verbose', '--', '-verbosity:diagnostic']

    ############# Thread Limiter #############

    if f_CurrentPlatform == "FreeBSD":
        f_ExtraArgs.extend(["--", "-j", "2"])

    ############# Export compile commands? #############

    if args.export_commands:
        f_ExtraBuildConfigs.append('-DCMAKE_EXPORT_COMPILE_COMMANDS=ON')        

    ############# Target Platform Config #############

    f_ToolchainKey = ""

    if args.T:
        f_ToolchainKey = args.T[0].lower()
    else:
        f_MachineArch = platform.machine().lower()

        if f_CurrentPlatform == "Windows":
            f_ToolchainKey = "windows-arm64" if "arm" in f_MachineArch else "windows" #python is weird mang
        elif f_CurrentPlatform == "Darwin":
            f_ToolchainKey = "macos"
        elif f_CurrentPlatform == "Linux":
            f_ToolchainKey = "linux"
        elif f_CurrentPlatform == "FreeBSD":
            f_ToolchainKey = "freebsd"
        elif f_CurrentPlatform == "Haiku":
            f_ToolchainKey = "haiku"
        else:
            print(CreateColouredText(f"[ERROR]: Could not auto-detect platform: {f_CurrentPlatform}, please specify with -T uwu", "red"))
            return False

        print(CreateColouredText(f"[INFO]: Auto-detected platform: {f_ToolchainKey} ~ nya~", "bright cyan"))

    ############# Get Current Working Directory #############

    f_BaseDir = os.getcwd()

    ############# Decompress archives with large libs #############

    if(f_CurrentPlatform == "Windows"): #only needed for windows so far since the lib sizes are ridiculous

        assimp_dir = f_BaseDir + "/third_party/peach_editor/assimp/win64"

        if not unpack_versioned_dep(assimp_dir, "debug_v"): #the compressed deps are always named using debug_v* or release_v*
            return False
        if not unpack_versioned_dep(assimp_dir, "release_v"):
            return False
        
        f_ShadercDir = f_BaseDir + "/third_party/peach_editor/shaderc_combined/win64"

        if not unpack_versioned_dep(f_ShadercDir, "debug_v"): #the compressed deps are always named using debug_v* or release_v*
            return False
        if not unpack_versioned_dep(f_ShadercDir, "release_v"):
            return False

    ############# Run Build Fingers Crossed >w< #############

    build_result = run_cmake(f_BuildType, f_DesiredGenerator, f_ToolchainKey, f_ExtraArgs, f_ExtraBuildConfigs)

    ############# Provide Printout #############

    if args.dump_output:
        WriteBuildSummaryMarkdown(".", True, True);
    elif args.dump_warnings:
        WriteBuildSummaryMarkdown(".", False, True);
    elif args.dump_errors:
        WriteBuildSummaryMarkdown(".", True, False);

    ############# return false on failed build ;w; #############

    if not build_result:
        return False
    
    ############# Report Build Stats #############

    print(CreateColouredText(f"[INFO]: Final Build Summary: \n", "bright green"))
    print(CreateColouredText(f"Generator: {f_DesiredGenerator}", "bright magenta"))
    print(CreateColouredText(f"Build Type: {f_BuildType}", "bright magenta"))
    print(CreateColouredText(f"Platform: {f_CurrentPlatform}\n", "bright magenta"))

    return True

############# Main Caller #############

if __name__ == "__main__":

    if platform.system() == "Windows": #enable ANSI colour codes for Windows Console
        os.system('color') 

    if not main():
        print(CreateColouredText("[ERROR]: execution of full build process was unsuccessful\n", "red"))
    else:
        print(CreateColouredText("done!\n", "magenta"))


#Rawr OwO
