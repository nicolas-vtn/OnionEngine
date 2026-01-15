# cmake/FetchAssimp.cmake
# ---------------------------------------------------------------------------
# Fetch and make Assimp available as a CMake target `assimp`
# ---------------------------------------------------------------------------

# Prevent double inclusion
if (TARGET assimp)
    return()
endif()

include(FetchContent)

# Disable everything not needed for a runtime engine
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES     OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT         ON  CACHE BOOL "" FORCE)

# Optional: reduce formats if you want faster builds
# set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_OBJ_IMPORTER  ON CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1
)

FetchContent_MakeAvailable(assimp)
