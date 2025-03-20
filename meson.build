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
    '-Wno-sign-compare',
    '-O3'
    # '-Werror'
]

source = files(
    'src/argument_parser.cpp',
    'src/command_handler.cpp',
    'src/cursor_renderer.cpp',
    'src/logging_utility.cpp',
    'src/command_logic.cpp',
    'src/config_parser.cpp',
    'src/input_handler.cpp',
    'src/cursor_logic.cpp',
    'src/file_handler.cpp',
    'src/input_logic.cpp',
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