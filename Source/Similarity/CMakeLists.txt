set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/CosineSimilarity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HistogramIntersection.cpp
    PARENT_SCOPE
)

set(HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Similarity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CosineSimilarity.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/HistogramIntersection.hpp
    PARENT_SCOPE
)
