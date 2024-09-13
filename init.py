import subprocess
import os
import argparse

def CreateColouredText(fp_SampleText, fp_DesiredColour) -> str:

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
    
def run_conan(build_type) -> bool:

    try:
        subprocess.run \
        (
            ['conan', 'install', '.', '-s', 'build_type=' + build_type, '-s', 'compiler.cppstd=20', '--output-folder=build', "--build=missing"],
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

def run_cmake(build_type) -> bool:

    try:
        subprocess.run \
        (
            ['cmake', '-S', '.', '-B', 'build'],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        if( build_type == "debug" or build_type == "both" ):
            subprocess.run \
            (
                ['cmake', '--build', 'build', '--config', 'Debug'],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
        if( build_type == "release" or build_type == "both" ):
            subprocess.run \
            (
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

    parser = argparse.ArgumentParser(description='Used for Building Peach-E from Source')

    parser.add_argument('--release', action='store_true', help='Use release for only a release build')
    parser.add_argument('--debug', action='store_true', help='Use debug for only a debug build')
    parser.add_argument('--both', action='store_true', help='Use both for a debug and release build')
    
    args = parser.parse_args()

    os.system('color') #enable ANSI colour codes

    f_IsSetupSuccessful = False

    if(args.debug):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        f_IsSetupSuccessful = run_cmake("debug")

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug, your CMake project should be good to go!", "green"))
        
    elif(args.release):

        if run_conan("Release"):   # >w>
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("release")

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for release, your CMake project should be good to go!", "green"))

    elif(args.both):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        if run_conan("Release"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("both")
        
        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug and release, your CMake project should be good to go!", "green"))
        
    else:
        print(CreateColouredText("No valid input was detected, nothing was done.", "red"))

    if f_IsSetupSuccessful:
        print(CreateColouredText("done!", "magenta"))
    