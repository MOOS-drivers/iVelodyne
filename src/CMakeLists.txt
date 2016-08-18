#===========================================================================
# FILE:  iVelodyne/src/CMakeLists.txt
# DATE:  2016/08/18
# INFO:  Source-level CMakeLists.txt file for the Velodyne driver project 
# NAME:  Mohamed Saad Ibn Seddik
#===========================================================================

FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

SET(SRC 
  Velodyne.cpp
  Velodyne_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(iVelodyne ${SRC})

TARGET_LINK_LIBRARIES(iVelodyne
  ${MOOS_LIBRARIES}
  # ${MOOSGeodesy_LIBRARIES}
  mbutil
  apputil
  m
  pthread
)

