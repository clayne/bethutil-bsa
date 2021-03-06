vcpkg_from_github(
  OUT_SOURCE_PATH
  SOURCE_PATH
  REPO
  sheredom/utf8.h
  REF
  500d4ea9f4c3449e5243c088d8af8700f7189734
  SHA512
  fbae7dbfd932176058dacb33ea491d17eb8e02c845e1ef2afc0ae482f563578a8a8011a8578f4aa8593bb0d7de8ba2df140bda05859a9a6bd67f6545ca0d7421
  HEAD_REF
  master)

file(COPY "${SOURCE_PATH}/utf8.h"
     DESTINATION "${CURRENT_PACKAGES_DIR}/include/utf8h")

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)
