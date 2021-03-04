set (IMAGE_LIST)

# png
pkg_check_modules(PNG libpng)

if (${PNG_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/pngimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${PNG_LIBRARIES} 
  )

  add_definitions("-DPNG_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} png)
endif()

# jpg
pkg_check_modules(JPG libjpeg)

if (${JPG_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/jpgimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${JPG_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${JPG_LIBRARIES} 
  )

  add_definitions("-DJPG_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} jpg)
endif()

# bmp
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/bmpimage.cpp
)

add_definitions("-DBMP_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} bmp)

# gif
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/gifimage.cpp
)

add_definitions("-DGIF_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} gif)

# ico
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/icoimage.cpp
)

add_definitions("-DICO_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} ico)

# pcx
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/pcximage.cpp
)

add_definitions("-DPCX_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} pcx)

# ppm
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/ppmimage.cpp
)

add_definitions("-DPPM_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} ppm)

# tga
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/tgaimage.cpp
)

add_definitions("-DTGA_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} tga)

# xbm
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/xbmimage.cpp
)

add_definitions("-DXBM_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} xbm)

# xpm
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/xpmimage.cpp
)

add_definitions("-DXPM_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} xpm)

# mjpeg
set(IMAGE_PROVIDERS_SRCS 
  ${IMAGE_PROVIDERS_SRCS}
  src/providers/mjpegimage.cpp
)

add_definitions("-DMJPEG_IMAGE")
set (IMAGE_LIST ${IMAGE_LIST} mjpeg)

# heif
pkg_check_modules(HEIF libheif)

if (${HEIF_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/heifimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${HEIF_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${HEIF_LIBRARIES} 
  )

  add_definitions("-DHEIF_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} heif)
endif()

# webp
pkg_check_modules(WEBP libwebp)

if (${WEBP_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/webpimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${WEBP_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${WEBP_LIBRARIES} 
  )

  add_definitions("-DWEBP_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} webp)
endif()

# svg
pkg_check_modules(SVG librsvg-2.0)

if (${SVG_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/svgimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${SVG_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${SVG_LIBRARIES} 
  )

  add_definitions("-DSVG_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} svg)
endif()

# jp2000
include (cmake/FindJasper.cmake)

if (${JASPER_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/jp2000image.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${JASPER_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${JASPER_LIBRARIES} 
  )

  add_definitions("-DJP2000_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} jp2000)
endif()

# tiff
include (cmake/FindTiff.cmake)

if (${TIFF_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/tiffimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${TIFF_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${TIFF_LIBRARIES} 
  )

  add_definitions("-DTIFF_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} tiff)
endif()

# bpg
include (cmake/FindBpg.cmake)

if (${BPG_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/bpgimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${BPG_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${BPG_LIBRARIES} 
  )

  add_definitions("-DBPG_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} bpg)
endif()

# flif
include (cmake/FindFlif.cmake)

if (${FLIF_FOUND})
  set(IMAGE_PROVIDERS_SRCS 
    ${IMAGE_PROVIDERS_SRCS}
    src/providers/flifimage.cpp
  )

  set(IMAGE_PROVIDERS_INCLUDE_DIRS 
    ${IMAGE_PROVIDERS_INCLUDE_DIRS}
    ${FLIF_INCLUDE_DIRS}
  )
  
  set(IMAGE_PROVIDERS_LIBRARIES
    ${IMAGE_PROVIDERS_LIBRARIES}
    ${FLIF_LIBRARIES} 
  )

  add_definitions("-DFLIF_IMAGE")
  set (IMAGE_LIST ${IMAGE_LIST} flif)
endif()

set(SRCS ${SRCS} ${IMAGE_PROVIDERS_SRCS})
