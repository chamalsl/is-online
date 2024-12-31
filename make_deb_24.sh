VERSION=0.1
PATCH_VERSION=1
RELEASE_DIR=release/Ubuntu24/is-online_${VERSION}-${PATCH_VERSION}_amd64
mkdir -p ${RELEASE_DIR}/usr/local/bin
mkdir -p ${RELEASE_DIR}/DEBIAN
touch ${RELEASE_DIR}/DEBIAN/control
cp build/release/is-online ${RELEASE_DIR}/usr/local/bin/is-online

echo "Package: is-online" > ${RELEASE_DIR}/DEBIAN/control
echo "Version: 0.1.1" >> ${RELEASE_DIR}/DEBIAN/control
echo "Architecture: amd64" >> ${RELEASE_DIR}/DEBIAN/control
echo "Maintainer: Chamal De Silva <chamaldesilva@gmail.com>" >> ${RELEASE_DIR}/DEBIAN/control
echo "Description: Program which checks whether a web site is online." >> ${RELEASE_DIR}/DEBIAN/control
echo "Depends: libgtkmm-3.0-1t64 (>= 3.24.9), libcurl4t64 (>= 7.16.2) , libsqlite3-0 (>= 3.5.9)" >> ${RELEASE_DIR}/DEBIAN/control
cd release/Ubuntu24/
dpkg-deb --build --root-owner-group is-online_0.1-1_amd64


