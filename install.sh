#!/bin/sh

# =============================================================================
# SPANE GAME ENGINE - INSTALLATION SCRIPT
# =============================================================================

# PROJECT BASIC INFO
PROJECT_NAME="Spane"
PROJECT_DESCRIPTION="SPANE Game Engine - Multi-Game Platform"

# INSTALLATION PATHS
INSTALL_DIR="/usr/local/etc/$PROJECT_NAME"
BIN_DIR="/usr/local/bin"
GAMES_DIR="$INSTALL_DIR/games"

# SOURCE PATHS
REPO_DIR=$(pwd)
MAIN_SOURCE_DIR="$REPO_DIR"

# =============================================================================
# BUILD CONFIGURATION
# =============================================================================
MAIN_FILE="Spane.c"
BINARY_NAME="spane"
BUILD_DIR="/tmp/spane_build_$$"

# =============================================================================
# FUNCTION DEFINITIONS
# =============================================================================

log_message() {
    message="$1"
    timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    echo "[$timestamp] $message"
}

# Check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if a package is installed (dpkg based)
package_installed() {
    dpkg -l "$1" 2>/dev/null | grep -q "^ii"
}

# Install package with fallback: direct → apt update → retry
install_package() {
    package="$1"
    
    log_message "Installing $package..."
    
    # First attempt: direct install without update
    if sudo apt-get install -y "$package" 2>/dev/null; then
        log_message "✓ Installed $package (direct)"
        return 0
    fi
    
    # Second attempt: update and retry
    log_message "Direct install failed, updating package lists..."
    if sudo apt-get update -y 2>/dev/null && sudo apt-get install -y "$package" 2>/dev/null; then
        log_message "✓ Installed $package (after update)"
        return 0
    fi
    
    log_message "✗ Failed to install $package"
    return 1
}

# Verify and install build dependencies
install_dependencies() {
    log_message "Checking build dependencies..."
    
    # Check GCC
    if command_exists gcc; then
        log_message "✓ GCC found: $(gcc --version | head -n1)"
    else
        log_message "GCC not found, installing..."
        install_package "gcc" || {
            log_message "Error: Failed to install GCC"
            exit 1
        }
    fi
    
    # Check libx11-dev
    if package_installed "libx11-dev"; then
        log_message "✓ libx11-dev found"
    else
        log_message "libx11-dev not found, installing..."
        install_package "libx11-dev" || {
            log_message "Error: Failed to install libx11-dev"
            exit 1
        }
    fi
}

# Check if a C file includes X11 headers
needs_x11() {
    c_file="$1"
    grep -q "#include.*<X11/" "$c_file" 2>/dev/null
    return $?
}

# Check if a C file has the create_game symbol (is a game shared library)
is_game_library() {
    c_file="$1"
    grep -q "Game\* create_game" "$c_file" 2>/dev/null
    return $?
}

# Check if a C file has main function (is an executable)
has_main() {
    c_file="$1"
    grep -q "int main\s*(" "$c_file" 2>/dev/null
    return $?
}

# Compile the main engine
compile_engine() {
    local engine_file="$1"
    
    log_message "Compiling engine: $engine_file"
    
    # Detect X11 flags
    X11_CFLAGS=""
    X11_LDFLAGS=""
    if pkg-config --exists x11 2>/dev/null; then
        X11_CFLAGS=$(pkg-config --cflags x11 2>/dev/null)
        X11_LDFLAGS=$(pkg-config --libs x11 2>/dev/null)
    else
        X11_CFLAGS="-I/usr/include/X11"
        X11_LDFLAGS="-L/usr/lib/x86_64-linux-gnu -lX11"
    fi
    
    cd "$BUILD_DIR" || exit 1
    
    # Compile engine
    gcc -O3 -march=native -pipe -flto -fomit-frame-pointer \
        $X11_CFLAGS \
        -c "$engine_file" \
        -o "$BUILD_DIR/spane_engine.o" 2>&1
    
    if [ $? -ne 0 ]; then
        log_message "✗ Failed to compile engine"
        return 1
    fi
    
    # Link engine
    gcc -O3 -flto \
        "$BUILD_DIR/spane_engine.o" \
        $X11_LDFLAGS \
        -lm -ldl -lpthread \
        -o "$BUILD_DIR/$BINARY_NAME" 2>&1
    
    if [ $? -ne 0 ]; then
        log_message "✗ Failed to link engine"
        return 1
    fi
    
    strip "$BUILD_DIR/$BINARY_NAME" 2>/dev/null || true
    log_message "✓ Engine compiled successfully"
    return 0
}

# Compile all game .so files
compile_games() {
    log_message "Compiling game libraries..."
    
    # Find all game files (files with create_game but not main)
    local games_dir="$MAIN_SOURCE_DIR/games"
    mkdir -p "$BUILD_DIR/games"
    
    if [ ! -d "$games_dir" ]; then
        mkdir -p "$games_dir"
        log_message "Created games directory: $games_dir"
    fi
    
    local compiled=0
    local failed=0
    
    for game_file in "$games_dir"/*.c; do
        [ ! -f "$game_file" ] && continue
        
        local game_name=$(basename "$game_file" .c)
        log_message "  Building game: $game_name"
        
        # Compile shared library - NO main function needed
        if gcc -shared -fPIC -O3 -march=native \
            -o "$BUILD_DIR/games/${game_name}.so" \
            "$game_file" 2>&1; then
            log_message "  ✓ $game_name.so built"
            compiled=$((compiled + 1))
        else
            log_message "  ✗ Failed to build $game_name"
            failed=$((failed + 1))
        fi
    done
    
    log_message "Games compiled: $compiled successful, $failed failed"
    return 0
}

# Install everything globally
install_spane() {
    log_message "Installing SPANE engine globally..."
    
    # Create directories
    sudo mkdir -p "$INSTALL_DIR"
    sudo mkdir -p "$GAMES_DIR"
    sudo mkdir -p "$BIN_DIR"
    
    # Copy engine binary
    sudo cp "$BUILD_DIR/$BINARY_NAME" "$INSTALL_DIR/$BINARY_NAME"
    sudo chmod 755 "$INSTALL_DIR/$BINARY_NAME"
    
    # Copy game libraries
    if [ -d "$BUILD_DIR/games" ]; then
        sudo cp "$BUILD_DIR/games"/*.so "$GAMES_DIR/" 2>/dev/null
        sudo chmod 644 "$GAMES_DIR"/*.so 2>/dev/null
    fi
    
    # Create wrapper script that runs from games directory
    sudo tee "$INSTALL_DIR/run_spane.sh" > /dev/null << 'EOF'
#!/bin/sh
# SPANE Engine Launcher
ENGINE_DIR="/usr/local/etc/Spane"
GAMES_DIR="$ENGINE_DIR/games"

# Change to games directory so engine can find .so files
cd "$GAMES_DIR" 2>/dev/null || cd "$ENGINE_DIR"

# Run engine with arguments
exec "$ENGINE_DIR/spane" "$@"
EOF
    sudo chmod 755 "$INSTALL_DIR/run_spane.sh"
    
    # Create symlink
    [ -L "$BIN_DIR/$BINARY_NAME" ] && sudo rm -f "$BIN_DIR/$BINARY_NAME"
    sudo ln -sf "$INSTALL_DIR/run_spane.sh" "$BIN_DIR/$BINARY_NAME"
    
    log_message "✓ Installed to $INSTALL_DIR"
    log_message "✓ Game libraries in $GAMES_DIR"
    log_message "✓ Global command: $BINARY_NAME"
}

# Cleanup build files
cleanup() {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        log_message "Cleaned up build directory"
    fi
}

# Uninstall
uninstall_spane() {
    log_message "Uninstalling SPANE engine..."
    
    [ -L "$BIN_DIR/$BINARY_NAME" ] && sudo rm -f "$BIN_DIR/$BINARY_NAME"
    [ -d "$INSTALL_DIR" ] && sudo rm -rf "$INSTALL_DIR"
    
    log_message "✓ SPANE engine uninstalled"
}

# Show help
show_help() {
    echo "SPANE Game Engine - Installation Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --install     Install SPANE engine (default)"
    echo "  --uninstall   Uninstall SPANE engine"
    echo "  --help        Show this help"
    echo ""
    echo "After installation, run: spane"
    echo "  spane           - X11 mode"
    echo "  spane --web     - Web server mode (http://localhost:3000)"
}

# =============================================================================
# MAIN EXECUTION
# =============================================================================

# Parse arguments
case "${1:-}" in
    --uninstall|-u)
        uninstall_spane
        exit 0
        ;;
    --help|-h)
        show_help
        exit 0
        ;;
esac

echo ""
echo "╔══════════════════════════════════════════╗"
echo "║     SPANE Game Engine - Installer        ║"
echo "╚══════════════════════════════════════════╝"
echo ""

# Check for existing installation
if [ -d "$INSTALL_DIR" ]; then
    log_message "Existing installation found"
    printf "Choose: [1]=Update [2]=Remove [3]=Exit: "
    read choice
    case "$choice" in
        1) log_message "Updating..."; sudo rm -rf "$INSTALL_DIR"; [ -L "$BIN_DIR/$BINARY_NAME" ] && sudo rm -f "$BIN_DIR/$BINARY_NAME" ;;
        2) uninstall_spane; exit 0 ;;
        *) exit 0 ;;
    esac
fi

# Install dependencies
install_dependencies

# Create build directory
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Find main engine file
engine_file="$MAIN_SOURCE_DIR/$MAIN_FILE"
if [ ! -f "$engine_file" ]; then
    # Search for file with main function
    engine_file=$(find "$MAIN_SOURCE_DIR" -maxdepth 1 -name "*.c" | while read f; do
        if has_main "$f"; then
            echo "$f"
            break
        fi
    done)
fi

if [ -z "$engine_file" ] || [ ! -f "$engine_file" ]; then
    log_message "Error: Could not find Spane.c or any file with main() function"
    exit 1
fi

log_message "Engine source: $engine_file"

# Compile engine
if compile_engine "$engine_file"; then
    # Compile games
    compile_games
    
    # Install
    install_spane
    cleanup
    
    # Count games
    game_count=$(ls "$GAMES_DIR"/*.so 2>/dev/null | wc -l)
    
    echo ""
    echo "╔══════════════════════════════════════════╗"
    echo "║       Installation Complete!             ║"
    echo "║                                          ║"
    echo "║  Run with: spane                         ║"
    echo "║  Web mode: spane --web                   ║"
    echo "║                                          ║"
    echo "║  Games installed: $game_count                    ║"
    echo "║  Game location: $GAMES_DIR"
    echo "║                                          ║"
    echo "║  Controls:                               ║"
    echo "║  - F1-F4: Switch games                   ║"
    echo "║  - Click sidebar: Switch games           ║"
    echo "║  - ESC: Quit                             ║"
    echo "║                                          ║"
    echo "║  Add games: Place .so files in:          ║"
    echo "║  $GAMES_DIR"
    echo "║                                          ║"
    echo "╚══════════════════════════════════════════╝"
    echo ""
else
    log_message "Compilation failed!"
    cleanup
    exit 1
fi