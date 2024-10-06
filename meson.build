project(
    'libquspin',
    'cpp',
    version: '0.1.0',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

if host_machine.system() == 'windows'
    add_global_arguments('/bigobj', language: 'cpp')
endif

include = include_directories('libquspin/include')

src_files = files(
    'libquspin/src/array/array.cpp',
    'libquspin/src/array/kernel/kernels.cpp',
    'libquspin/src/array/reference.cpp',
    'libquspin/src/array/scalar.cpp',
    'libquspin/src/dtype/dtype.cpp',
    'libquspin/src/quantum_operator/kernel/dot.cpp',
    'libquspin/src/quantum_operator/kernel/sum.cpp',
    'libquspin/src/quantum_operator/quantum_operator.cpp',
)

libquspin = static_library('quspin', src_files, include_directories: include)
shared_library('quspin', src_files, include_directories: include)

test_files = files(
    'libquspin/test/array/constructor.cpp',
    'libquspin/test/array/sum.cpp',
    'libquspin/test/dtype.cpp',
)

foreach test_file : test_files
    test_name = test_file.full_path().strip('.cpp')
    exe_name = test_name.split('/')[-1]
    exe = executable(
        exe_name,
        test_file,
        link_with: libquspin,
        include_directories: include,
    )
    test(test_name, exe)
endforeach
