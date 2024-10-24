project(
    'libquspin',
    'cpp',
    version: '0.1.0',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)

if host_machine.system() == 'windows'
    add_project_arguments('/bigobj', language: 'cpp')
elif host_machine.system() == 'darwin'
    add_project_arguments(
        '-arch',
        'x86_64',
        '-arch',
        'arm64',
        '-mmacosx-version-min=10.14',
        language: 'cpp',
    )
    add_project_link_arguments(
        '-arch',
        'x86_64',
        '-arch',
        'arm64',
        '-mmacosx-version-min=10.14',
        language: 'cpp',
    )
endif

add_global_arguments(
    '-DVERSION_INFO=' + meson.project_version(),
    language: 'cpp',
)

include = include_directories('include')

src_files = files(
    'src' / 'array' / 'array.cpp',
    'src' / 'array' / 'scalar.cpp',
    'src' / 'array' / 'reference.cpp',
    'src' / 'array' / 'kernel' / 'kernels.cpp',
    'src' / 'dtype' / 'dtype.cpp',
    'src' / 'quantum_operator' / 'kernel' / 'dot.cpp',
    'src' / 'quantum_operator' / 'kernel' / 'sum.cpp',
    'src' / 'quantum_operator' / 'quantum_operator.cpp',
)

if host_machine.system() == 'windows'
    libquspin = static_library('quspin', src_files, include_directories: include, name_suffix: 'lib')
else
    libquspin = shared_library('quspin', src_files, include_directories: include)
endif

libquspin_dep = declare_dependency(
    include_directories: include,
    link_with: libquspin,
)


test_files = [
    ('test' / 'array' / 'constructor.cpp'),
    ('test' / 'array' / 'sum.cpp'),
    ('test' / 'dtype.cpp'),
    ('test' / 'quantum_operator' / 'constructor.cpp'),
]

foreach test_file : test_files
    test_name = test_file.strip('.cpp').replace('/', '_').replace('\\', '_')
    exe = executable(test_name, test_file, dependencies: libquspin_dep)
    test(test_name, exe)
endforeach
