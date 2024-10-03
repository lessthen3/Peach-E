import subprocess
import os
import argparse

def CreateColouredText(fp_SampleText: str, fp_DesiredColour: str) -> str:

    fp_SampleText = fp_SampleText.lower()

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
        print(CreateColouredText("[Warning]: no valid input detected for CreateColouredText, returned original text in all lower-case", "yellow"))
        return fp_SampleText
    
def run_conan(fp_BuildType: str) -> bool:

    try:
        subprocess.run(
            ['conan', 'install', '.', '-s', 'build_type=' + fp_BuildType, '-s', 'compiler.cppstd=20', '--output-folder=build', "--build=missing"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )

    except subprocess.CalledProcessError as err:
        print(CreateColouredText("[ERROR]: Failed to run Conan:", "red"))
        print(CreateColouredText(err.stdout.decode(), "yellow"))
        print(CreateColouredText(err.stderr.decode(), "yellow"))
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
        print(CreateColouredText("[ERROR]: Failed to run CMake:", "red"))
        print(CreateColouredText(err.stdout.decode(), "yellow"))
        print(CreateColouredText(err.stderr.decode(), "yellow"))
        return False

    return True



def main() -> bool:

    parser = argparse.ArgumentParser(description=CreateColouredText('Used for Building Peach-E from Source', 'magenta'))

    parser.add_argument(
        '--release', 
        action='store_true', 
        help=CreateColouredText('Used for a release build', 'cyan')
    )

    parser.add_argument(
        '--debug', 
        action='store_true', 
        help=CreateColouredText('Used for a debug build', 'cyan')
    )

    parser.add_argument(
        '--both', 
        action='store_true', 
        help=CreateColouredText('Used to build both a debug and release build', 'cyan')
    )

    parser.add_argument(
        '-G', 
        action='store_true', 
        help=CreateColouredText('Used to set the project file generator', 'cyan') + CreateColouredText('\n eg. -G vs2022', 'blue')
    )

    parser.add_argument(
        'vs2022',
        action='store_true',
        help=CreateColouredText('Generates solution for Visual Studio 17 2022, intended use:', 'cyan') + CreateColouredText('\n -G vs2022', 'blue')
    )

    parser.add_argument(
        'ninja',
        action='store_true',
        help=CreateColouredText('Generates project files using Ninja, intended use:', 'cyan') + CreateColouredText('\n -G ninja', 'blue')
    )
    
    args = parser.parse_args()

    f_IsSetupSuccessful = False

    f_DesiredGenerator = ""

    if(not args.debug or not args.release or not args.both):
        print(CreateColouredText("[ERROR]: No valid build type input detected, use -h or --help if you're unfamiliar", "red"))
        return False
        
    if(not args.G):
        print(CreateColouredText("[ERROR]: YOU DIDN'T USE -G FLAG BROTHER", "red"))
        return False

    if(args.vs2022):
        f_DesiredGenerator = "vs2022"
    elif(args.ninja):
        f_DesiredGenerator = "ninja"
    else:
        print(CreateColouredText("[ERROR]: Invalid Generator Selected, PICK A VALID GENERATOR", "red"))
        return False

    if(args.debug):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        f_IsSetupSuccessful = run_cmake("debug", f_DesiredGenerator)

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug, your CMake project should be good to go!", "green"))
        
    elif(args.release):

        if run_conan("Release"):   # >w>
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("release", f_DesiredGenerator)

        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for release, your CMake project should be good to go!", "green"))

    elif(args.both):

        if run_conan("Debug"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for debug", "green"))

        if run_conan("Release"):
            print(CreateColouredText("Conan setup and dependencies installation successfully completed for release", "green"))

        f_IsSetupSuccessful = run_cmake("both", f_DesiredGenerator)
        
        if f_IsSetupSuccessful:
            print(CreateColouredText("CMakeLists.txt succesfully read and compiled for debug and release, your CMake project should be good to go!", "green"))

    if not f_IsSetupSuccessful:
        return False

    print(CreateColouredText("done!", "magenta"))
    return True


if __name__ == "__main__":

    os.system('color') #enable ANSI colour codes

    if not main():
        print(CreateColouredText("nothing was done.", "yellow"))


#Rawr OwO