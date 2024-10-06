project(
    'testproject',
    'cpp',
    version: '0.1.0',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

include = include_directories('include')

src_files = files(
    'src/array/array.cpp',
    'src/array/kernel/kernels.cpp',
    'src/array/reference.cpp',
    'src/array/scalar.cpp',
    'src/dtype/dtype.cpp',
    'src/quantum_operator/kernel/dot.cpp',
    'src/quantum_operator/kernel/sum.cpp',
    'src/quantum_operator/quantum_operator.cpp',
)

quspin = static_library('quspin', src_files, include_directories: include)
shared_library('quspin', src_files, include_directories: include)

test_files = files(
    'test/array/constructor.cpp',
    'test/array/sum.cpp',
    'test/dtype.cpp',
)

foreach test_file : test_files
    test_name = test_file.full_path().strip('.cpp')
    exe_name = test_name.split('/')[-1]
    exe = executable(
        exe_name,
        test_file,
        link_with: quspin,
        include_directories: include,
    )
    test(test_name, exe)
endforeach

