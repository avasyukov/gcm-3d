#!/bin/bash

SCRIPT="`readlink -f $0`"
DIR="`pwd`"

get_attachment() {
    sed -e "1,/^# ----- attachment $1/d" "$SCRIPT" | sed -e '/^# ----- attachment /,$d'
}

get_build_script() {
    get_attachment 1
}

get_bootstrap_script() {
    get_attachment 2
}

cd /tmp
rm -rf gcm-3d-bundle && mkdir gcm-3d-bundle && cd gcm-3d-bundle

# Links to original sources (as for 08.07.2013)
# 
# https://waf.googlecode.com/files/waf-1.7.11
# http://www.nic.funet.fi/pub/gnu/ftp.gnu.org/pub/gnu/gsl/gsl-1.15.tar.gz
# ftp://xmlsoft.org/libxml2/libxml2-2.9.1.tar.gz
# http://geuz.org/gmsh/src/gmsh-2.7.1-source.tgz
# http://www.cmake.org/files/v2.8/cmake-2.8.11.2.tar.gz
# http://apache-mirror.rbc.ru/pub/apache//apr/apr-1.4.8.tar.gz
# http://apache-mirror.rbc.ru/pub/apache//apr/apr-util-1.5.2.tar.gz
# http://www.sai.msu.su/apache/logging/log4cxx/0.10.0/apache-log4cxx-0.10.0.tar.gz
# http://www.vtk.org/files/release/5.2/vtk-5.2.1.tar.gz


if [[ -z "$GCM3D_DEPS_PATH" ]]; then
    # Downloading from Dropbox «mirror»
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/waf-1.7.11
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/gsl-1.15.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/libxml2-2.9.1.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/gmsh-2.7.1-source.tgz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/cmake-2.8.11.2.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/apr-1.4.8.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/apr-util-1.5.2.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/apache-log4cxx-0.10.0.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/vtk-5.2.1.tar.gz
    wget https://dl.dropboxusercontent.com/u/19548163/mipt/gcm3d-deps/patches.tar.gz
else
    cp "$GCM3D_DEPS_PATH"/* .
fi;

git clone https://github.com/avasyukov/gcm-3d.git

get_build_script > build.sh
chmod +x build.sh

cd "$DIR"

get_bootstrap_script > gcm3d-boostrap.sh
tar -cz -C /tmp/gcm-3d-bundle . >> gcm3d-boostrap.sh
chmod +x gcm3d-boostrap.sh

exit

# ----- attachment 1: script to build gcm3d with all dependencies
#!/bin/bash
# dependencies: gcc, g++, make, patch, gfortran

SCRIPT="`readlink -f $0`"
DIR="`dirname "$SCRIPT"`"

die() {
    echo "$1" 2>&1
    exit 1
}

prepare_env() {
    if [[ -z "$GCM3D_INSTALL_PATH" ]]; then
        export GCM3D_INSTALL_PATH="$HOME/gcm3d"
    fi;
    export PATH="$GCM3D_INSTALL_PATH/bin:$PATH"
    export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib:$LD_LIBRARY_PATH"
    export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib64:$LD_LIBRARY_PATH"
    export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib/vtk-5.2:$LD_LIBRARY_PATH"
}

unpack() {
    echo "Unpacking"
    cd "$DIR"                             && \
    mkdir patches                         && \
    tar -xf patches.tar.gz -C patches     && \
    tar -xf gsl-1.15.tar.gz               && \
    tar -xf libxml2-2.9.1.tar.gz          && \
    tar -xf cmake-2.8.11.2.tar.gz         && \
    tar -xf gmsh-2.7.1-source.tgz         && \
    tar -xf apr-1.4.8.tar.gz              && \
    tar -xf apr-util-1.5.2.tar.gz         && \
    tar -xf apache-log4cxx-0.10.0.tar.gz  && \
    tar -xf vtk-5.2.1.tar.gz
}

install_waf() {
    echo "Installing waf"
    cd "$DIR"                                   && \
    mkdir -p "$GCM3D_INSTALL_PATH/bin"          && \
    mv waf-1.7.11 "$GCM3D_INSTALL_PATH/bin/waf" && \
    chmod +x "$GCM3D_INSTALL_PATH/bin/waf"
}

install_gsl() {
    echo "Installing gsl"
    cd "$DIR/gsl-1.15"                 && \
    ./configure                           \
        --prefix="$GCM3D_INSTALL_PATH" && \
    make                               && \
    make install
}

install_libxml2() {
    echo "Installing libxml2"
    cd "$DIR/libxml2-2.9.1"           && \
    ./configure                          \
        --prefix="$GCM3D_INSTALL_PATH"   \
        --without-python              && \
    make                              && \
    make install
}

install_cmake(){
    echo "Installing cmake"
    cd "$DIR/cmake-2.8.11.2"           && \
    ./configure                           \
        --prefix="$GCM3D_INSTALL_PATH" && \
    make                               && \
    make install
}

install_gmsh() {
    echo "Installing gmsh"
    cd "$DIR/gmsh-2.7.1-source"                           && \
    patch                                                    \
        -p1                                                  \
        -i "$DIR/patches/gmsh-addruledfaces-return.patch" && \
    cmake                                                    \
        -DCMAKE_INSTALL_PREFIX="$GCM3D_INSTALL_PATH"         \
        -DENABLE_BUILD_DYNAMIC=1                             \
        -DENABLE_BUILD_SHARED=1                           && \
    make                                                  && \
    make install
}

install_apr() {
    echo "Installing apr"
    cd "$DIR/apr-1.4.8"                && \
    ./configure                           \
        --prefix="$GCM3D_INSTALL_PATH" && \
    make                               && \
    make install
}

install_apr_util() {
    echo "Installing apr-util"
    cd "$DIR/apr-util-1.5.2"                              && \
    ./configure                                              \
        --prefix="$GCM3D_INSTALL_PATH"                       \
        --with-apr="$GCM3D_INSTALL_PATH/bin/apr-1-config" && \
    make                                                  && \
    make install    
}

install_log4cxx() {
    echo "Installing log4cxx"
    cd "$DIR/apache-log4cxx-0.10.0"                            && \
    patch                                                         \
        -p1                                                       \
        -i "$DIR/patches/cppFolder_stringInclude.patch"        && \
    patch                                                         \
        -p1                                                       \
        -i "$DIR/patches/exampleFolder_stringInclude.patch"    && \
    ./configure                                                   \
        --prefix="$GCM3D_INSTALL_PATH"                            \
        --with-apr="$GCM3D_INSTALL_PATH/bin/apr-1-config"         \
        --with-apr-util="$GCM3D_INSTALL_PATH/bin/apu-1-config" && \
    make                                                       && \
    make install
}

install_vtk() {
    echo "Installing vtk"
    cd "$DIR/VTK"                                    && \
    cmake                                               \
        -DCMAKE_INSTALL_PREFIX="$GCM3D_INSTALL_PATH"    \
        -DVTK_USE_RENDERING=0                           \
        -DVTK_USE_TK=0                                  \
        -DVTK_WRAP_PYTHON=0                             \
        -DVTK_WRAP_TCL=0                                \
        -DVTK_WRAP_JAVA=0                               \
        -DBUILD_SHARED_LIBS=1                           \
        -DVTK_INSTALL_NO_DEVELOPMENT=0               && \
    make clean                                       && \
    make                                             && \
    make install
}

patch_bashrc() {
    if [[ ! -f "$HOME/.bashrc" ]] || ! grep -q 'source "$HOME/.gcm3d_bashrc"' "$HOME/.bashrc"; then
        echo 'source "$HOME/.gcm3d_bashrc"'                                          >> "$HOME/.bashrc"
    fi
    echo "export GCM3D_INSTALL_PATH=\"$GCM3D_INSTALL_PATH\""                         >  "$HOME/.gcm3d_bashrc"
    echo 'export PATH="$GCM3D_INSTALL_PATH/bin:$PATH"'                               >> "$HOME/.gcm3d_bashrc"
    echo 'export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib:$LD_LIBRARY_PATH"'         >> "$HOME/.gcm3d_bashrc"
    echo 'export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib64:$LD_LIBRARY_PATH"'       >> "$HOME/.gcm3d_bashrc"
    echo 'export LD_LIBRARY_PATH="$GCM3D_INSTALL_PATH/lib/vtk-5.2:$LD_LIBRARY_PATH"' >> "$HOME/.gcm3d_bashrc"
}

rm -f build.log

prepare_env

unpack            && \
install_waf       && \
install_cmake     && \
install_apr       && \
install_apr_util  && \
install_log4cxx   && \
install_gsl       && \
install_libxml2   && \
install_vtk       && \
install_gmsh      || die "Build failed"

patch_bashrc

echo "gcm3d dependencies installed"
echo "Now configure environment and build gcm3d manually"
echo ""
echo "Use command like this:"
echo 'cd "$DIR/gcm-3d"                                        && \'
echo 'waf configure                                              \'
echo '    --prefix="$GCM3D_INSTALL_PATH"                         \'
echo '    --includepath="$GCM3D_INSTALL_PATH/include/libxml2"    \'
echo '    --includepath="$GCM3D_INSTALL_PATH/include/gmsh"       \'
echo '    --includepath="$GCM3D_INSTALL_PATH/include"            \'
echo '    --includepath="$GCM3D_INSTALL_PATH/include/vtk-5.2"    \'
echo '    --libpath="$GCM3D_INSTALL_PATH/lib"                    \'
echo '    --libpath="$GCM3D_INSTALL_PATH/lib/vtk-5.2"            \'
echo '    --libpath="$GCM3D_INSTALL_PATH/lib64"               && \'
echo 'waf build                                               && \'
echo 'waf install                                                 '


exit

# ----- attachment 2: bootstrap script
#!/bin/bash
SCRIPT=`readlink -f $0`

if [[ -z "$GCM3D_INSTALL_TMP" ]]; then
    export GCM3D_INSTALL_TMP="$HOME/tmp"
fi

echo "gcm3d boostrap script"

mkdir -p "$GCM3D_INSTALL_TMP"
cd "$GCM3D_INSTALL_TMP"

rm -rf gcm-3d-boostrap && mkdir gcm-3d-boostrap && cd gcm-3d-boostrap
sed -e '1,/^exit$/d' "$SCRIPT" | tar xzf - && ./build.sh

exit
