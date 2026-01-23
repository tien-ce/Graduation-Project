cmake_minimum_required (VERSION 3.28.3)
# Use GLOB_RECURSE to search recursively (due to the ** pattern)
# Step 1: Use GLOB to find the top-level component *directories*.
# Note: We set LIST_DIRECTORIES TRUE and use a non-recursive pattern to get 
file(GLOB COMPONENT_BASE_DIRS
    LIST_DIRECTORIES TRUE
    # Non-recursive glob to match PDE and Read_Write_Matrix folders
    "../../Components/**" 
)



project(msg_rec) # Explicitly specify language for clarity

# Define the executable target
add_executable(msg_rec receive.c)
add_executable(msg_send send.c)

# 1. Include Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_include_directories(msg_rec
    PRIVATE "${DIR}/Include" 
    )
    target_include_directories(msg_send
    PRIVATE "${DIR}/Include" 
    )
endforeach()

# 2. Link Directories
foreach(DIR ${COMPONENT_BASE_DIRS})
    # Append the "Include" subdirectory path to our final list
    target_link_directories(msg_rec 
        PRIVATE "${DIR}/build" 
    )
    target_link_directories(msg_send 
        PRIVATE "${DIR}/build" 
    )
endforeach()


# 3. Link Libraries
target_link_libraries(msg_rec PRIVATE msg_pa)
target_link_libraries(msg_send PRIVATE msg_pa)
