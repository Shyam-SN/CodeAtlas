find_package(Qt6 COMPONENTS Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Network QUIET)
endif()

if(Qt6_FOUND OR Qt5_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    add_executable(codeatlas_qt_app
        main.cpp
        main_window.cpp
        atlas_canvas.cpp
        property_inspector.cpp
        ai_panel.cpp
        security_panel.cpp
        llm_client.cpp
        embedding_client.cpp
        command_palette.cpp
    )

    target_link_libraries(codeatlas_qt_app PRIVATE codeatlas)

    if(Qt6_FOUND)
        target_link_libraries(codeatlas_qt_app PRIVATE Qt6::Widgets Qt6::Network)
    else()
        target_link_libraries(codeatlas_qt_app PRIVATE Qt5::Widgets Qt5::Network)
    endif()
    message(STATUS "Qt Desktop Visualizer enabled for CodeAtlas")
else()
    message(STATUS "Qt not found. Skipping CodeAtlas Qt GUI application.")
endif()
