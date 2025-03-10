project('pardus-pen', ['cpp', 'c'])
# Source files
src = [
    'src/main.cpp',
    'src/widgets/DrawingWidget.cpp',
    'src/utils/Selection.cpp',
    'src/utils/Geometry.cpp',
    'src/SetupWidgets.cpp',
    'src/utils/Settings.cpp',
    'src/widgets/OverView.cpp',
    'src/widgets/FloatingWidget.cpp',
    'src/widgets/FloatingSettings.cpp',
    'src/widgets/WhiteBoard.cpp',
    'src/widgets/Background.cpp',
    'src/widgets/Button.cpp',
    'src/tools/update.cpp',
    'src/tools/background.cpp',
    'src/tools/mainWindow.cpp',
    'src/tools/screenshot.cpp',
    'src/tools/save-load.cpp',
    'src/tools/pen.cpp',
    'src/utils/which.c'
]

subdir('po')

if get_option('resources')
    # Build qrc
    rcc_args = ['rcc', './data/resources.qrc', '-o', meson.current_build_dir()/'resources.cpp']
    if not get_option('etap19')
        rcc_args += ['-threshold=0' ,'--compress=9', '--compress-algo=zlib']
    endif
    run_command(rcc_args, check:true)

    src += [meson.current_build_dir()/'resources.cpp']

endif
if get_option('fix1')
    add_project_arguments(['-Wno-error=template-id-cdtor' ], language: 'cpp')
endif
add_project_arguments(['-Wextra', '-Wall', '-Werror', '-g3'], language: 'cpp')

if get_option('screenshot')
    add_project_arguments('-Dscreenshot', language: 'cpp')
    src += ['src/utils/ScreenShot.cpp']
endif
# Qt dependencies
qt_dep = []

link_args = []

if get_option('etap19')
    add_project_arguments('-DETAP19', '-pthread',  language: 'cpp')
    link_args += ['-lpthread']
endif


if get_option('save')
    qt_dep += dependency('libarchive')
    add_project_arguments('-DLIBARCHIVE', language: 'cpp')
    src += 'src/utils/Archive.cpp'
endif


# background images
add_project_arguments('-DBGDIR="' + get_option('backgrounds') + '"', language: 'cpp')
foreach bg_src : run_command('find', 'data/backgrounds','-type','f',check:true).stdout().strip().split('\n')
    install_data(bg_src, install_dir: get_option('backgrounds'))
endforeach



if get_option('qt') == '5'
    add_project_arguments('-DQT5', language: 'cpp')
    qt_dep += [
        dependency('Qt5Core'),
        dependency('Qt5Gui'),
        dependency('Qt5Widgets'),
        dependency('Qt5Svg'),
    ]
    if get_option('pdf')
        add_project_arguments('-DQPRINTER', language: 'cpp')
        qt_dep += dependency('Qt5PrintSupport')
    endif
elif get_option('qt') == '6'
    add_project_arguments('-DQT6', language: 'cpp')
    qt_dep += [
        dependency('Qt6Core'),
        dependency('Qt6Gui'),
        dependency('Qt6Widgets'),
        dependency('Qt6Svg'),
    ]
    if get_option('pdf')
        add_project_arguments('-DQPRINTER', language: 'cpp')
        qt_dep += dependency('Qt6PrintSupport')
    endif
else
    error('Your qt version is ont supperted')
endif
# -fPIC required
add_project_arguments('-fPIC', language: 'cpp')
add_project_arguments('-fpermissive', language: 'cpp')
add_project_arguments('-I'+meson.current_source_dir()/'src/tools', language: 'cpp')

icondir = get_option('prefix')/'share/icons/hicolor/scalable/apps'
desktopdir = get_option('prefix')/'share/applications'

# executable file
executable('pardus-pen', src, dependencies: qt_dep, link_args: link_args, install: true)
install_data('data/tr.org.pardus.pen.svg', install_dir : icondir)
install_data('data/tr.org.pardus.pen.desktop', install_dir : desktopdir)
install_data('data/tr.org.pardus.pen.appdata.xml', install_dir : get_option('prefix') / 'share/metainfo')
install_data('data/tr.org.pardus.pen.mime.xml', install_dir : get_option('prefix') / 'share/mime/packages/')
