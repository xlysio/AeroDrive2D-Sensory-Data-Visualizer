cmake_minimum_required(VERSION 3.19)
project(pc_application LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort Charts LinguistTools)

qt_standard_project_setup()

qt_add_executable(pc_application
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    serialhandler.h
    serialhandler.cpp
    chartwidget.h
    chartwidget.cpp
    gamewidget.h
    gamewidget.cpp
)

qt_add_resources(pc_application "flags"
    PREFIX "/"
    FILES
        resources/flag_british.png
        resources/flag_polish.png
)

qt_add_translations(pc_application
    TS_FILES
        translations/app_en.ts
        translations/app_pl.ts
)

target_link_libraries(pc_application
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SerialPort
        Qt6::Charts
)

install(TARGETS pc_application
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET pc_application
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
