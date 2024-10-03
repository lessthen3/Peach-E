import subprocess
import os
import argparse

def CreateColouredText(fp_SampleText: str, fp_DesiredColour: str) -> str:

    if (fp_DesiredColour == "black"):
        return '\033[30m' + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "red"):
        return '\033[31m' + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "green"):
        return '\033[32m' + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "yellow"):
        return "\033[33m" + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "blue"):
        return "\033[34m" + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "magenta"):
        return "\033[35m" + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "cyan"):
        return "\033[36m" + fp_SampleText + '\033[0m'
    
    elif (fp_DesiredColour == "white"):
        return "\033[37m" + fp_SampleText + '\033[0m'
    
    else:
        return fp_SampleText;
    
def run_conan(fp_BuildType: str) -> bool:

    try:
        subprocess.run(
            ['conan', 'install', '.', '-s', 'build_type=' + fp_BuildType, '-s', 'compiler.cppstd=20', '--output-folder=build', "--build=missing"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

    except subprocess.CalledProcessError as err:
        print("Failed to run Conan:")
        print(err.stdout.decode())
        print(err.stderr.decode())
        return False

    return True

def run_cmake(fp_BuildType: str, fp_Generator: str) -> bool:

    try:
        if fp_Generator == "vs2022":
            subprocess.run(
                ['cmake', '-S', '.', '-B', 'build', '-G', 'Visual Studio 17 2022'],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

        elif fp_Generator == "ninja":
            subprocess.run(
                ['cmake', '-S', '.', '-B', 'build', '-G', 'Ninja'],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

        if( fp_BuildType == "debug" or fp_BuildType == "both" ):
            subprocess.run(
                ['cmake', '--build', 'build', '--config', 'Debug'],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

        if( fp_BuildType == "release" or fp_BuildType == "both" ):
            subprocess.run(
                ['cmake', '--build', 'build', '--config', 'Release'],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )

    except subprocess.CalledProcessError as err:
        print(CreateColouredText("Failed to run CMake:", "red"))
        print(err.stdout.decode())
        print(err.stderr.decode())
        return False

    return True


if __name__ == "__main__":

    os.system('color') #enable ANSI colour codes

    parser = argparse.ArgumentParser(description='Used for Building Peach-E from Source')

    parser.add_argument(
        '--release', 
        action='store_true', 
        help=CreateColouredText('Used release for only a release build', 'cyan')
    )

    parser.add_argument(
        '--debug', 
        action='store_true', 
        help=CreateColouredText('Used debug for only a debug build', 'cyan')
    )

    parser.add_argument(
        '--both', 
        action='store_true', 
        help=CreateColouredText('Used both for a debug and release build', 'cyan')
    )

    parser.add_argument(
        '-G', 
        action='store_true', 
        help=CreateColouredText('Used to set the project file generator', 'cyan') + CreateColouredText('\n eg. -G vs2022', 'blue')
    )

    parser.add_argument(
        'vs2022',
        action='store_true',
        help=CreateColouredText('Option for project file generation', 'cyan') + CreateColouredText('\n eg. -G vs2022', 'blue')
    )
    
    args = parser.parse_args()

    f_IsSetupSuccessful = False

    if(args.debug):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        f_IsSetupSuccessful = run_cmake("debug", "vs2022")

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug, your CMake project should be good to go!", "green"))
        
    elif(args.release):

        if run_conan("Release"):   # >w>
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("release", "vs2022")

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for release, your CMake project should be good to go!", "green"))

    elif(args.both):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        if run_conan("Release"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("both", "vs2022")
        
        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug and release, your CMake project should be good to go!", "green"))
        
    else:
        print(CreateColouredText("No valid input was detected, nothing was done.", "red"))

    if f_IsSetupSuccessful:
        print(CreateColouredText("done!", "magenta"))
    