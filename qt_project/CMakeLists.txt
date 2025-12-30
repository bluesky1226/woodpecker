
cmake_minimum_required(VERSION 3.16)
project(Aerobic VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)
qt_standard_project_setup()

# Add your source file (e.g., main.cpp)
qt_add_executable(Aerobic
    enthalpy.cpp
    reactant.cpp
    qcustomplot.cpp
    adxl345.cpp
    rpi5gpiomgr.cpp
    coral.cpp
)

# Link the Qt6 libraries
target_link_libraries(Aerobic PRIVATE
    Qt6::Widgets
    Qt6::PrintSupport
    gpiodcxx
)

# =============================================================================================

# cmake_minimum_required(VERSION 3.16)
# project(Serene VERSION 1.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(/usr/include/aarch64-linux-gnu/qt6)
# include_directories(/usr/include/aarch64-linux-gnu/qt6/QtDataVisualization)

# # Find Qt6 components
# find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport DataVisualization)

# qt_standard_project_setup()

# # Add your source file (e.g., main.cpp)
# qt_add_executable(Serene
#     tower.cpp
#     Aurora.cpp
#     qcustomplot.cpp
#     adxl345.cpp
# )

# # Link the Qt6 libraries
# target_link_libraries(Serene PRIVATE
#     Qt6::Widgets
#     Qt6::PrintSupport
#     Qt6::DataVisualization
# )

# =============================================================================================

# cmake_minimum_required(VERSION 3.16)
# project(SensorDashboard VERSION 1.0 LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Find Qt6 components
# find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)
# qt_standard_project_setup()

# # Add your source file (e.g., main.cpp)
# qt_add_executable(SensorDashboard
#     main.cpp
#     mainwindow.cpp
#     qcustomplot.cpp
#     adxl345.cpp
#     rpi5gpiomgr.cpp
# )

# # Link the Qt6 libraries
# target_link_libraries(SensorDashboard PRIVATE
#     Qt6::Widgets
#     Qt6::PrintSupport
#     gpiodcxx
# )
