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
        '-mmacosx-version-min=11.1',
        language: 'cpp',
    )
    add_project_link_arguments(
        '-arch',
        'x86_64',
        '-arch',
        'arm64',
        '-flto',
        '-mmacosx-version-min=11.1',
        language: 'cpp',
    )
elif host_machine.system() == 'linux'
    add_project_link_arguments('-lpthread', language: 'cpp')
endif

add_global_arguments(
    '-DVERSION_INFO=' + meson.project_version(),
    language: 'cpp',
)

include = include_directories('include')

boost = dependency('boost')

src_files = files(
    'src' / 'array' / 'array.cpp',
    'src' / 'array' / 'details' / 'array.cpp',
    'src' / 'array' / 'details' / 'pointer.cpp',
    'src' / 'array' / 'details' / 'iterator.cpp',
    'src' / 'array' / 'scalar.cpp',
    'src' / 'array' / 'reference.cpp',
    'src' / 'array' / 'kernels.cpp',
    'src' / 'dtype' / 'dtype.cpp',
    'src' / 'qmatrix' / 'kernel' / 'dot.cpp',
    'src' / 'qmatrix' / 'kernel' / 'sum.cpp',
    'src' / 'qmatrix' / 'qmatrix.cpp',
    'src' / 'qmatrix' / 'details' / 'qmatrix.cpp',
    'src' / 'qmatrix' / 'details' / 'iterator.cpp',
)

if host_machine.system() == 'windows'
    libquspin = static_library(
        'quspin',
        src_files,
        include_directories: include,
        name_suffix: 'lib',
        dependencies: boost,
    )
else
    libquspin = shared_library(
        'quspin',
        src_files,
        include_directories: include,
        dependencies: boost,
    )
endif

libquspin_dep = declare_dependency(
    include_directories: include,
    link_with: libquspin,
)


test_files = [
    ('test' / 'array' / 'iterator.cpp'),
    ('test' / 'array' / 'constructor.cpp'),
    ('test' / 'array' / 'kernels.cpp'),
    ('test' / 'dtype.cpp'),
    ('test' / 'qmatrix' / 'constructor.cpp'),
    ('test' / 'qmatrix' / 'sum.cpp'),
    ('test' / 'details' / 'select.cpp'),
    ('test' / 'details' / 'threading.cpp'),
]

foreach test_file : test_files
    test_name = test_file.strip('.cpp').replace('/', '_').replace('\\', '_')
    exe = executable(test_name, test_file, dependencies: libquspin_dep)
    test(test_name, exe)
endforeach


find_prog = find_program('find', required: false)
run_find_prog = find_program('scripts/run-find.sh')
clang_tidy_prog = find_program('clang-tidy', required: false)
run_clang_tidy_prog = find_program('scripts/run-clang-tidy.sh')
clang_format_prog = find_program('clang-format', required: false)
run_clang_format_prog = find_program('scripts/run-clang-format.sh')


if host_machine.system() != 'windows' and find_prog.found() and run_find_prog.found()
    all_files = run_command(run_find_prog, check: true, capture: true).stdout().split(
        '\n',
    )

    if clang_format_prog.found() and all_files.length() > 0
        format_deps = []
        foreach path : all_files
            if path == ''
                continue
            endif
            stamp = 'format__' + path.replace('/', '__')
            message('formatter found ' + path)
            format_deps += custom_target(
                stamp,
                input: path,
                output: stamp,
                command: [
                    run_clang_format_prog,
                    clang_format_prog,
                    '@OUTPUT@',
                    '--style=file',
                    '-i',
                    '@INPUT@',
                ],
            )
        endforeach
        alias_target('format', format_deps)
    endif

    if clang_tidy_prog.found() and all_files.length() > 0
        tidy_deps = []
        foreach path : all_files
            if path == ''
                continue
            endif
            message('tidy found ' + path)

            stamp = 'tidy__' + path.replace('/', '__')
            tidy_deps += custom_target(
                stamp,
                input: path,
                output: stamp,
                command: [
                    run_clang_tidy_prog,
                    clang_tidy_prog,
                    '@OUTPUT@',
                    '--use-color',
                    '--quiet',
                    '-p',
                    meson.project_build_root(),
                    '@INPUT@',
                ],
            )
        endforeach
        alias_target('tidy', tidy_deps)
    endif

endif
