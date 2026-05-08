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

# Installation mode (default: full, --web: skip X11)
WEB_MODE=false

# Detect if we need sudo
if [ "$(id -u)" = "0" ]; then
    SUDO=""
else
    SUDO="sudo"
fi

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

# Install GCC and standard libraries
install_build_tools() {
    log_message "Checking build tools..."
    
    # Test if we can compile
    if command_exists gcc; then
        echo 'int main(){return 0;}' > /tmp/spane_test.c
        if gcc /tmp/spane_test.c -o /tmp/spane_test 2>/dev/null; then
            log_message "✓ Build tools working"
            rm -f /tmp/spane_test.c /tmp/spane_test
            return 0
        fi
        rm -f /tmp/spane_test.c /tmp/spane_test
        log_message "GCC found but cannot compile (missing headers)"
    fi
    
    log_message "Installing build tools..."
    
    # Alpine Linux (apk)
    if command_exists apk; then
        log_message "Detected Alpine Linux"
        if $SUDO apk update 2>/dev/null && $SUDO apk add build-base 2>/dev/null; then
            log_message "✓ Build tools installed via apk"
            return 0
        fi
        # Try just musl-dev and gcc
        if $SUDO apk add gcc musl-dev 2>/dev/null; then
            log_message "✓ Minimal build tools installed"
            return 0
        fi
        log_message "✗ Failed to install via apk"
        exit 1
    fi
    
    # Debian/Ubuntu (apt-get)
    if command_exists apt-get; then
        log_message "Detected APT"
        if $SUDO apt-get update -y 2>/dev/null && $SUDO apt-get install -y gcc build-essential 2>/dev/null; then
            log_message "✓ Build tools installed via apt-get"
            return 0
        fi
        exit 1
    fi
    
    # Fedora/RHEL (dnf)
    if command_exists dnf; then
        log_message "Detected DNF"
        if $SUDO dnf install -y gcc make 2>/dev/null; then
            log_message "✓ Build tools installed via dnf"
            return 0
        fi
        exit 1
    fi
    
    # CentOS/RHEL 7 (yum)
    if command_exists yum; then
        log_message "Detected YUM"
        if $SUDO yum install -y gcc make 2>/dev/null; then
            log_message "✓ Build tools installed via yum"
            return 0
        fi
        exit 1
    fi
    
    # Arch (pacman)
    if command_exists pacman; then
        log_message "Detected Pacman"
        if $SUDO pacman -S --noconfirm base-devel 2>/dev/null; then
            log_message "✓ Build tools installed via pacman"
            return 0
        fi
        exit 1
    fi
    
    # openSUSE (zypper)
    if command_exists zypper; then
        log_message "Detected Zypper"
        if $SUDO zypper install -y gcc make 2>/dev/null; then
            log_message "✓ Build tools installed via zypper"
            return 0
        fi
        exit 1
    fi
    
    log_message "✗ Unknown package manager"
    exit 1
}

# Install X11 development libraries (optional)
install_x11_libs() {
    log_message "Checking X11 development libraries..."
    
    if [ -f "/usr/include/X11/Xlib.h" ] || [ -f "/usr/local/include/X11/Xlib.h" ]; then
        log_message "✓ X11 found"
        return 0
    fi
    
    log_message "Installing X11 libraries..."
    
    if command_exists apk; then
        $SUDO apk add libx11-dev 2>/dev/null
    elif command_exists apt-get; then
        $SUDO apt-get install -y libx11-dev 2>/dev/null
    elif command_exists dnf; then
        $SUDO dnf install -y libX11-devel 2>/dev/null
    elif command_exists yum; then
        $SUDO yum install -y libX11-devel 2>/dev/null
    elif command_exists pacman; then
        $SUDO pacman -S --noconfirm libx11 2>/dev/null
    elif command_exists zypper; then
        $SUDO zypper install -y libX11-devel 2>/dev/null
    fi
    
    if [ -f "/usr/include/X11/Xlib.h" ]; then
        log_message "✓ X11 libraries installed"
        return 0
    fi
    
    log_message "⚠ X11 not available - will build web-only version"
    return 1
}

# Check if X11 headers are available
has_x11() {
    [ -f "/usr/include/X11/Xlib.h" ] || [ -f "/usr/local/include/X11/Xlib.h" ]
}

# Create web-only source
create_web_source() {
    local src="$1"
    local dst="$2"
    
    log_message "Creating web-only source..."
    
    {
        while IFS= read -r line || [ -n "$line" ]; do
            case "$line" in
                "#include <X11/Xlib.h>")
                    echo "// X11 stubs for web-only build"
                    echo "typedef unsigned long Window;"
                    echo "typedef unsigned long GC;"
                    echo "typedef unsigned long KeySym;"
                    echo "typedef struct _XDisplay Display;"
                    echo "typedef struct _XImage XImage;"
                    echo "typedef union _XEvent XEvent;"
                    echo "struct _XImage { int width, height; char *data; };"
                    echo ""
                    echo "static Display* XOpenDisplay(void* a) { return 0; }"
                    echo "static int XCloseDisplay(void* a) { return 0; }"
                    echo "static int DefaultScreen(void* a) { return 0; }"
                    echo "static Window XCreateSimpleWindow(void* a, Window b, int c, int d, unsigned int e, unsigned int f, unsigned int g, unsigned long h, unsigned long i) { return 0; }"
                    echo "static int XSelectInput(void* a, Window b, long c) { return 0; }"
                    echo "static int XStoreName(void* a, Window b, char* c) { return 0; }"
                    echo "static int XMapWindow(void* a, Window b) { return 0; }"
                    echo "static GC XCreateGC(void* a, Window b, unsigned long c, void* d) { return 0; }"
                    echo "static int XFreeGC(void* a, GC b) { return 0; }"
                    echo "static int XDestroyWindow(void* a, Window b) { return 0; }"
                    echo "static int XPending(void* a) { return 0; }"
                    echo "static int XNextEvent(void* a, void* b) { return 0; }"
                    echo "static KeySym XLookupKeysym(void* a, int b) { return 0; }"
                    echo "static XImage* XCreateImage(void* a, void* b, int c, int d, int e, char* f, int g, int h, int i, int j) { return 0; }"
                    echo "static int XPutImage(void* a, Window b, GC c, XImage* d, int e, int f, int g, int h, unsigned int i, unsigned int j) { return 0; }"
                    echo "static int XFlush(void* a) { return 0; }"
                    echo "static Window XRootWindow(void* a, int b) { return 0; }"
                    echo "static unsigned long XBlackPixel(void* a, int b) { return 0; }"
                    echo ""
                    echo "#define KeyPress 2"
                    echo "#define KeyRelease 3"
                    echo "#define ButtonPress 4"
                    echo "#define ButtonRelease 5"
                    echo "#define MotionNotify 6"
                    echo "#define MapNotify 19"
                    echo "#define Expose 12"
                    echo "#define ExposureMask 0"
                    echo "#define KeyPressMask 0"
                    echo "#define KeyReleaseMask 0"
                    echo "#define ButtonPressMask 0"
                    echo "#define ButtonReleaseMask 0"
                    echo "#define PointerMotionMask 0"
                    echo "#define StructureNotifyMask 0"
                    echo ""
                    ;;
                "#include <X11/keysym.h>"|"#include <X11/Xutil.h>")
                    # Skip
                    ;;
                *)
                    echo "$line"
                    ;;
            esac
        done < "$src"
        
        # Add stubs for X11 functions at the end
        echo ""
        echo "// Web-only implementations"
        echo "static void x11_mirror_frame(void* gm) {}"
        echo "static int x11_init(void* gm) { return 0; }"
        echo "static int x11_to_keycode(void* ks) { return 0; }"
        echo "static void x11_process(void* gm, int* running) {}"
        echo "static void x11_cleanup(void* gm) {}"
        
    } > "$dst"
    
    [ -f "$dst" ] && [ -s "$dst" ]
}

# Compile engine
compile_engine() {
    local src="$1"
    
    if has_x11; then
        log_message "Building with X11 support"
        
        local cflags=""
        local ldflags="-lX11"
        if command_exists pkg-config && pkg-config --exists x11 2>/dev/null; then
            cflags=$(pkg-config --cflags x11)
            ldflags=$(pkg-config --libs x11)
        fi
        
        cd "$BUILD_DIR" || exit 1
        
        gcc -O3 -march=native -pipe $cflags \
            -c "$src" -o spane_engine.o 2>&1 || {
            log_message "✗ Compilation failed"
            return 1
        }
        
        gcc -O3 spane_engine.o $ldflags -lm -ldl -lpthread \
            -o "$BINARY_NAME" 2>&1 || {
            log_message "✗ Linking failed"
            return 1
        }
        
        log_message "✓ Built with X11"
    else
        log_message "Building web-only version"
        
        local web_src="$BUILD_DIR/Spane_web.c"
        if ! create_web_source "$src" "$web_src"; then
            log_message "✗ Failed to create web source"
            return 1
        fi
        
        cd "$BUILD_DIR" || exit 1
        
        gcc -O3 -march=native -pipe \
            -o "$BINARY_NAME" "$web_src" \
            -lm -ldl -lpthread 2>&1 || {
            log_message "✗ Compilation failed"
            return 1
        }
        
        log_message "✓ Built web-only"
    fi
    
    strip "$BINARY_NAME" 2>/dev/null || true
    return 0
}

# Compile game libraries
compile_games() {
    log_message "Compiling game libraries..."
    
    local games_dir="$MAIN_SOURCE_DIR/games"
    mkdir -p "$BUILD_DIR/games"
    
    if [ ! -d "$games_dir" ]; then
        mkdir -p "$games_dir"
        log_message "Created games directory"
    fi
    
    local count=0
    for f in "$games_dir"/*.c; do
        [ ! -f "$f" ] && continue
        local name=$(basename "$f" .c)
        log_message "  Building $name..."
        
        if gcc -shared -fPIC -O3 -march=native \
            -o "$BUILD_DIR/games/${name}.so" "$f" 2>&1; then
            log_message "  ✓ $name.so"
            count=$((count + 1))
        else
            log_message "  ✗ Failed: $name"
        fi
    done
    
    log_message "Games compiled: $count"
    return 0
}

# Install
install_spane() {
    log_message "Installing..."
    
    $SUDO mkdir -p "$INSTALL_DIR" "$GAMES_DIR" "$BIN_DIR"
    
    $SUDO cp "$BUILD_DIR/$BINARY_NAME" "$INSTALL_DIR/"
    $SUDO chmod 755 "$INSTALL_DIR/$BINARY_NAME"
    
    if ls "$BUILD_DIR/games/"*.so >/dev/null 2>&1; then
        $SUDO cp "$BUILD_DIR/games/"*.so "$GAMES_DIR/" 2>/dev/null
        $SUDO chmod 644 "$GAMES_DIR/"*.so 2>/dev/null
    fi
    
    $SUDO tee "$INSTALL_DIR/run_spane.sh" > /dev/null << 'EOF'
#!/bin/sh
cd /usr/local/etc/Spane/games 2>/dev/null || cd /usr/local/etc/Spane
exec /usr/local/etc/Spane/spane "$@"
EOF
    $SUDO chmod 755 "$INSTALL_DIR/run_spane.sh"
    
    if [ -L "$BIN_DIR/$BINARY_NAME" ]; then
        $SUDO rm -f "$BIN_DIR/$BINARY_NAME"
    fi
    $SUDO ln -sf "$INSTALL_DIR/run_spane.sh" "$BIN_DIR/$BINARY_NAME"
    
    log_message "✓ Installed"
}

# Cleanup
cleanup() { rm -rf "$BUILD_DIR" 2>/dev/null; }

# Uninstall
uninstall_spane() {
    log_message "Uninstalling..."
    if [ -L "$BIN_DIR/$BINARY_NAME" ]; then
        $SUDO rm -f "$BIN_DIR/$BINARY_NAME"
    fi
    if [ -d "$INSTALL_DIR" ]; then
        $SUDO rm -rf "$INSTALL_DIR"
    fi
    log_message "✓ Uninstalled"
}

# Help
show_help() {
    echo "SPANE Game Engine Installer"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo "  --install    Install (default)"
    echo "  --web        Web-only install (no X11)"
    echo "  --uninstall  Remove installation"
    echo "  --help       This help"
    echo ""
    echo "Usage after install:"
    echo "  spane           X11 mode"
    echo "  spane --web     Web server (http://localhost:3000)"
}

# =============================================================================
# MAIN
# =============================================================================

case "${1:-}" in
    --uninstall|-u) uninstall_spane; exit 0 ;;
    --help|-h) show_help; exit 0 ;;
    --web) WEB_MODE=true ;;
esac

echo ""
echo "╔════════════════════════════════╗"
echo "║  SPANE Game Engine Installer   ║"
if [ "$WEB_MODE" = true ]; then
    echo "║  Mode: Web Server Only         ║"
fi
echo "╚════════════════════════════════╝"
echo ""

# Check existing install
if [ -d "$INSTALL_DIR" ]; then
    log_message "Existing installation found"
    printf "[1]=Update [2]=Remove [3]=Exit: "
    read choice
    case "$choice" in
        1) $SUDO rm -rf "$INSTALL_DIR"; [ -L "$BIN_DIR/$BINARY_NAME" ] && $SUDO rm -f "$BIN_DIR/$BINARY_NAME" ;;
        2) uninstall_spane; exit 0 ;;
        *) exit 0 ;;
    esac
fi

# Install build tools (required)
install_build_tools

# Install X11 if needed
if [ "$WEB_MODE" = false ]; then
    install_x11_libs
fi

# Setup build dir
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Find source
src="$MAIN_SOURCE_DIR/$MAIN_FILE"
if [ ! -f "$src" ]; then
    src=$(find "$MAIN_SOURCE_DIR" -maxdepth 1 -name "*.c" -exec grep -l "int main" {} \; | head -1)
fi

if [ ! -f "$src" ]; then
    log_message "Error: No Spane.c found"
    exit 1
fi

log_message "Source: $src"

# Compile
if compile_engine "$src"; then
    compile_games
    install_spane
    cleanup
    
    games=$(ls "$GAMES_DIR"/*.so 2>/dev/null | wc -l)
    
    echo ""
    echo "╔════════════════════════════════╗"
    echo "║    Installation Complete!      ║"
    echo "║                                ║"
    if has_x11; then
        echo "║  spane          X11 mode       ║"
    fi
    echo "║  spane --web    Web mode       ║"
    echo "║  Games: $games                    ║"
    echo "║                                ║"
    echo "║  Add games: $GAMES_DIR"
    echo "╚════════════════════════════════╝"
    echo ""
else
    log_message "Compilation failed!"
    cleanup
    exit 1
fi