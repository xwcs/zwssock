####### ZWSSOCK
PROJECT(zwssock)
## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

if(XW_BUILD_ZWSSOCK)
set(CMAKE_INSTALL_PREFIX "${XW_3RDP_ROOT}")
endif() 

set (ZMQ_TOOL_SET "v140")
set (XW_ZEROMQ_VERSION "4_2_0")
set (BOOT_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (ZEROMQ_INCLUDE_DIRS "${BOOT_PREFIX}/include" CACHE STRING "Where to search zmq headers." FORCE )
set (ZEROMQ_LIBRARIES "${BOOT_PREFIX}/lib/libzmq-${ZMQ_TOOL_SET}-mt-${XW_ZEROMQ_VERSION}.lib" CACHE STRING "zmq libraries." FORCE )


set (TRERDP_ROOT "${CMAKE_INSTALL_PREFIX}")
set (ZWSSOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set (ZLIB_ROOT "${TRERDP_ROOT}")


#########################################################
#
#       string macros 
#
########################################################

# Replace slashes by backslashes in a string.
macro (slashes_to_backslashes output input)
    string (REGEX REPLACE "/" "\\\\" ${output} ${input})
endmacro ()

# Filter a list of items using a regular expression.
macro (filter_list output_list input_list regex)
    foreach (item ${input_list})
        if (${item} MATCHES ${regex})
            list (APPEND ${output_list} ${item})
        endif ()
    endforeach ()
endmacro ()

# Convert a semicolon-separated list to a whitespace-separated string.
macro (convert_list_to_string output_string input_list)
    foreach (item ${input_list})
        if (DEFINED ${output_string})
            set (${output_string} "${${output_string}} ${item}")
        else ()
            set (${output_string} "${item}")
        endif ()
    endforeach ()
endmacro ()

# Assign a whitespace-separated string to a variable, given a list.
macro (set_to_string output_variable first_arg)
    set (arg_list ${first_arg} ${ARGN})
    convert_list_to_string (${output_variable} "${arg_list}")
endmacro ()


if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	
	# All configurations.
	set (preprocessor_definitions_common
		WIN32
		_WIN64
		_CRT_SECURE_NO_WARNINGS
		_CRT_NONSTDC_NO_DEPRECATE
		_SCL_SECURE_NO_WARNINGS		
	)
	
else()
	# All configurations.
	set (preprocessor_definitions_common
		WIN32
		_USE_32BIT_TIME_T
		_CRT_SECURE_NO_WARNINGS
		_CRT_NONSTDC_NO_DEPRECATE
		_SCL_SECURE_NO_WARNINGS			
	)

endif()



# Debug configuration.
set (preprocessor_definitions_debug
)

# Release configuration.
set (preprocessor_definitions_release
)




#--------------------------------------------------------------------------------------------------
# Compilation/linking flags.
#--------------------------------------------------------------------------------------------------

# All configurations.
set (compiler_flags_common
    /MP                                     # Multi-processor Compilation
    /GF                                     # Enable String Pooling

)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (compiler_flags_common
        ${compiler_flags_common}
		/W4
		/WX-
		/Zp8
		
    )
else ()
    set (compiler_flags_common
        ${compiler_flags_common}
        /W3                                 # Treat Warnings 3
		/WX-
		/Zp4
    )
endif ()

set (exe_linker_flags_common
    /WX										# Treat Warnings as errors
)
set (shared_lib_linker_flags_common
    ${exe_linker_flags_common}
    /OPT:NOREF
)

# Debug configuration.
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (compiler_flags_debug
        /Zi		# set Debug Information Format to Program Database
		-wd4820 -wd4668 -wd4121 -wd4100 -wd4640 -wd4127 -wd4710 -wd4548 -wd4480 -wd4505 -wd4201 -wd4512 -wd4189 -wd4706 
    )
else ()
    set (compiler_flags_debug
        /ZI                                 # set Debug Information Format to Program Database for Edit & Continue
		-wd4820 -wd4668 -wd4121 -wd4100 -wd4640 -wd4127 -wd4710 -wd4548 -wd4480 -wd4505 -wd4201 -wd4512 -wd4189 -wd4706 
    )
endif ()

set (compiler_flags_debug
    ${compiler_flags_debug}
    /MDd                                    # set Runtime Library to Multi-threaded Debug DLL
)

# Release configuration.
set (compiler_flags_release
    /Zi                                     # set Debug Information Format to Program Database
    /Ox                                     # Full Optimization
    /Ob2                                    # set Inline Function Expansion to Any Suitable
    /Oi                                     # Enable Intrinsic Functions
    /Ot                                     # Favor Fast Code
    /Oy                                     # Omit Frame Pointers
    /MD                                     # set Runtime Library to Multi-threaded DLL
	/EHsc
)
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set (compiler_flags_release
        ${compiler_flags_release}
        /fp:fast                            # set Floating Point Model to Fast, in 32-bit builds only because of a bug in VS 2010:
    )                                       # http://connect.microsoft.com/VisualStudio/feedback/details/518015/nan-comparison-under-the-64-bit-compiler-is-incorrect
endif ()


# Release configuration for warnings.
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set (compiler_flags_release
        ${compiler_flags_release}
        -wd4820 -wd4668 -wd4121 -wd4100 -wd4640 -wd4127 -wd4710 -wd4548 -wd4480 -wd4505 -wd4201 -wd4512 -wd4189 -wd4706 
    )
else ()
    set (compiler_flags_release
        ${compiler_flags_release}                               # set Debug Information Format to Program Database for Edit & Continue
		-wd4820 -wd4668 -wd4121 -wd4100 -wd4640 -wd4127 -wd4710 -wd4548 -wd4480 -wd4505 -wd4201 -wd4512 -wd4189 -wd4706 
    )
endif ()


set (exe_linker_flags_release
)
set (shared_lib_linker_flags_release
    ${exe_linker_flags_release}
)

# Ship configuration.
set (compiler_flags_ship
    /GL                                     # Enable link-time code generation
)
set (exe_linker_flags_ship
    /LTCG                                   # Use Link Time Code Generation
    /INCREMENTAL:NO                         # Disable Incremental Linking
)
set (shared_lib_linker_flags_ship
    ${exe_linker_flags_ship}
)

# Profile configuration.
set (exe_linker_flags_profile
    /DEBUG                                  # Generate Debug Info
)
set (shared_lib_linker_flags_profile
    ${exe_linker_flags_profile}
)


# All configurations.
set (preprocessor_definitions_common
    ${preprocessor_definitions_common}
)
if (CMAKE_SIZEOF_VOID_P MATCHES 4)
    set (preprocessor_definitions_common
        ${preprocessor_definitions_common}
        ARCH32
)
elseif (CMAKE_SIZEOF_VOID_P MATCHES 8)
    set (preprocessor_definitions_common
        ${preprocessor_definitions_common}
        ARCH64
)
else ()
    message (FATAL_ERROR "Cannot determine pointer size")
endif ()

# Debug configuration.
set (preprocessor_definitions_debug
    ${preprocessor_definitions_debug}
    ${preprocessor_definitions_common}
    DEBUG
    _DEBUG
)

# Release configuration.
set (preprocessor_definitions_release
    ${preprocessor_definitions_release}
    ${preprocessor_definitions_common}
    NDEBUG
)


# A convenience macro to apply the preprocessor definitions to all configurations of a given target.
macro (apply_preprocessor_definitions target)
    set_property (TARGET ${target} APPEND PROPERTY
        #  COMPILE_DEFINITIONS_DEBUG ${preprocessor_definitions_debug}
        COMPILE_DEFINITIONS $<$<CONFIG:Debug>:${preprocessor_definitions_debug}>
    )
    set_property (TARGET ${target} APPEND PROPERTY
        # COMPILE_DEFINITIONS_RELEASE ${preprocessor_definitions_release}
        COMPILE_DEFINITIONS $<$<CONFIG:Release>:${preprocessor_definitions_release}>
    )    
endmacro ()


#--------------------------------------------------------------------------------------------------
# Compilation/linking flags.
#--------------------------------------------------------------------------------------------------

# All configurations.
set (compiler_flags_common
    ${compiler_flags_common}
)
set (exe_linker_flags_common
    ${exe_linker_flags_common}
)
set (shared_lib_linker_flags_common
    ${shared_lib_linker_flags_common}
)

# Debug configuration.
set (compiler_flags_debug
    ${compiler_flags_debug}
    ${compiler_flags_common}
)
set (exe_linker_flags_debug
    ${exe_linker_flags_debug}
    ${exe_linker_flags_common}
)
set (shared_lib_linker_flags_debug
    ${shared_lib_linker_flags_debug}
    ${shared_lib_linker_flags_common}
)

# Release configuration.
set (compiler_flags_release
    ${compiler_flags_release}
    ${compiler_flags_common}
)
set (exe_linker_flags_release
    ${exe_linker_flags_release}
    ${exe_linker_flags_common}
)
set (shared_lib_linker_flags_release
    ${shared_lib_linker_flags_release}
    ${shared_lib_linker_flags_common}
)



# Apply Debug configuration compilation/linking flags.
convert_list_to_string (CMAKE_CXX_FLAGS_DEBUG "${compiler_flags_debug}")
convert_list_to_string (CMAKE_EXE_LINKER_FLAGS_DEBUG "${exe_linker_flags_debug}")
convert_list_to_string (CMAKE_SHARED_LINKER_FLAGS_DEBUG "${shared_lib_linker_flags_debug}")

# Apply Release configuration compilation/linking flags.
convert_list_to_string (CMAKE_CXX_FLAGS_RELEASE "${compiler_flags_release}")
convert_list_to_string (CMAKE_EXE_LINKER_FLAGS_RELEASE "${exe_linker_flags_release}")
convert_list_to_string (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${shared_lib_linker_flags_release}")



link_directories("${TRERDP_ROOT}/lib")



IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  target_link_libraries(zwssock Ws2_32)
ENDIF()


#set (TRERDP_ROOT "${CMAKE_INSTALL_PREFIX}")
#set (ZWSSOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories (${TRERDP_ROOT}/include)
INCLUDE_DIRECTORIES(${ZWSSOCK_DIR})


FILE(GLOB zwsock_files ${ZWSSOCK_DIR}/*.cpp)

ADD_LIBRARY (zwssock SHARED ${zwsock_files})


########################################################################
# ZMQ dependency
########################################################################
include_directories(${ZEROMQ_INCLUDE_DIRS})
target_link_libraries(zwssock czmq ${ZEROMQ_LIBRARIES})
			

apply_preprocessor_definitions (zwssock)

#if builded inside xwee src tree use also this
if(XW_BUILD_ZWSSOCK)
    set_bindir_properties(zwssock)
endif()


FILE(GLOB zwssock_headers ${ZWSSOCK_DIR}/*.h)
INSTALL(FILES ${zwssock_headers} DESTINATION ${TRERDP_ROOT}/include)

INSTALL(TARGETS zwssock
		ARCHIVE DESTINATION  "${TRERDP_ROOT}/lib/" CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
		LIBRARY DESTINATION  "${TRERDP_ROOT}/lib/" CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
		RUNTIME DESTINATION  "${TRERDP_ROOT}/bin/" CONFIGURATIONS Debug Release MinSizeRel RelWithDebInfo
		)