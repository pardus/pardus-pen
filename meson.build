project('example', 'cpp')
# Source files
src = [
    'src/main.cpp',
    'src/DrawingWidget.cpp',
    'src/FloatingWidget.cpp',
    'src/WhiteBoard.cpp',
    'src/Button.cpp'
]

# Qt5 dependencies
qt5_dep = [
    dependency('Qt5Core'),
    dependency('Qt5Gui'),
    dependency('Qt5Widgets'),
]

# -fPIC required
add_project_arguments('-fPIC', language: 'cpp')
add_project_arguments('-fpermissive', language: 'cpp')
add_project_arguments('-I'+meson.current_source_dir()/'src/tools', language: 'cpp')

# executable file
executable('main', src, dependencies: qt5_dep)
