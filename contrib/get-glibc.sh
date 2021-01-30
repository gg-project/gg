set -xe
cd third_party
git clone --depth 1 -b release/2.32/master git://sourceware.org/git/glibc.git
cd glibc
mkdir build
cd build
../configure --prefix=$(pwd)/../../install --enable-static-nss
make -j $(nproc)
