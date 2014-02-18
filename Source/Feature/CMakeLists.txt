set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/HistogramFeature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorHistogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalBinaryPattern.cpp
    PARENT_SCOPE
)

set(HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/HistogramFeature.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ColorHistogram.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LocalBinaryPattern.hpp
    PARENT_SCOPE
)
