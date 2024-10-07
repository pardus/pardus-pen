project('pardus-pen', ['cpp', 'c'])
# Source files
src = [
    'src/main.cpp',
    'src/DrawingWidget.cpp',
    'src/FloatingWidget.cpp',
    'src/FloatingSettings.cpp',
    'src/WhiteBoard.cpp',
    'src/Button.cpp',
    'src/SetupWidgets.cpp',
    'src/settings.c',
    'src/OverView.cpp',
    'src/which.c'
]

subdir('po')

if get_option('resources')
    # Build qrc
    run_command('rcc', './data/resources.qrc', '-o', meson.current_build_dir()/'resources.cpp', check:true)

    src += [meson.current_build_dir()/'resources.cpp']

endif
if get_option('fix1')
    add_project_arguments(['-Wno-error=template-id-cdtor', '-g3' ], language: 'cpp')
endif
add_project_arguments(['-O3', '-Wextra', '-Wall', '-Werror', '-s'], language: 'cpp')

if get_option('screenshot')
    add_project_arguments('-Dscreenshot', language: 'cpp')
    src += ['src/ScreenShot.cpp']
endif
# Qt dependencies
qt_dep = [
    dependency('gio-2.0'),
]
if get_option('etap19')
    add_project_arguments('-DETAP19', '-pthread',  language: 'cpp')
endif


if get_option('save')
    qt_dep += dependency('libarchive')
    add_project_arguments('-DLIBARCHIVE', language: 'cpp')
    src += 'src/Archive.cpp'
endif

if get_option('qt') == '5'
    add_project_arguments('-DQT5', language: 'cpp')
    qt_dep += [
        dependency('Qt5Core'),
        dependency('Qt5Gui'),
        dependency('Qt5Widgets'),
    ]
elif get_option('qt') == '6'
    add_project_arguments('-DQT6', language: 'cpp')
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

add_project_arguments('-DHISTORY='+get_option('history'), language: 'cpp')

glibdir = get_option('prefix')/'share/glib-2.0/schemas/'
icondir = get_option('prefix')/'share/icons/hicolor/scalable/apps'
desktopdir = get_option('prefix')/'share/applications'

# executable file
executable('pardus-pen', src, dependencies: qt_dep, install: true)
install_data('data/tr.org.pardus.pen.gschema.xml', install_dir : glibdir)
install_data('data/tr.org.pardus.pen.svg', install_dir : icondir)
install_data('data/tr.org.pardus.pen.desktop', install_dir : desktopdir)
install_data('data/tr.org.pardus.pen.appdata.xml', install_dir : get_option('prefix') / 'share/metainfo')
install_data('data/tr.org.pardus.pen.mime.xml', install_dir : get_option('prefix') / 'share/mime/packages/')
