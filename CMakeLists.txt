cmake_minimum_required(VERSION 3.5)

project(batteryctl)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets)

set(srcs core/battery.cpp
	 ui/mainwindow.cpp
	 ui/batteryicon.cpp
	 ui/chargethreshold.cpp
	 core/storage.cpp
	 main.cpp
	 ui/thinkpads_org_about.cpp
) 

add_executable(batteryctl ${srcs} resources.qrc)
target_link_libraries(batteryctl Qt5::Widgets)

install(TARGETS batteryctl RUNTIME DESTINATION bin)
install(FILES org.thinkpads.pkexec.batteryctl.policy DESTINATION /usr/share/polkit-1/actions)
install(FILES batteryctl.desktop DESTINATION /usr/share/applications)
install(FILES batteryctl.service DESTINATION /lib/systemd/system/)

set(CPACK_PACKAGE_VENDOR "Ognjen Galic")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 20)
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "\.git;\.idea;CMakeLists\.txt\.user;build;qbatteryctl.pro;qbatteryctl.pro.user")

include(CPack)

