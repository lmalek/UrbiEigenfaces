set(URBI_ROOT_DIR "/usr" CACHE PATH "Urbi root directory.")

find_package (Urbi REQUIRED)
find_package (OpenCV REQUIRED)
find_package (Boost REQUIRED)

link_directories (${BOOST_LIBRARYDIR})

if (WIN32)
  add_definitions( -DBOOST_ALL_DYN_LINK )
endif (WIN32)

include_directories (${URBI_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_library (UEigenfaces MODULE UEigenfaces.cpp eigenfaces.cpp helper.cpp)

target_link_libraries (UEigenfaces ${URBI_LIBRARIES} ${OpenCV_CV_LIBRARY} ${Boost_LIBRARIES})

set_target_properties (UEigenfaces PROPERTIES
  VERSION 0.0.1
  SOVERSION 0.0.1
  PREFIX "")

#install (TARGETS UEigenfaces  DESTINATION lib/gostai/uobjects COMPONENT libraries)
