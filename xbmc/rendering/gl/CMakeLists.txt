if(TARGET ${APP_NAME_LC}::OpenGl)
  set(SOURCES RenderSystemGL.cpp
              ScreenshotSurfaceGL.cpp
              GLShader.cpp)

  set(HEADERS RenderSystemGL.h
              ScreenshotSurfaceGL.h
              GLShader.h)

  core_add_library(rendering_gl)
endif()

