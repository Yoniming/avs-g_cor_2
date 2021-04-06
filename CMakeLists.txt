cmake_minimum_required(VERSION 3.6)
project(AVS_api)

#INCLUDE_DIRECTORIES(/Users/berry/anaconda3/include)
#LINK_DIRECTORIES(/Users/berry/anaconda3/lib)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wno-unused-function -g -O2 -DHAVE_PTHREAD -DUSE_MALLOC_WRAPPERS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wno-sign-compare -std=c++0x")


set(MAIN_SOURCE_FILES
    main.cpp
    BufPool.cpp
    CommandParse.cpp
    Encap.cpp
    EncodeSEWorker.cpp
    EncodePEWorker.cpp
    EncodeWorker.cpp
    FastqRead.cpp
    NameProcess.cpp
    Param.cpp
    QualProcess.cpp
    AcoCompress.cpp
    SeqLenProcess.cpp
    SeqProcess.cpp
    ThreadPool.cpp
    util.cpp
    DecodeWorker.cpp
    DecodeSEWorker.cpp
    DecodePipeWorker.cpp
    DecodePEWorker.cpp
    ArcFile.cpp
    Context.cpp
    BlockParamProcess.cpp
    ExtractWorker.cpp
    NameLenProcess.cpp
)

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    add_definitions(-DHAVE_ZLIB)
    include_directories(${ZLIB_INCLUDE_DIRS})
endif()

add_executable(AVS_api ${MAIN_SOURCE_FILES})
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(AVS_api m pthread ${ZLIB_LIBRARIES}  rt)
else()
    target_link_libraries(AVS_api m pthread ${ZLIB_LIBRARIES} )
endif()
