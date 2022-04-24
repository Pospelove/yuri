vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO sammycage/lunasvg
  REF 7f82e941270aad040922798c210ce0e2c1eb58aa #2.3.1+
  SHA512 eac91b15d18bacb8ce580040517293210ac8de3e008a1ea93f2bed8e4d5fbd059e830c8c1bd835b3537aedd3ff81a1175c870458cb3d556cfed2878f42738d47
  HEAD_REF master
  PATCHES
    fix-install.patch
)

vcpkg_cmake_configure(
	SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS
		-DLUNASVG_BUILD_EXAMPLES=OFF
    -DBUILD_SHARED_LIBS=OFF
)

vcpkg_cmake_install()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

# Handle copyright
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)