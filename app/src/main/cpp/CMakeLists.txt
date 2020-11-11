# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.6.0)

# Declares and names the project.

project("mfplayer")


set(lib_name native-player)
set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/../jniLibs)


include_directories(include)


link_directories(
        ${JNI_LIBS_DIR}/${ANDROID_ABI})


file(GLOB src_files
        ${CMAKE_SOURCE_DIR}/*.cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ${lib_name}

        SHARED

        ${src_files} )

#add_library(avformat-lib STATIC IMPORTED)
#
#set_target_properties(avformat-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.a)
#
#
#add_library(avutil-lib STATIC IMPORTED)
#
#set_target_properties(avutil-lib
#        PROPERTIES
#        IMPORTED_LOCATION ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.a)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library(
#        log-lib
#        log )
#
#
#find_library(
#        android-lib
#        android )


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
        z)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        ${lib_name}
                        ${third-party-libs}
                        ${native-libs})