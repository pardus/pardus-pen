project('example', 'cpp')
# Source files
src = [
    'src/main.cpp',
    'src/DrawingWidget.cpp',
    'src/FloatingWidget.cpp',
    'src/WhiteBoard.cpp',
    'src/Button.cpp'
]

# Qt dependencies
qt_dep = []
if get_option('qt') == '5'
    qt_dep += [
        dependency('Qt5Core'),
        dependency('Qt5Gui'),
        dependency('Qt5Widgets'),
    ]
elif get_option('qt') == '6'
    qt_dep += [
        dependency('Qt6Core'),
        dependency('Qt6Gui'),
        dependency('Qt6Widgets'),
    ]
else
    error('Your qt version is ont supperted')
endif
# -fPIC required
add_project_arguments('-fPIC', language: 'cpp')
add_project_arguments('-fpermissive', language: 'cpp')
add_project_arguments('-I'+meson.current_source_dir()/'src/tools', language: 'cpp')

# executable file
executable('main', src, dependencies: qt_dep)
