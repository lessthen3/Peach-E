import subprocess

def run_conan(build_type):
    subprocess.run([
        'conan', 'install', '.', '-s', 'build_type=Debug', '-s', 'compiler.cppstd=20', '--output-folder=build', "--build missing"
    ])

def run_cmake():
    subprocess.run([
        'cmake', '-S', '.', '-B', 'build'
    ])
    # subprocess.run([
    #     'cmake', '--build', 'build'
    # ])

if __name__ == "__main__":
    run_conan("Debug")
    #run_conan("Release")
    run_cmake()
