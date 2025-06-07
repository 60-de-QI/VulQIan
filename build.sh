#!/bin/bash

# VulQIan Engine Build Script
# Usage: ./build.sh [build_type] [options]
# Example: ./build.sh Release --clean --tests --examples

set -e  # Exit on any error

# Default values
BUILD_TYPE="Release"
CLEAN_BUILD=false
BUILD_TESTS=true
BUILD_EXAMPLES=true
ENABLE_SHADERS=true
VERBOSE=false
JOBS=$(nproc)
BUILD_DIR="conan-build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show usage
show_usage() {
    echo "VulQIan Engine Build Script"
    echo ""
    echo "Usage: $0 [BUILD_TYPE] [OPTIONS]"
    echo ""
    echo "BUILD_TYPE:"
    echo "  Release     Build in Release mode (default)"
    echo "  Debug       Build in Debug mode"
    echo "  RelWithDebInfo  Release with debug info"
    echo "  MinSizeRel  Minimum size release"
    echo ""
    echo "OPTIONS:"
    echo "  --clean         Clean build directory before building"
    echo "  --no-tests      Disable building tests"
    echo "  --no-examples   Disable building examples"
    echo "  --no-shaders    Disable shader compilation"
    echo "  --verbose       Enable verbose output"
    echo "  --jobs N        Number of parallel jobs (default: $(nproc))"
    echo "  --help          Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                          # Release build with defaults"
    echo "  $0 Debug --clean            # Clean Debug build"
    echo "  $0 Release --no-tests       # Release without tests"
    echo "  $0 Debug --verbose --jobs 4 # Debug build, verbose, 4 jobs"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        Release|Debug|RelWithDebInfo|MinSizeRel)
            BUILD_TYPE="$1"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        --no-examples)
            BUILD_EXAMPLES=false
            shift
            ;;
        --no-shaders)
            ENABLE_SHADERS=false
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --jobs)
            JOBS="$2"
            shift 2
            ;;
        --help)
            show_usage
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Validate build type
case $BUILD_TYPE in
    Release|Debug|RelWithDebInfo|MinSizeRel)
        ;;
    *)
        print_error "Invalid build type: $BUILD_TYPE"
        show_usage
        exit 1
        ;;
esac

print_status "VulQIan Engine Build Configuration:"
echo "  Build Type: $BUILD_TYPE"
echo "  Build Directory: $BUILD_DIR"
echo "  Clean Build: $CLEAN_BUILD"
echo "  Build Tests: $BUILD_TESTS"
echo "  Build Examples: $BUILD_EXAMPLES"
echo "  Enable Shaders: $ENABLE_SHADERS"
echo "  Parallel Jobs: $JOBS"
echo "  Verbose: $VERBOSE"
echo ""

# Check if Vulkan SDK is set
if [[ -z "$VULKAN_SDK" ]]; then
    print_error "VULKAN_SDK environment variable is not set!"
    print_error "Please source your Vulkan SDK setup script first."
    exit 1
fi

print_status "Using Vulkan SDK: $VULKAN_SDK"

# Check for required tools
command -v conan >/dev/null 2>&1 || { print_error "conan is required but not installed. Aborting."; exit 1; }
command -v cmake >/dev/null 2>&1 || { print_error "cmake is required but not installed. Aborting."; exit 1; }

# Check conan version
CONAN_VERSION=$(conan --version | grep -oP '\d+\.\d+\.\d+' | head -1)
print_status "Using Conan version: $CONAN_VERSION"

# Clean build directory if requested
if [[ "$CLEAN_BUILD" == true ]]; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Set up conan profile if it doesn't exist
if ! conan profile list | grep -q "default"; then
    print_status "Creating default conan profile..."
    conan profile detect --force
fi

# Install conan dependencies
print_status "Installing Conan dependencies..."

CONAN_OPTIONS=""
if [[ "$BUILD_TESTS" == false ]]; then
    CONAN_OPTIONS="$CONAN_OPTIONS -o vulqian:build_tests=False"
fi

if [[ "$BUILD_EXAMPLES" == false ]]; then
    CONAN_OPTIONS="$CONAN_OPTIONS -o vulqian:build_examples=False"
fi

CONAN_CMD="conan install . --output-folder=$BUILD_DIR --build=missing -s build_type=$BUILD_TYPE"
if [[ -n "$CONAN_OPTIONS" ]]; then
    CONAN_CMD="$CONAN_CMD $CONAN_OPTIONS"
fi

if [[ "$VERBOSE" == true ]]; then
    print_status "Running: $CONAN_CMD"
fi

eval $CONAN_CMD

if [[ $? -ne 0 ]]; then
    print_error "Conan install failed!"
    exit 1
fi

print_success "Conan dependencies installed successfully"

# Configure with CMake
print_status "Configuring with CMake..."

CMAKE_OPTIONS=""
if [[ "$BUILD_TESTS" == true ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DVULQIAN_BUILD_TESTS=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DVULQIAN_BUILD_TESTS=OFF"
fi

if [[ "$BUILD_EXAMPLES" == true ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DVULQIAN_BUILD_EXAMPLE=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DVULQIAN_BUILD_EXAMPLE=OFF"
fi

if [[ "$ENABLE_SHADERS" == true ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_SHADER_COMPILATION=ON"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DENABLE_SHADER_COMPILATION=OFF"
fi

# Find the correct toolchain file path
TOOLCHAIN_FILE=""
if [[ -f "$BUILD_DIR/$BUILD_DIR/$BUILD_TYPE/generators/conan_toolchain.cmake" ]]; then
    TOOLCHAIN_FILE="$BUILD_DIR/$BUILD_DIR/$BUILD_TYPE/generators/conan_toolchain.cmake"
elif [[ -f "$BUILD_DIR/generators/conan_toolchain.cmake" ]]; then
    TOOLCHAIN_FILE="$BUILD_DIR/generators/conan_toolchain.cmake"
elif [[ -f "$BUILD_DIR/conan_toolchain.cmake" ]]; then
    TOOLCHAIN_FILE="$BUILD_DIR/conan_toolchain.cmake"
else
    print_error "Could not find conan_toolchain.cmake file!"
    print_error "Searched in:"
    print_error "  $BUILD_DIR/$BUILD_DIR/$BUILD_TYPE/generators/conan_toolchain.cmake"
    print_error "  $BUILD_DIR/generators/conan_toolchain.cmake"
    print_error "  $BUILD_DIR/conan_toolchain.cmake"
    exit 1
fi

print_status "Using toolchain file: $TOOLCHAIN_FILE"

# Check if CMake presets are available (CMake 3.23+)
CMAKE_VERSION=$(cmake --version | grep -oP '\d+\.\d+' | head -1)
CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d. -f2)

if [[ $CMAKE_MAJOR -gt 3 || ($CMAKE_MAJOR -eq 3 && $CMAKE_MINOR -ge 23) ]] && [[ -f "CMakeUserPresets.json" ]]; then
    # Use CMake preset if available
    PRESET_NAME="conan-$(echo $BUILD_TYPE | tr '[:upper:]' '[:lower:]')"
    if grep -q "\"$PRESET_NAME\"" CMakeUserPresets.json; then
        print_status "Using CMake preset: $PRESET_NAME"
        CMAKE_CMD="cmake --preset $PRESET_NAME"
        if [[ -n "$CMAKE_OPTIONS" ]]; then
            CMAKE_CMD="$CMAKE_CMD $CMAKE_OPTIONS"
        fi
    else
        print_warning "Preset $PRESET_NAME not found, using traditional cmake command"
        CMAKE_CMD="cmake . -B $BUILD_DIR -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DCMAKE_BUILD_TYPE=$BUILD_TYPE $CMAKE_OPTIONS"
    fi
else
    # Use traditional cmake command
    CMAKE_CMD="cmake . -B $BUILD_DIR -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE -DCMAKE_BUILD_TYPE=$BUILD_TYPE $CMAKE_OPTIONS"
fi

if [[ "$VERBOSE" == true ]]; then
    print_status "Running: $CMAKE_CMD"
fi

eval $CMAKE_CMD

if [[ $? -ne 0 ]]; then
    print_error "CMake configuration failed!"
    exit 1
fi

print_success "CMake configuration completed successfully"

# Build the project
print_status "Building VulQIan Engine..."

BUILD_CMD="cmake --build $BUILD_DIR --config $BUILD_TYPE -j $JOBS"

if [[ "$VERBOSE" == true ]]; then
    BUILD_CMD="$BUILD_CMD --verbose"
    print_status "Running: $BUILD_CMD"
fi

eval $BUILD_CMD

if [[ $? -ne 0 ]]; then
    print_error "Build failed!"
    exit 1
fi

print_success "Build completed successfully!"

# Run tests if enabled and built
if [[ "$BUILD_TESTS" == true ]]; then
    print_status "Running tests..."
    cd "$BUILD_DIR"
    if [[ "$VERBOSE" == true ]]; then
        ctest --output-on-failure -j $JOBS -C $BUILD_TYPE
    else
        ctest -j $JOBS -C $BUILD_TYPE
    fi
    if [[ $? -eq 0 ]]; then
        print_success "All tests passed!"
    else
        print_warning "Some tests failed"
    fi
    cd ..
fi

print_success "VulQIan Engine build process completed!"
print_status "Build artifacts are in: $BUILD_DIR"

# Show build summary
echo ""
echo "=== Build Summary ==="
echo "Build Type: $BUILD_TYPE"
echo "Build Directory: $BUILD_DIR"
if [[ -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    echo "CMake Generator: $(grep CMAKE_GENERATOR: "$BUILD_DIR/CMakeCache.txt" | cut -d= -f2)"
fi
echo "===================="
