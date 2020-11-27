# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6.0)

# Declares and names the project.

project("mfplayer")


set(lib_name native-player)
set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs)


include_directories(include player audio decoder render)


link_directories(
        ${JNI_LIBS_DIR}/${ANDROID_ABI})


file(GLOB src_files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/player/*.cpp
        ${CMAKE_SOURCE_DIR}/decoder/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp
        ${CMAKE_SOURCE_DIR}/audio/*.cpp)


add_library( # Sets the name of the library.
        ${lib_name}

        SHARED

        ${src_files} )


# 第一种写法
set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil)

#z 库不加会报 找不到 uncompress / inflate / inflateEnd 等问题，导致编译不通过
set(native-libs
        android
        log
        z
        OpenSLES)


target_link_libraries( # Specifies the target library.
        ${lib_name}
        ${third-party-libs}
        ${native-libs})


# 第二种写法

#add_library(avformat-lib STATIC IMPORTED)
#set_target_properties(avformat-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.a)



#add_library(avcodec-lib STATIC IMPORTED)
#set_target_properties(avcodec-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.a)


#add_library(avutil-lib STATIC IMPORTED)
#set_target_properties(avutil-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.a)


#add_library(avfilter-lib STATIC IMPORTED)
#set_target_properties(avfilter-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.a)


#add_library(swresample-lib STATIC IMPORTED)
#set_target_properties(swresample-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.a)


#add_library(swscale-lib STATIC IMPORTED)
#set_target_properties(swscale-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.a)


#find_library(
#        log-lib
#        log )

#find_library(
#        android-lib
#        android )


#find_library(
#        z-lib
#        z )


#target_link_libraries(${lib_name}
#        ${log-lib}
#        ${android-lib}
#        ${z-lib}
#        ${avformat-lib}
#        ${avcodec-lib}
#        ${avfilter-lib}
#        ${swresample-lib}
#        ${swscale-lib}
#        ${avutil-lib})
