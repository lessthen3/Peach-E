import subprocess

def run_conan(build_type):
    try:
        subprocess.run(
            ['conan', 'install', '.', '-s', 'build_type=' + build_type, '-s', 'compiler.cppstd=20', '--output-folder=build', "--build=missing"],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    except subprocess.CalledProcessError as e:
        print("Failed to run Conan:")
        print(e.stdout.decode())
        print(e.stderr.decode())

def run_cmake():
    try:
        subprocess.run(
            ['cmake', '-S', '.', '-B', 'build'],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        subprocess.run(
            ['cmake', '--build', 'build'],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
    except subprocess.CalledProcessError as e:
        print("Failed to run CMake:")
        print(e.stdout.decode())
        print(e.stderr.decode())

if __name__ == "__main__":
    run_conan("Debug")
    #run_conan("Release")   >w>
    run_cmake()
