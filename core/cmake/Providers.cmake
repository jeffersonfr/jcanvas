set (IMAGE_PROVIDER_LIST)

# png
pkg_check_modules(LibPng REQUIRED IMPORTED_TARGET libpng)

if (LibPng_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/pngimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibPng)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PNG_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST png)
endif()

# jpeg
pkg_check_modules(LibJpeg IMPORTED_TARGET libjpeg)

if (LibJpeg_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/jpgimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibJpeg)
  target_compile_definitions(${PROJECT_NAME} PRIVATE JPG_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST jpeg)
endif()

# heif
pkg_check_modules(LibHeif IMPORTED_TARGET libheif)

if (LibHeif_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/heifimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibHeif)
  target_compile_definitions(${PROJECT_NAME} PRIVATE HEIF_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST heif)
endif()

# webp
pkg_check_modules(LibWebp IMPORTED_TARGET libwebp)

if (LibWebp_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/webpimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibWebp)
  target_compile_definitions(${PROJECT_NAME} PRIVATE WEBP_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST webp)
endif()

# svg
# pkg_check_modules(LibSvg IMPORTED_TARGET librsvg-2.0)

if (LibSvg_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/svgimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LibSvg)
  target_compile_definitions(${PROJECT_NAME} PRIVATE SVG_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST svg)
endif()

# bmp
target_sources(${PROJECT_NAME} PRIVATE src/providers/bmpimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE BMP_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST bmp)

# gif
target_sources(${PROJECT_NAME} PRIVATE src/providers/gifimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE GIF_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST gif)

# ico
target_sources(${PROJECT_NAME} PRIVATE src/providers/icoimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE ICO_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST ico)

# pcx
target_sources(${PROJECT_NAME} PRIVATE src/providers/pcximage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE PCX_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST pcx)

# ppm
target_sources(${PROJECT_NAME} PRIVATE src/providers/ppmimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE PPM_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST ppm)

# tga
target_sources(${PROJECT_NAME} PRIVATE src/providers/tgaimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE TGA_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST tga)

# xbm
target_sources(${PROJECT_NAME} PRIVATE src/providers/xbmimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE XBM_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST xbm)

# xpm
target_sources(${PROJECT_NAME} PRIVATE src/providers/xpmimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE XPM_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST xpm)

# mjpeg
target_sources(${PROJECT_NAME} PRIVATE src/providers/mjpegimage.cpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE MJPEG_IMAGE)
list(APPEND IMAGE_PROVIDER_LIST mjpeg)

# jp2
find_package(Jasper)

if (Jasper_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/jp2image.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC Jasper)
  target_compile_definitions(${PROJECT_NAME} PRIVATE JP2_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST jp2)
endif()

# tiff
find_package(Tiff)

if (Tiff_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/tiffimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC Tiff)
  target_compile_definitions(${PROJECT_NAME} PRIVATE TIFF_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST tiff)
endif()

# bpg
find_package(Bpg)

if (Bpg_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/bpgimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC Bpg)
  target_compile_definitions(${PROJECT_NAME} PRIVATE BPG_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST bpg)
endif()

# flif
find_package(Flif)

if (Flif_FOUND)
  target_sources(${PROJECT_NAME} PRIVATE src/providers/flifimage.cpp)
  target_link_libraries(${PROJECT_NAME} PUBLIC Flif)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FLIF_IMAGE)
  list(APPEND IMAGE_PROVIDER_LIST flif)
endif()

message ("\tProviders: ${IMAGE_PROVIDER_LIST}")

