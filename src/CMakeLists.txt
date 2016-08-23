#===========================================================================
# FILE:  iVelodyne/src/CMakeLists.txt
# DATE:  2016/08/18
# INFO:  Source-level CMakeLists.txt file for the Velodyne driver project
# NAME:  Mohamed Saad Ibn Seddik
#===========================================================================

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(LIBVELODYNE REQUIRED libvelodyne)
INCLUDE_DIRECTORIES(${LIBVELODYNE_INCLUDE_DIRS})
LINK_DIRECTORIES(${LIBVELODYNE_LIBRARY_PATH})


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

SET(SRC
  Velodyne.cpp
  Velodyne_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iVelodyne ${SRC})

TARGET_LINK_LIBRARIES(iVelodyne
  ${LIBVELODYNE_LIBRARIES}
  ${MOOS_LIBRARIES}
  # ${MOOSGeodesy_LIBRARIES}
  mbutil
  apputil
  m
  pthread
)
