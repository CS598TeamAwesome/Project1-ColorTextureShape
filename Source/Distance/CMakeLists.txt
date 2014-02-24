set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/L1Distance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/L2Distance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ChiSquareDistance.cpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/KLDistance.cpp
    PARENT_SCOPE    
)

set(HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Distance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/L1Distance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/L2Distance.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ChiSquareDistance.hpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/KLDistance.hpp
    PARENT_SCOPE
)
