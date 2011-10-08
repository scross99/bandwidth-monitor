cmake_minimum_required(VERSION 2.8)
PROJECT(bandwidth_monitor)
FIND_PACKAGE(Qt4 REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}")

SET(bandwidth_monitor_gui_SOURCES common/Module.cpp gui/main.cpp gui/TrayIcon.cpp gui/Window.cpp)
SET(bandwidth_monitor_gui_HEADERS gui/TrayIcon.h gui/Window.h)
SET(bandwidth_monitor_gui_RESOURCES gui/systray.qrc)

QT4_WRAP_CPP(bandwidth_monitor_gui_HEADERS_MOC ${bandwidth_monitor_gui_HEADERS})
QT4_ADD_RESOURCES(bandwidth_monitor_gui_RESOURCES_RCC ${bandwidth_monitor_gui_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(bandwidth_monitor_gui ${bandwidth_monitor_gui_SOURCES} 
	${bandwidth_monitor_gui_HEADERS_MOC}
	${bandwidth_monitor_gui_RESOURCES_RCC})

TARGET_LINK_LIBRARIES(bandwidth_monitor_gui ${QT_LIBRARIES})

ADD_EXECUTABLE(bandwidth_monitor_cmd common/Module.cpp cmd/main.cpp)

INSTALL(TARGETS bandwidth_monitor_gui bandwidth_monitor_cmd RUNTIME DESTINATION bin)

