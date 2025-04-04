project(
    'c+text`',
    'cpp',
    version: 'i',
    default_options: [
        'cpp_std=c++23',
    ]
)

compile_flags = [
    '-Iinc',
    '-Wall',
    '-Wextra',
    '-O3'
    # '-Werror'
]

source = files(
    'src/command/handler.cpp',
    'src/command/logic.cpp',

    'src/cursor/renderer.cpp',
    'src/cursor/logic.cpp',

    'src/input/handler.cpp',
    'src/input/logic.cpp',

    'src/argument_parser.cpp',
    'src/logging_utility.cpp',
    'src/config_parser.cpp',
    'src/file_handler.cpp',
    'src/sdl_helper.cpp',
    'src/utilities.cpp',
    'src/editor.cpp',
    'src/main.cpp',
)

deps = [
    dependency('fontconfig'),
    dependency('SDL3_ttf'),
    dependency('SDL3'),
]

executable(
    'c+text',
    source,
    include_directories: include_directories('inc'),
    cpp_args: compile_flags,
    dependencies: deps
)