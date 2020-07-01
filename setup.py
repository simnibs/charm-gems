# Build ITK
cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=../ITK-install ../ITK
make
make install

#
cmake -DITK_DIR=ITK-install -DPYTHON_EXECUTABLE=sys.executable .
make
