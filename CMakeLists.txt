project(main)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ZLIB REQUIRED)
add_executable(main
    main.cpp
    src/pdfparser.h src/pdfparser.cpp
    src/custombuffer/custombuffer.hpp src/custombuffer/custombuffer.cpp
    src/customfunctions/customfunction.h src/customfunctions/customfunction.cpp
)

target_link_libraries(main
    ZLIB::ZLIB
)
