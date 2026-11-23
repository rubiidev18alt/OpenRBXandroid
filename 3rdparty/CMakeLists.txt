if (NOT ANDROID)
set(OPENRBX_QTBASE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qtbase")
set(OPENRBX_QTBASE_BINARY_DIR "${CMAKE_BINARY_DIR}/3rdparty/qtbase-build")
set(OPENRBX_QTBASE_PREFIX "${OPENRBX_QTBASE_BINARY_DIR}/install")

set(OPENRBX_QTBASE_OPTIONS
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_INSTALL_PREFIX=${OPENRBX_QTBASE_PREFIX}
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DBUILD_SHARED_LIBS=ON
  -DQT_BUILD_TESTS=OFF
  -DQT_BUILD_EXAMPLES=OFF
  -DFEATURE_sql=OFF
  -DFEATURE_network=OFF
  -DFEATURE_testlib=OFF
  -DFEATURE_concurrent=OFF
  -DFEATURE_dbus=OFF
  -DFEATURE_vulkan=OFF
  -DFEATURE_xml=OFF
  -DQT_GENERATE_SBOM=OFF
)
if (WIN32)
  list(APPEND OPENRBX_QTBASE_OPTIONS -DFEATURE_windeployqt=OFF -DFEATURE_printsupport=OFF)
else()
  list(APPEND OPENRBX_QTBASE_OPTIONS
    -DFEATURE_printsupport=ON
    -DFEATURE_cups=ON
    -DFEATURE_icu=OFF
    -DFEATURE_glib=ON
    -DFEATURE_libudev=OFF
    -DFEATURE_evdev=OFF
    -DFEATURE_eglfs=OFF
    -DFEATURE_linuxfb=OFF
    -DFEATURE_vnc=OFF
    -DFEATURE_xcb=ON
    -DFEATURE_system_zlib=OFF
    -DFEATURE_system_png=OFF
    -DFEATURE_system_jpeg=OFF
    -DFEATURE_system_pcre2=OFF
    -DFEATURE_system_harfbuzz=OFF
    -DFEATURE_system_doubleconversion=OFF
    -DFEATURE_system_libb2=OFF
  )
endif()

if (NOT EXISTS "${OPENRBX_QTBASE_PREFIX}/lib/cmake/Qt6/Qt6Config.cmake")
  message(STATUS "Building qtbase in ${OPENRBX_QTBASE_BINARY_DIR}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${OPENRBX_QTBASE_SOURCE_DIR}" -B "${OPENRBX_QTBASE_BINARY_DIR}" -G Ninja
            ${OPENRBX_QTBASE_OPTIONS}
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qtbase configure failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --build "${OPENRBX_QTBASE_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qtbase build failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --install "${OPENRBX_QTBASE_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qtbase install failed")
  endif()
endif()

if (NOT EXISTS "${OPENRBX_QTBASE_PREFIX}/lib/cmake/Qt6Core5Compat/Qt6Core5CompatConfig.cmake")
  set(OPENRBX_QT5COMPAT_BINARY_DIR "${CMAKE_BINARY_DIR}/3rdparty/qt5compat-build")
  message(STATUS "Building qt5compat in ${OPENRBX_QT5COMPAT_BINARY_DIR}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_CURRENT_SOURCE_DIR}/qt5compat" -B "${OPENRBX_QT5COMPAT_BINARY_DIR}" -G Ninja
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_PREFIX_PATH=${OPENRBX_QTBASE_PREFIX}
            -DCMAKE_INSTALL_PREFIX=${OPENRBX_QTBASE_PREFIX}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DQT_BUILD_TESTS=OFF
            -DQT_BUILD_EXAMPLES=OFF
            -DQT_GENERATE_SBOM=OFF
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qt5compat configure failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --build "${OPENRBX_QT5COMPAT_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qt5compat build failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --install "${OPENRBX_QT5COMPAT_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "qt5compat install failed")
  endif()
endif()

list(APPEND CMAKE_PREFIX_PATH "${OPENRBX_QTBASE_PREFIX}")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
endif() # NOT ANDROID

foreach(option ALSOFT_UTILS ALSOFT_EXAMPLES ALSOFT_INSTALL ALSOFT_INSTALL_CONFIG ALSOFT_INSTALL_HRTF_DATA
               ALSOFT_INSTALL_AMBDEC_PRESETS ALSOFT_INSTALL_EXAMPLES ALSOFT_INSTALL_UTILS ALSOFT_UPDATE_BUILD_VERSION)
  set(${option} OFF CACHE BOOL "" FORCE)
endforeach()
if (ANDROID)
  foreach(option ALSOFT_BACKEND_OSS ALSOFT_BACKEND_JACK ALSOFT_BACKEND_PORTAUDIO ALSOFT_BACKEND_SNDIO
                 ALSOFT_BACKEND_SOLARIS ALSOFT_BACKEND_ALSA ALSOFT_BACKEND_PULSEAUDIO ALSOFT_BACKEND_PIPEWIRE
                 ALSOFT_REQUIRE_PIPEWIRE ALSOFT_REQUIRE_PULSEAUDIO ALSOFT_REQUIRE_ALSA)
    set(${option} OFF CACHE BOOL "" FORCE)
  endforeach()
  set(ALSOFT_BACKEND_OPENSL ON CACHE BOOL "" FORCE)
  set(ALSOFT_REQUIRE_OPENSL ON CACHE BOOL "" FORCE)
elseif (NOT WIN32)
  foreach(option ALSOFT_BACKEND_OSS ALSOFT_BACKEND_JACK ALSOFT_BACKEND_PORTAUDIO ALSOFT_BACKEND_SNDIO
                 ALSOFT_BACKEND_SOLARIS)
    set(${option} OFF CACHE BOOL "" FORCE)
  endforeach()
  foreach(option ALSOFT_REQUIRE_PIPEWIRE ALSOFT_REQUIRE_PULSEAUDIO ALSOFT_REQUIRE_ALSA)
    set(${option} ON CACHE BOOL "" FORCE)
  endforeach()
endif()
add_subdirectory(openal-soft EXCLUDE_FROM_ALL)
if (NOT ANDROID)
  set_target_properties(OpenAL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
  )
endif()

set(LIBOPENMPT "${CMAKE_CURRENT_SOURCE_DIR}/libopenmpt")
file(GLOB OPENRBX_LIBOPENMPT_SOURCES
  "${LIBOPENMPT}/src/openmpt/all/*.cpp"
  "${LIBOPENMPT}/src/openmpt/base/*.cpp"
  "${LIBOPENMPT}/src/openmpt/logging/*.cpp"
  "${LIBOPENMPT}/src/openmpt/random/*.cpp"
  "${LIBOPENMPT}/src/openmpt/fileformat_base/*.cpp"
  "${LIBOPENMPT}/src/openmpt/soundbase/*.cpp"
  "${LIBOPENMPT}/src/openmpt/soundfile_data/*.cpp"
  "${LIBOPENMPT}/common/*.cpp"
  "${LIBOPENMPT}/soundlib/*.cpp"
  "${LIBOPENMPT}/soundlib/plugins/*.cpp"
  "${LIBOPENMPT}/soundlib/plugins/dmo/*.cpp"
  "${LIBOPENMPT}/sounddsp/*.cpp"
  "${LIBOPENMPT}/libopenmpt/*.cpp"
)
add_library(openmpt STATIC ${OPENRBX_LIBOPENMPT_SOURCES})
target_include_directories(openmpt PRIVATE "${LIBOPENMPT}/src" "${LIBOPENMPT}/common")
target_include_directories(openmpt PUBLIC "${LIBOPENMPT}")
target_compile_definitions(openmpt PRIVATE LIBOPENMPT_BUILD)
set_target_properties(openmpt PROPERTIES CXX_STANDARD 20)

add_library(stb_vorbis STATIC stb/stb_vorbis.c)
target_include_directories(stb_vorbis PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/stb")
target_compile_definitions(stb_vorbis PUBLIC STB_VORBIS_NO_STDIO STB_VORBIS_NO_PUSHDATA_API)

if (MSVC)
  set(CMAKE_CXX_FLAGS "/nologo")
  set(CMAKE_C_FLAGS "/nologo")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "")
  set(CMAKE_CXX_FLAGS_RELEASE "")
  set(CMAKE_C_FLAGS_RELEASE "")
endif()

set(G3D "${PROJECT_SOURCE_DIR}/Client/Rendering/g3d")

file(GLOB OPENRBX_GRAPHICS3D_SOURCES "${G3D}/g3dcpp/*.cpp" "${G3D}/IJG/*.c" "${G3D}/png/*.c" "${G3D}/zlib/*.c")
file(GLOB OPENRBX_GLG3D_SOURCES "${G3D}/glg3dcpp/*.cpp")

add_library(graphics3d STATIC ${OPENRBX_GRAPHICS3D_SOURCES})
add_library(glg3d_egl STATIC ${OPENRBX_GLG3D_SOURCES})
target_compile_definitions(glg3d_egl PRIVATE G3D_EGL)
if (ANDROID)
  target_compile_definitions(graphics3d PRIVATE PNG_NO_MMX_CODE G3D_GLES_COMPAT)
  target_compile_definitions(glg3d_egl PUBLIC G3D_EGL G3D_GLES_COMPAT)
endif()
target_link_libraries(glg3d_egl PUBLIC egl)
set(OPENRBX_G3D_TARGETS graphics3d glg3d_egl)
if (NOT ANDROID)
  add_library(glg3d STATIC ${OPENRBX_GLG3D_SOURCES})
  list(APPEND OPENRBX_G3D_TARGETS glg3d)
  if (NOT WIN32)
    target_link_libraries(glg3d PUBLIC OpenGL::GL X11::X11)
  endif()
endif()

foreach(name ${OPENRBX_G3D_TARGETS})
  target_include_directories(${name} PRIVATE
    "${G3D}/include" "${G3D}/png" "${G3D}/zlib" "${G3D}/IJG"
  )
  if (WIN32)
    target_compile_definitions(${name} PRIVATE WIN32 NDEBUG _LIB G3D_BUILDING_LIBRARY_DLL _VC80_UPGRADE=0x0710 _MBCS)
  else()
    target_include_directories(${name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/glu/include")
    target_compile_definitions(${name} PRIVATE NDEBUG _LIB G3D_DEBUG_NOGUI)
    set_target_properties(${name} PROPERTIES C_STANDARD 99)
  endif()
  if (MSVC)
    target_compile_options(${name} PRIVATE /O2 /Ob1 /GF /EHsc /Gy)
  endif()
endforeach()
if (MSVC)
  target_compile_options(graphics3d PRIVATE /W2)
  target_compile_options(glg3d PRIVATE /W3)
  target_compile_options(glg3d_egl PRIVATE /W3)
endif()

add_library(egl INTERFACE)
if (WIN32)
  set(OPENRBX_MESA_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mesa")
  set(OPENRBX_MESA_BINARY_DIR "${CMAKE_BINARY_DIR}/3rdparty/mesa-build")
  set(OPENRBX_MESA_PREFIX "${OPENRBX_MESA_BINARY_DIR}/install")

  if (NOT EXISTS "${OPENRBX_MESA_PREFIX}/lib/libEGL.lib")
    find_program(MESON_EXECUTABLE meson REQUIRED)
    message(STATUS "Building Mesa in ${OPENRBX_MESA_BINARY_DIR}")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E env "NINJA=${CMAKE_MAKE_PROGRAM}" PYTHONDONTWRITEBYTECODE=1
              "${MESON_EXECUTABLE}" setup "${OPENRBX_MESA_BINARY_DIR}" "${OPENRBX_MESA_SOURCE_DIR}"
              --wipe --wrap-mode=nodownload --backend=ninja --buildtype=release --prefix=${OPENRBX_MESA_PREFIX}
              -Db_vscrt=md -Dplatforms=windows -Dgallium-drivers=softpipe -Dvulkan-drivers= -Dllvm=disabled
              -Degl=enabled -Dopengl=true -Dgles1=disabled -Dgles2=disabled -Dglx=disabled -Dgbm=disabled
              -Dgallium-va=disabled -Dgallium-mediafoundation=disabled -Dgallium-rusticl=false -Dvideo-codecs=
              -Dgallium-d3d12-video=disabled -Dgallium-d3d12-graphics=disabled -Dmicrosoft-clc=disabled
              -Dspirv-to-dxil=false -Dspirv-tools=disabled -Dteflon=false -Dzlib=disabled -Dzstd=disabled
              -Dexpat=disabled -Dvalgrind=disabled -Dlibunwind=disabled -Dlmsensors=disabled -Dbuild-tests=false
              -Dshader-cache=disabled -Dtools= -Dhtml-docs=disabled -Dxmlconfig=disabled -Dperfetto=false
              -Dsysprof=false -Dgpuvis=false -Dtorx=false
      RESULT_VARIABLE result
    )
    if (NOT result EQUAL 0)
      message(FATAL_ERROR "Mesa configure failed")
    endif()
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E env "NINJA=${CMAKE_MAKE_PROGRAM}" PYTHONDONTWRITEBYTECODE=1
              "${MESON_EXECUTABLE}" install -C "${OPENRBX_MESA_BINARY_DIR}"
      RESULT_VARIABLE result
    )
    if (NOT result EQUAL 0)
      message(FATAL_ERROR "Mesa build failed")
    endif()
  endif()

  target_include_directories(egl INTERFACE "${OPENRBX_MESA_SOURCE_DIR}/include")
  target_link_libraries(egl INTERFACE "${OPENRBX_MESA_PREFIX}/lib/libEGL.lib" "${OPENRBX_MESA_PREFIX}/lib/opengl32.lib")
elseif (ANDROID)
  target_link_libraries(egl INTERFACE EGL GLESv2)
else()
  target_link_libraries(egl INTERFACE OpenGL::EGL OpenGL::OpenGL)
endif()

set(GLU "${CMAKE_CURRENT_SOURCE_DIR}/glu")
file(GLOB OPENRBX_GLU_SOURCES "${GLU}/src/libutil/*.c" "${GLU}/src/libtess/*.c" "${GLU}/src/libnurbs/*/*.cc")
list(REMOVE_ITEM OPENRBX_GLU_SOURCES "${GLU}/src/libtess/priorityq-heap.c")
if (WIN32)
  add_library(glu SHARED ${OPENRBX_GLU_SOURCES})
  set_target_properties(glu PROPERTIES OUTPUT_NAME "GLU32" SUFFIX ".DLL")
  target_compile_definitions(glu PRIVATE BUILD_GLU32 RESOLVE_3D_TEXTURE_SUPPORT)
  target_include_directories(glu PRIVATE "${OPENRBX_MESA_SOURCE_DIR}/include")
  target_link_libraries(glu PRIVATE "${OPENRBX_MESA_PREFIX}/lib/opengl32.lib")
else()
  add_library(glu STATIC ${OPENRBX_GLU_SOURCES})
  set_target_properties(glu PROPERTIES POSITION_INDEPENDENT_CODE ON)
  if (ANDROID)
    target_include_directories(glu BEFORE PRIVATE "${G3D}/include")
    target_compile_definitions(glu PRIVATE G3D_GLES_COMPAT G3D_EGL G3D_DEBUG_NOGUI)
  endif()
endif()
target_include_directories(glu PRIVATE
  "${GLU}/src/include" "${GLU}/src/libnurbs/interface" "${GLU}/src/libnurbs/internals" "${GLU}/src/libnurbs/nurbtess"
)
target_include_directories(glu PUBLIC "${GLU}/include")
target_compile_definitions(glu PRIVATE LIBRARYBUILD)

foreach(name graphics3d glg3d glg3d_egl glu)
  if (TARGET ${name})
    set_target_properties(${name} PROPERTIES
      C_CLANG_TIDY "" CXX_CLANG_TIDY "" CXX_INCLUDE_WHAT_YOU_USE "")
  endif()
endforeach()

set(OGRE "${PROJECT_SOURCE_DIR}/3rdparty/ogrenew")

set(OPENRBX_OGREMAIN_SOURCES
  "${OGRE}/OgreMain/src/OgreAlignedAllocator.cpp"
  "${OGRE}/OgreMain/src/OgreAnimable.cpp"
  "${OGRE}/OgreMain/src/OgreAnimation.cpp"
  "${OGRE}/OgreMain/src/OgreAnimationState.cpp"
  "${OGRE}/OgreMain/src/OgreAnimationTrack.cpp"
  "${OGRE}/OgreMain/src/OgreArchiveManager.cpp"
  "${OGRE}/OgreMain/src/OgreAutoParamDataSource.cpp"
  "${OGRE}/OgreMain/src/OgreBillboard.cpp"
  "${OGRE}/OgreMain/src/OgreBillboardChain.cpp"
  "${OGRE}/OgreMain/src/OgreBillboardParticleRenderer.cpp"
  "${OGRE}/OgreMain/src/OgreBillboardSet.cpp"
  "${OGRE}/OgreMain/src/OgreBone.cpp"
  "${OGRE}/OgreMain/src/OgreBorderPanelOverlayElement.cpp"
  "${OGRE}/OgreMain/src/OgreCamera.cpp"
  "${OGRE}/OgreMain/src/OgreCodec.cpp"
  "${OGRE}/OgreMain/src/OgreColourValue.cpp"
  "${OGRE}/OgreMain/src/OgreCommon.cpp"
  "${OGRE}/OgreMain/src/OgreCompiler2Pass.cpp"
  "${OGRE}/OgreMain/src/OgreCompositionPass.cpp"
  "${OGRE}/OgreMain/src/OgreCompositionTargetPass.cpp"
  "${OGRE}/OgreMain/src/OgreCompositionTechnique.cpp"
  "${OGRE}/OgreMain/src/OgreCompositor.cpp"
  "${OGRE}/OgreMain/src/OgreCompositorChain.cpp"
  "${OGRE}/OgreMain/src/OgreCompositorInstance.cpp"
  "${OGRE}/OgreMain/src/OgreCompositorManager.cpp"
  "${OGRE}/OgreMain/src/OgreCompositorScriptCompiler.cpp"
  "${OGRE}/OgreMain/src/OgreCompositorSerializer.cpp"
  "${OGRE}/OgreMain/src/WIN32/OgreConfigDialog.cpp"
  "${OGRE}/OgreMain/src/OgreConfigFile.cpp"
  "${OGRE}/OgreMain/src/OgreControllerManager.cpp"
  "${OGRE}/OgreMain/src/OgreConvexBody.cpp"
  "${OGRE}/OgreMain/src/OgreDataStream.cpp"
  "${OGRE}/OgreMain/src/OgreDDSCodec.cpp"
  "${OGRE}/OgreMain/src/OgreDefaultHardwareBufferManager.cpp"
  "${OGRE}/OgreMain/src/OgreDefaultSceneQueries.cpp"
  "${OGRE}/OgreMain/src/OgreDynLib.cpp"
  "${OGRE}/OgreMain/src/OgreDynLibManager.cpp"
  "${OGRE}/OgreMain/src/OgreEdgeListBuilder.cpp"
  "${OGRE}/OgreMain/src/OgreEntity.cpp"
  "${OGRE}/OgreMain/src/WIN32/OgreErrorDialog.cpp"
  "${OGRE}/OgreMain/src/OgreException.cpp"
  "${OGRE}/OgreMain/src/OgreExternalTextureSource.cpp"
  "${OGRE}/OgreMain/src/OgreExternalTextureSourceManager.cpp"
  "${OGRE}/OgreMain/src/OgreFileSystem.cpp"
  "${OGRE}/OgreMain/src/OgreFont.cpp"
  "${OGRE}/OgreMain/src/OgreFontManager.cpp"
  "${OGRE}/OgreMain/src/OgreFreeImageCodec.cpp"
  "${OGRE}/OgreMain/src/OgreFrustum.cpp"
  "${OGRE}/OgreMain/src/OgreGpuProgram.cpp"
  "${OGRE}/OgreMain/src/OgreGpuProgramManager.cpp"
  "${OGRE}/OgreMain/src/OgreGpuProgramUsage.cpp"
  "${OGRE}/OgreMain/src/OgreHardwareBufferManager.cpp"
  "${OGRE}/OgreMain/src/OgreHardwareIndexBuffer.cpp"
  "${OGRE}/OgreMain/src/OgreHardwareOcclusionQuery.cpp"
  "${OGRE}/OgreMain/src/OgreHardwarePixelBuffer.cpp"
  "${OGRE}/OgreMain/src/OgreHardwareVertexBuffer.cpp"
  "${OGRE}/OgreMain/src/OgreHighLevelGpuProgram.cpp"
  "${OGRE}/OgreMain/src/OgreHighLevelGpuProgramManager.cpp"
  "${OGRE}/OgreMain/src/OgreImage.cpp"
  "${OGRE}/OgreMain/src/OgreInstancedGeometry.cpp"
  "${OGRE}/OgreMain/src/OgreKeyFrame.cpp"
  "${OGRE}/OgreMain/src/OgreLight.cpp"
  "${OGRE}/OgreMain/src/OgreLog.cpp"
  "${OGRE}/OgreMain/src/OgreLogManager.cpp"
  "${OGRE}/OgreMain/src/OgreManualObject.cpp"
  "${OGRE}/OgreMain/src/OgreMaterial.cpp"
  "${OGRE}/OgreMain/src/OgreMaterialManager.cpp"
  "${OGRE}/OgreMain/src/OgreMaterialScriptCompiler.cpp"
  "${OGRE}/OgreMain/src/OgreMaterialSerializer.cpp"
  "${OGRE}/OgreMain/src/OgreMath.cpp"
  "${OGRE}/OgreMain/src/OgreMatrix3.cpp"
  "${OGRE}/OgreMain/src/OgreMatrix4.cpp"
  "${OGRE}/OgreMain/src/OgreMemoryManager.cpp"
  "${OGRE}/OgreMain/src/OgreMesh.cpp"
  "${OGRE}/OgreMain/src/OgreMeshManager.cpp"
  "${OGRE}/OgreMain/src/OgreMeshSerializer.cpp"
  "${OGRE}/OgreMain/src/OgreMeshSerializerImpl.cpp"
  "${OGRE}/OgreMain/src/OgreMovableObject.cpp"
  "${OGRE}/OgreMain/src/OgreMovablePlane.cpp"
  "${OGRE}/OgreMain/src/OgreNode.cpp"
  "${OGRE}/OgreMain/src/OgreNumerics.cpp"
  "${OGRE}/OgreMain/src/OgreOptimisedUtil.cpp"
  "${OGRE}/OgreMain/src/OgreOptimisedUtilGeneral.cpp"
  "${OGRE}/OgreMain/src/OgreOptimisedUtilSSE.cpp"
  "${OGRE}/OgreMain/src/OgreOverlay.cpp"
  "${OGRE}/OgreMain/src/OgreOverlayContainer.cpp"
  "${OGRE}/OgreMain/src/OgreOverlayElement.cpp"
  "${OGRE}/OgreMain/src/OgreOverlayElementCommands.cpp"
  "${OGRE}/OgreMain/src/OgreOverlayManager.cpp"
  "${OGRE}/OgreMain/src/OgrePanelOverlayElement.cpp"
  "${OGRE}/OgreMain/src/OgreParticle.cpp"
  "${OGRE}/OgreMain/src/OgreParticleEmitter.cpp"
  "${OGRE}/OgreMain/src/OgreParticleEmitterCommands.cpp"
  "${OGRE}/OgreMain/src/OgreParticleIterator.cpp"
  "${OGRE}/OgreMain/src/OgreParticleSystem.cpp"
  "${OGRE}/OgreMain/src/OgreParticleSystemManager.cpp"
  "${OGRE}/OgreMain/src/OgrePass.cpp"
  "${OGRE}/OgreMain/src/OgrePatchMesh.cpp"
  "${OGRE}/OgreMain/src/OgrePatchSurface.cpp"
  "${OGRE}/OgreMain/src/OgrePixelFormat.cpp"
  "${OGRE}/OgreMain/src/OgrePlane.cpp"
  "${OGRE}/OgreMain/src/OgrePlatformInformation.cpp"
  "${OGRE}/OgreMain/src/OgrePolygon.cpp"
  "${OGRE}/OgreMain/src/OgrePose.cpp"
  "${OGRE}/OgreMain/src/OgrePrecompiledHeaders.cpp"
  "${OGRE}/OgreMain/src/OgrePredefinedControllers.cpp"
  "${OGRE}/OgreMain/src/OgrePrefabFactory.cpp"
  "${OGRE}/OgreMain/src/OgreProfiler.cpp"
  "${OGRE}/OgreMain/src/OgreProgressiveMesh.cpp"
  "${OGRE}/OgreMain/src/OgreQuaternion.cpp"
  "${OGRE}/OgreMain/src/OgreRectangle2D.cpp"
  "${OGRE}/OgreMain/src/OgreRenderQueue.cpp"
  "${OGRE}/OgreMain/src/OgreRenderQueueInvocation.cpp"
  "${OGRE}/OgreMain/src/OgreRenderQueueSortingGrouping.cpp"
  "${OGRE}/OgreMain/src/OgreRenderSystem.cpp"
  "${OGRE}/OgreMain/src/OgreRenderSystemCapabilities.cpp"
  "${OGRE}/OgreMain/src/OgreRenderTarget.cpp"
  "${OGRE}/OgreMain/src/OgreRenderTexture.cpp"
  "${OGRE}/OgreMain/src/OgreRenderWindow.cpp"
  "${OGRE}/OgreMain/src/OgreResource.cpp"
  "${OGRE}/OgreMain/src/OgreResourceBackgroundQueue.cpp"
  "${OGRE}/OgreMain/src/OgreResourceGroupManager.cpp"
  "${OGRE}/OgreMain/src/OgreResourceManager.cpp"
  "${OGRE}/OgreMain/src/OgreRibbonTrail.cpp"
  "${OGRE}/OgreMain/src/OgreRoot.cpp"
  "${OGRE}/OgreMain/src/OgreRotationSpline.cpp"
  "${OGRE}/OgreMain/src/OgreSceneManager.cpp"
  "${OGRE}/OgreMain/src/OgreSceneManagerEnumerator.cpp"
  "${OGRE}/OgreMain/src/OgreSceneNode.cpp"
  "${OGRE}/OgreMain/src/OgreSceneQuery.cpp"
  "${OGRE}/OgreMain/src/OgreSerializer.cpp"
  "${OGRE}/OgreMain/src/OgreShadowCameraSetup.cpp"
  "${OGRE}/OgreMain/src/OgreShadowCameraSetupFocused.cpp"
  "${OGRE}/OgreMain/src/OgreShadowCameraSetupLiSPSM.cpp"
  "${OGRE}/OgreMain/src/OgreShadowCameraSetupPlaneOptimal.cpp"
  "${OGRE}/OgreMain/src/OgreShadowCaster.cpp"
  "${OGRE}/OgreMain/src/OgreShadowTextureManager.cpp"
  "${OGRE}/OgreMain/src/OgreShadowVolumeExtrudeProgram.cpp"
  "${OGRE}/OgreMain/src/OgreSimpleRenderable.cpp"
  "${OGRE}/OgreMain/src/OgreSimpleSpline.cpp"
  "${OGRE}/OgreMain/src/OgreSkeleton.cpp"
  "${OGRE}/OgreMain/src/OgreSkeletonInstance.cpp"
  "${OGRE}/OgreMain/src/OgreSkeletonManager.cpp"
  "${OGRE}/OgreMain/src/OgreSkeletonSerializer.cpp"
  "${OGRE}/OgreMain/src/OgreStaticGeometry.cpp"
  "${OGRE}/OgreMain/src/OgreString.cpp"
  "${OGRE}/OgreMain/src/OgreStringConverter.cpp"
  "${OGRE}/OgreMain/src/OgreStringInterface.cpp"
  "${OGRE}/OgreMain/src/OgreSubEntity.cpp"
  "${OGRE}/OgreMain/src/OgreSubMesh.cpp"
  "${OGRE}/OgreMain/src/OgreTagPoint.cpp"
  "${OGRE}/OgreMain/src/OgreTechnique.cpp"
  "${OGRE}/OgreMain/src/OgreTextAreaOverlayElement.cpp"
  "${OGRE}/OgreMain/src/OgreTexture.cpp"
  "${OGRE}/OgreMain/src/OgreTextureManager.cpp"
  "${OGRE}/OgreMain/src/OgreTextureUnitState.cpp"
  "${OGRE}/OgreMain/src/WIN32/OgreTimer.cpp"
  "${OGRE}/OgreMain/src/OgreUnifiedHighLevelGpuProgram.cpp"
  "${OGRE}/OgreMain/src/OgreUserDefinedObject.cpp"
  "${OGRE}/OgreMain/src/OgreVector2.cpp"
  "${OGRE}/OgreMain/src/OgreVector3.cpp"
  "${OGRE}/OgreMain/src/OgreVector4.cpp"
  "${OGRE}/OgreMain/src/OgreVertexIndexData.cpp"
  "${OGRE}/OgreMain/src/OgreViewport.cpp"
  "${OGRE}/OgreMain/src/OgreWindowEventUtilities.cpp"
  "${OGRE}/OgreMain/src/OgreWireBoundingBox.cpp"
  "${OGRE}/OgreMain/src/OgreZip.cpp"
  "${OGRE}/OgreMain/src/WIN32/OgreWin32Resources.rc"
)

set(OPENRBX_PARTICLEFX_SOURCES
  "${OGRE}/PlugIns/ParticleFX/src/OgreAreaEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreBoxEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreColourFaderAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreColourFaderAffector2.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreColourImageAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreColourInterpolatorAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreCylinderEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreDeflectorPlaneAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreDirectionRandomiserAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreEllipsoidEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreHollowEllipsoidEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreLinearForceAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreParticleFX.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreParticleFXPlugin.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgrePointEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreRingEmitter.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreRotationAffector.cpp"
  "${OGRE}/PlugIns/ParticleFX/src/OgreScaleAffector.cpp"
)

set(OPENRBX_GL_SOURCES
  "${OGRE}/RenderSystems/GL/src/glew.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLATIFSInit.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLContext.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLDefaultHardwareBufferManager.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLEngineDll.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLFBOMultiRenderTarget.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLFBORenderTexture.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLFrameBufferObject.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLGpuNvparseProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLGpuProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLGpuProgramManager.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLHardwareBufferManager.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLHardwareIndexBuffer.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLHardwareOcclusionQuery.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLHardwarePixelBuffer.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLHardwareVertexBuffer.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLPBRenderTexture.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLPixelFormat.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLPlugin.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLRenderSystem.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLRenderTexture.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLSupport.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLTexture.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreGLTextureManager.cpp"
  "${OGRE}/RenderSystems/GL/src/win32/OgreWin32Context.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreWin32GLSupport.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreWin32RenderTexture.cpp"
  "${OGRE}/RenderSystems/GL/src/OgreWin32Window.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_ps1.0_lexer.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_ps1.0_parser.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_rc1.0_lexer.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_rc1.0_parser.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_ts1.0_lexer.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_ts1.0_parser.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_vs1.0_lexer.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/_vs1.0_parser.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/avp1.0_impl.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/nvparse.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/nvparse_errors.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/ps1.0_program.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/rc1.0_combiners.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/rc1.0_final.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/rc1.0_general.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/ts1.0_inst.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/ts1.0_inst_list.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/vcp1.0_impl.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/vp1.0_impl.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/vs1.0_inst.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/vs1.0_inst_list.cpp"
  "${OGRE}/RenderSystems/GL/src/nvparse/vsp1.0_impl.cpp"
  "${OGRE}/RenderSystems/GL/src/atifs/src/ATI_FS_GLGpuProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/atifs/src/Compiler2Pass.cpp"
  "${OGRE}/RenderSystems/GL/src/atifs/src/ps_1_4.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLExtSupport.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLGpuProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLLinkProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLLinkProgramManager.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLProgram.cpp"
  "${OGRE}/RenderSystems/GL/src/GLSL/src/OgreGLSLProgramFactory.cpp"
)

set(FREEIMAGE "${OGRE}/Dependencies/src/FreeImage/Source")
set(FREETYPE "${OGRE}/Dependencies/src/freetype-2.3.0")
set(ZZIPLIB "${OGRE}/Dependencies/src/zziplib-0.13.49")

file(GLOB OPENRBX_ZLIB_SOURCES "${FREEIMAGE}/ZLib/*.c")
file(GLOB OPENRBX_LIBJPEG_SOURCES "${FREEIMAGE}/LibJPEG/*.c")
file(GLOB OPENRBX_LIBPNG_SOURCES "${FREEIMAGE}/LibPNG/*.c")
file(GLOB OPENRBX_LIBTIFF_SOURCES "${FREEIMAGE}/LibTIFF/*.c")
file(GLOB OPENRBX_LIBMNG_SOURCES "${FREEIMAGE}/LibMNG/*.c")
file(GLOB OPENRBX_FREEIMAGE_SOURCES
  "${FREEIMAGE}/FreeImage/*.cpp"
  "${FREEIMAGE}/FreeImageToolkit/*.cpp"
  "${FREEIMAGE}/Metadata/*.cpp"
  "${FREEIMAGE}/DeprecationManager/*.cpp"
)
file(GLOB OPENRBX_ZZIPLIB_SOURCES "${ZZIPLIB}/zzip/*.c")


set(OPENRBX_FREETYPE_SOURCES
  "${FREETYPE}/src/autofit/autofit.c"
  "${FREETYPE}/src/bdf/bdf.c"
  "${FREETYPE}/src/cff/cff.c"
  "${FREETYPE}/src/base/ftbase.c"
  "${FREETYPE}/src/base/ftbitmap.c"
  "${FREETYPE}/src/base/ftgasp.c"
  "${FREETYPE}/src/cache/ftcache.c"
  "${FREETYPE}/builds/win32/ftdebug.c"
  "${FREETYPE}/src/base/ftglyph.c"
  "${FREETYPE}/src/gzip/ftgzip.c"
  "${FREETYPE}/src/base/ftinit.c"
  "${FREETYPE}/src/lzw/ftlzw.c"
  "${FREETYPE}/src/base/ftstroke.c"
  "${FREETYPE}/src/base/ftsystem.c"
  "${FREETYPE}/src/smooth/smooth.c"
  "${FREETYPE}/src/base/ftbbox.c"
  "${FREETYPE}/src/base/ftmm.c"
  "${FREETYPE}/src/base/ftpfr.c"
  "${FREETYPE}/src/base/ftsynth.c"
  "${FREETYPE}/src/base/fttype1.c"
  "${FREETYPE}/src/base/ftwinfnt.c"
  "${FREETYPE}/src/pcf/pcf.c"
  "${FREETYPE}/src/pfr/pfr.c"
  "${FREETYPE}/src/psaux/psaux.c"
  "${FREETYPE}/src/pshinter/pshinter.c"
  "${FREETYPE}/src/psnames/psmodule.c"
  "${FREETYPE}/src/raster/raster.c"
  "${FREETYPE}/src/sfnt/sfnt.c"
  "${FREETYPE}/src/truetype/truetype.c"
  "${FREETYPE}/src/type1/type1.c"
  "${FREETYPE}/src/cid/type1cid.c"
  "${FREETYPE}/src/type42/type42.c"
  "${FREETYPE}/src/winfonts/winfnt.c"
)

if (NOT ANDROID)
foreach(option SDL_SHARED SDL_TEST_LIBRARY SDL_TESTS SDL_EXAMPLES SDL_INSTALL SDL_UNINSTALL
               SDL_AUDIO SDL_GPU SDL_RENDER SDL_CAMERA SDL_JOYSTICK SDL_HAPTIC SDL_HIDAPI SDL_POWER SDL_SENSOR
               SDL_DIALOG SDL_TRAY SDL_WAYLAND SDL_KMSDRM SDL_VULKAN SDL_OPENGLES SDL_OFFSCREEN SDL_PIPEWIRE
               SDL_LIBUDEV SDL_DBUS SDL_IBUS SDL_LIBURING SDL_FRIBIDI SDL_LIBTHAI)
  set(${option} OFF CACHE BOOL "" FORCE)
endforeach()
set(SDL_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(SDL3 EXCLUDE_FROM_ALL)
endif()

find_package(Perl REQUIRED)

set(OPENRBX_OPENSSL_BINARY_DIR "${CMAKE_BINARY_DIR}/3rdparty/openssl-build")
set(OPENRBX_OPENSSL_PREFIX "${OPENRBX_OPENSSL_BINARY_DIR}/install")
if (NOT WIN32)
  set(ENV{PKG_CONFIG_PATH} "${OPENRBX_OPENSSL_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

if (NOT EXISTS "${OPENRBX_OPENSSL_PREFIX}/include/openssl/ssl.h")
  message(STATUS "Building OpenSSL in ${OPENRBX_OPENSSL_BINARY_DIR}")
  set(OPENRBX_OPENSSL_ENV_ARGS)
  if (WIN32)
    set(OPENRBX_OPENSSL_TARGET VC-WIN64A no-asm)
    set(OPENRBX_OPENSSL_MAKE nmake)
  elseif (ANDROID)
    cmake_host_system_information(RESULT OPENRBX_JOBS QUERY NUMBER_OF_LOGICAL_CORES)
    if (CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
      set(OPENRBX_OPENSSL_ARCH android-arm64)
    elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
      set(OPENRBX_OPENSSL_ARCH android-arm)
    elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
      set(OPENRBX_OPENSSL_ARCH android-x86_64)
    elseif (CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
      set(OPENRBX_OPENSSL_ARCH android-x86)
    else()
      message(FATAL_ERROR "Unsupported Android ABI for OpenSSL: ${CMAKE_ANDROID_ARCH_ABI}")
    endif()
    set(OPENRBX_ANDROID_API "${CMAKE_SYSTEM_VERSION}")
    if (NOT OPENRBX_ANDROID_API)
      set(OPENRBX_ANDROID_API 24)
    endif()
    get_filename_component(OPENRBX_NDK_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    set(OPENRBX_NDK_ROOT "${CMAKE_ANDROID_NDK}")
    if (NOT OPENRBX_NDK_ROOT)
      set(OPENRBX_NDK_ROOT "$ENV{ANDROID_NDK_ROOT}")
    endif()
    set(OPENRBX_OPENSSL_TARGET ${OPENRBX_OPENSSL_ARCH} -D__ANDROID_API__=${OPENRBX_ANDROID_API})
    set(OPENRBX_OPENSSL_ENV_ARGS
      "${CMAKE_COMMAND}" -E env
      "ANDROID_NDK_ROOT=${OPENRBX_NDK_ROOT}"
      "PATH=${OPENRBX_NDK_BIN_DIR}:$ENV{PATH}"
    )
    set(OPENRBX_OPENSSL_MAKE make -j${OPENRBX_JOBS})
  else()
    cmake_host_system_information(RESULT OPENRBX_JOBS QUERY NUMBER_OF_LOGICAL_CORES)
    set(OPENRBX_OPENSSL_TARGET)
    set(OPENRBX_OPENSSL_MAKE make -j${OPENRBX_JOBS})
  endif()
  file(MAKE_DIRECTORY "${OPENRBX_OPENSSL_BINARY_DIR}")
  execute_process(
    COMMAND ${OPENRBX_OPENSSL_ENV_ARGS} "${PERL_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/openssl/Configure"
            ${OPENRBX_OPENSSL_TARGET} no-shared no-tests no-docs no-apps
            "--prefix=${OPENRBX_OPENSSL_PREFIX}" --libdir=lib
    WORKING_DIRECTORY "${OPENRBX_OPENSSL_BINARY_DIR}"
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "OpenSSL configure failed")
  endif()
  execute_process(
    COMMAND ${OPENRBX_OPENSSL_ENV_ARGS} ${OPENRBX_OPENSSL_MAKE} build_libs
    WORKING_DIRECTORY "${OPENRBX_OPENSSL_BINARY_DIR}"
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "OpenSSL build failed")
  endif()
  execute_process(
    COMMAND ${OPENRBX_OPENSSL_ENV_ARGS} ${OPENRBX_OPENSSL_MAKE} install_dev
    WORKING_DIRECTORY "${OPENRBX_OPENSSL_BINARY_DIR}"
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "OpenSSL install failed")
  endif()
endif()

set(OPENRBX_CURL_BINARY_DIR "${CMAKE_BINARY_DIR}/3rdparty/curl-build")
set(OPENRBX_CURL_PREFIX "${OPENRBX_CURL_BINARY_DIR}/install")

if (NOT EXISTS "${OPENRBX_CURL_PREFIX}/lib/cmake/CURL/CURLConfig.cmake")
  message(STATUS "Building curl in ${OPENRBX_CURL_BINARY_DIR}")
  set(OPENRBX_CURL_EXTRA_ARGS)
  if (ANDROID)
    list(APPEND OPENRBX_CURL_EXTRA_ARGS
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DANDROID_ABI=${CMAKE_ANDROID_ARCH_ABI}
      -DANDROID_PLATFORM=android-${CMAKE_SYSTEM_VERSION}
      -DOPENSSL_INCLUDE_DIR=${OPENRBX_OPENSSL_PREFIX}/include
      -DOPENSSL_SSL_LIBRARY=${OPENRBX_OPENSSL_PREFIX}/lib/libssl.a
      -DOPENSSL_CRYPTO_LIBRARY=${OPENRBX_OPENSSL_PREFIX}/lib/libcrypto.a
    )
  endif()
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_CURRENT_SOURCE_DIR}/curl" -B "${OPENRBX_CURL_BINARY_DIR}" -G Ninja
            -DCMAKE_BUILD_TYPE=Release
            -DCMAKE_INSTALL_PREFIX=${OPENRBX_CURL_PREFIX}
            -DCMAKE_INSTALL_LIBDIR=lib
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_DISABLE_FIND_PACKAGE_Perl=ON
            -DBUILD_SHARED_LIBS=OFF
            -DBUILD_STATIC_LIBS=ON
            -DBUILD_CURL_EXE=OFF
            -DBUILD_TESTING=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_LIBCURL_DOCS=OFF
            -DBUILD_MISC_DOCS=OFF
            -DENABLE_CURL_MANUAL=OFF
            -DCURL_USE_PKGCONFIG=OFF
            -DCURL_USE_OPENSSL=ON
            -DOPENSSL_ROOT_DIR=${OPENRBX_OPENSSL_PREFIX}
            -DOPENSSL_USE_STATIC_LIBS=ON
            -DCURL_CA_BUNDLE=none
            -DCURL_CA_PATH=none
            -DCURL_ZLIB=OFF
            -DCURL_BROTLI=OFF
            -DCURL_ZSTD=OFF
            -DUSE_NGHTTP2=OFF
            -DUSE_LIBIDN2=OFF
            -DCURL_USE_LIBPSL=OFF
            -DCURL_USE_LIBSSH2=OFF
            -DCURL_DISABLE_LDAP=ON
            ${OPENRBX_CURL_EXTRA_ARGS}
    RESULT_VARIABLE result
  )
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "curl configure failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --build "${OPENRBX_CURL_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "curl build failed")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --install "${OPENRBX_CURL_BINARY_DIR}" RESULT_VARIABLE result)
  if (NOT result EQUAL 0)
    message(FATAL_ERROR "curl install failed")
  endif()
endif()

set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ON)
set(OPENSSL_ROOT_DIR "${OPENRBX_OPENSSL_PREFIX}")
set(OPENSSL_USE_STATIC_LIBS ON)
if (ANDROID)
  set(OPENSSL_INCLUDE_DIR "${OPENRBX_OPENSSL_PREFIX}/include")
  set(OPENSSL_SSL_LIBRARY "${OPENRBX_OPENSSL_PREFIX}/lib/libssl.a")
  set(OPENSSL_CRYPTO_LIBRARY "${OPENRBX_OPENSSL_PREFIX}/lib/libcrypto.a")
endif()
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED CONFIG PATHS "${OPENRBX_CURL_PREFIX}" NO_DEFAULT_PATH)
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL OFF)

if (ANDROID)
  return()
endif()

if (NOT WIN32)
  set(OPENRBX_OGRE_LINUX_DEFINITIONS NDEBUG OGRE_GUI_GLX ZZIP_OMIT_CONFIG_H _LARGEFILE64_SOURCE XTSTRINGDEFINES)
  set(OPENRBX_ZZIP_LINUX_DEFINITIONS
    _LARGEFILE64_SOURCE ZZIP_OMIT_CONFIG_H ZZIP_HAVE_UNISTD_H ZZIP_HAVE_STDINT_H ZZIP_HAVE_STRING_H ZZIP_HAVE_STDLIB_H
    ZZIP_HAVE_SYS_STAT_H ZZIP_HAVE_SYS_TYPES_H ZZIP_HAVE_DIRENT_H ZZIP_HAVE_SYS_MMAN_H ZZIP_HAVE_ZLIB_H
    ZZIP_HAVE_STRCASECMP
  )

  add_library(zlib STATIC ${OPENRBX_ZLIB_SOURCES})
  add_library(libjpeg STATIC ${OPENRBX_LIBJPEG_SOURCES})
  add_library(libpng STATIC ${OPENRBX_LIBPNG_SOURCES})
  add_library(libtiff STATIC ${OPENRBX_LIBTIFF_SOURCES})
  add_library(libmng STATIC ${OPENRBX_LIBMNG_SOURCES})
  add_library(freeimage STATIC ${OPENRBX_FREEIMAGE_SOURCES})
  target_include_directories(libpng PRIVATE "${FREEIMAGE}/ZLib")
  target_compile_definitions(libpng PRIVATE PNG_NO_MMX_CODE)
  target_include_directories(libtiff PRIVATE "${FREEIMAGE}/ZLib")
  target_include_directories(libmng PRIVATE "${FREEIMAGE}/ZLib" "${FREEIMAGE}/LibJPEG")
  target_compile_options(libmng PRIVATE -Wno-incompatible-pointer-types)
  target_include_directories(freeimage PRIVATE "${FREEIMAGE}" "${FREEIMAGE}/ZLib" "${FREEIMAGE}/DeprecationManager")
  target_compile_definitions(freeimage PRIVATE NDEBUG FREEIMAGE_LIB)
  target_link_libraries(freeimage PRIVATE zlib libjpeg libpng libtiff libmng)

  add_library(freetype STATIC ${OPENRBX_FREETYPE_SOURCES})
  target_include_directories(freetype PRIVATE "${FREETYPE}/include")
  target_compile_definitions(freetype PRIVATE NDEBUG FT2_BUILD_LIBRARY)

  add_library(zziplib STATIC ${OPENRBX_ZZIPLIB_SOURCES})
  target_include_directories(zziplib PRIVATE "${ZZIPLIB}" "${FREEIMAGE}/ZLib")
  target_compile_definitions(zziplib PRIVATE NDEBUG ${OPENRBX_ZZIP_LINUX_DEFINITIONS})
  target_link_libraries(zziplib PRIVATE zlib)

  set_target_properties(zlib libjpeg libpng libtiff libmng freeimage freetype zziplib PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_STANDARD 99
    CXX_STANDARD 14
  )

  set(OPENRBX_OGREMAIN_LINUX_SOURCES ${OPENRBX_OGREMAIN_SOURCES})
  list(FILTER OPENRBX_OGREMAIN_LINUX_SOURCES EXCLUDE REGEX "/WIN32/")
  list(APPEND OPENRBX_OGREMAIN_LINUX_SOURCES
    "${OGRE}/OgreMain/src/GLX/OgreConfigDialog.cpp"
    "${OGRE}/OgreMain/src/GLX/OgreErrorDialog.cpp"
    "${OGRE}/OgreMain/src/GLX/OgreTimer.cpp"
    "${OGRE}/OgreMain/src/OgreSearchOps.cpp"
  )
  add_library(rgmain SHARED ${OPENRBX_OGREMAIN_LINUX_SOURCES})
  target_include_directories(rgmain PRIVATE
    "${OGRE}/OgreMain/include"
    "${FREEIMAGE}"
    "${FREETYPE}/include"
    "${ZZIPLIB}"
  )
  target_compile_definitions(rgmain PRIVATE ${OPENRBX_OGRE_LINUX_DEFINITIONS} OGRE_NONCLIENT_BUILD FREEIMAGE_LIB)
  target_link_libraries(rgmain PRIVATE zziplib zlib freetype freeimage X11::X11 X11::Xt X11::Xaw ${CMAKE_DL_LIBS})

  add_library(rgpar SHARED ${OPENRBX_PARTICLEFX_SOURCES})
  target_include_directories(rgpar PRIVATE
    "${OGRE}/PlugIns/ParticleFX/include"
    "${OGRE}/OgreMain/include"
  )
  target_compile_definitions(rgpar PRIVATE ${OPENRBX_OGRE_LINUX_DEFINITIONS})
  target_link_libraries(rgpar PRIVATE rgmain)

  set(OPENRBX_GL_LINUX_SOURCES ${OPENRBX_GL_SOURCES})
  list(FILTER OPENRBX_GL_LINUX_SOURCES EXCLUDE REGEX "/OgreWin32|/win32/")
  list(APPEND OPENRBX_GL_LINUX_SOURCES
    "${OGRE}/RenderSystems/GL/src/GLX/OgreGLXContext.cpp"
    "${OGRE}/RenderSystems/GL/src/GLX/OgreGLXGLSupport.cpp"
    "${OGRE}/RenderSystems/GL/src/GLX/OgreGLXRenderTexture.cpp"
    "${OGRE}/RenderSystems/GL/src/GLX/OgreGLXUtils.cpp"
    "${OGRE}/RenderSystems/GL/src/GLX/OgreGLXWindow.cpp"
  )
  add_library(rggl SHARED ${OPENRBX_GL_LINUX_SOURCES})
  target_include_directories(rggl PRIVATE
    "${OGRE}/RenderSystems/GL/include"
    "${OGRE}/RenderSystems/GL/include/GLX"
    "${OGRE}/RenderSystems/GL/src/GLX"
    "${OGRE}/OgreMain/include"
    "${OGRE}/RenderSystems/GL/src/nvparse"
    "${OGRE}/RenderSystems/GL/src/atifs/include"
    "${OGRE}/RenderSystems/GL/src/GLSL/include"
  )
  target_compile_definitions(rggl PRIVATE ${OPENRBX_OGRE_LINUX_DEFINITIONS})
  target_compile_options(rggl PRIVATE -Wno-narrowing)
  target_link_libraries(rggl PRIVATE rgmain glu OpenGL::GL X11::X11 X11::Xrandr)

  set_target_properties(rgmain rgpar rggl PROPERTIES
    PREFIX ""
    CXX_STANDARD 14
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "$ORIGIN"
  )
  target_link_options(rgmain PRIVATE "LINKER:--exclude-libs,ALL")

  return()
endif()

set(DXSDK "${PROJECT_SOURCE_DIR}/3rdparty/Microsoft DirectX 9.0 SDK (October 2004)")
set(DXSDK_LIB "${DXSDK}/Lib/x64")

set(OPENRBX_D3D9_SOURCES
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9Driver.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9DriverList.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9EngineDll.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9GpuProgram.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9GpuProgramManager.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HardwareBufferManager.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HardwareIndexBuffer.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HardwareOcclusionQuery.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HardwarePixelBuffer.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HardwareVertexBuffer.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HLSLProgram.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9HLSLProgramFactory.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9Mappings.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9MultiRenderTarget.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9Plugin.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9RenderSystem.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9RenderWindow.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9Texture.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9TextureManager.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9VertexDeclaration.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9VideoMode.cpp"
  "${OGRE}/RenderSystems/Direct3D9/src/OgreD3D9VideoModeList.cpp"
)

set(OPENRBX_OGRE_COMPILE_OPTIONS /Ob2 /Oi /Ot /Oy /GT /GF /EHsc /GS- /Gy /fp:fast /GR /W3)
set(OPENRBX_OGRE_LINK_OPTIONS /INCREMENTAL:NO /OPT:REF /OPT:ICF)

add_library(zlib STATIC ${OPENRBX_ZLIB_SOURCES})
add_library(libjpeg STATIC ${OPENRBX_LIBJPEG_SOURCES})
add_library(libpng STATIC ${OPENRBX_LIBPNG_SOURCES})
add_library(libtiff STATIC ${OPENRBX_LIBTIFF_SOURCES})
add_library(libmng STATIC ${OPENRBX_LIBMNG_SOURCES})
add_library(freeimage STATIC ${OPENRBX_FREEIMAGE_SOURCES})
foreach(name zlib libjpeg libpng libtiff libmng)
  target_compile_definitions(${name} PRIVATE WIN32 NDEBUG _LIB)
  target_compile_options(${name} PRIVATE /O2 /Ob2)
endforeach()
target_include_directories(libpng PRIVATE "${FREEIMAGE}/ZLib")
target_include_directories(libtiff PRIVATE "${FREEIMAGE}/ZLib")
target_include_directories(libmng PRIVATE "${FREEIMAGE}/ZLib" "${FREEIMAGE}/LibJPEG")
target_include_directories(freeimage PRIVATE "${FREEIMAGE}" "${FREEIMAGE}/ZLib" "${FREEIMAGE}/DeprecationManager")
target_compile_definitions(freeimage PRIVATE WIN32 NDEBUG FREEIMAGE_LIB _SECURE_NO_DEPRECATE)
target_compile_options(freeimage PRIVATE /O2 /Ob2 /EHsc)
target_link_libraries(freeimage PRIVATE zlib libjpeg libpng libtiff libmng)

add_library(freetype STATIC ${OPENRBX_FREETYPE_SOURCES})
target_include_directories(freetype PRIVATE "${FREETYPE}/include")
target_compile_definitions(freetype PRIVATE NDEBUG WIN32 _LIB FT2_BUILD_LIBRARY)
target_compile_options(freetype PRIVATE /O2 /Ob1)

add_library(zziplib STATIC ${OPENRBX_ZZIPLIB_SOURCES})
target_include_directories(zziplib PRIVATE "${ZZIPLIB}" "${FREEIMAGE}/ZLib")
target_compile_definitions(zziplib PRIVATE WIN32 NDEBUG _LIB)
target_compile_options(zziplib PRIVATE /O2 /Ob1)
target_link_libraries(zziplib PRIVATE zlib)

set_target_properties(zlib libjpeg libpng libtiff libmng freeimage freetype zziplib PROPERTIES
  MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
)

add_library(rgmain SHARED ${OPENRBX_OGREMAIN_SOURCES})
target_include_directories(rgmain PRIVATE
  "${OGRE}/OgreMain/include"
  "${FREEIMAGE}"
  "${FREETYPE}/include"
  "${ZZIPLIB}"
)
target_compile_definitions(rgmain PRIVATE WIN32 NDEBUG _USRDLL _MT OGRE_NONCLIENT_BUILD FREEIMAGE_LIB _MBCS)
target_compile_options(rgmain PRIVATE /O2 ${OPENRBX_OGRE_COMPILE_OPTIONS})
if (NOT CMAKE_CXX_COMPILER_LAUNCHER)
  target_precompile_headers(rgmain PRIVATE "${OGRE}/OgreMain/include/OgreStableHeaders.h")
endif()
target_link_options(rgmain PRIVATE ${OPENRBX_OGRE_LINK_OPTIONS} /SUBSYSTEM:CONSOLE)
target_link_libraries(rgmain PRIVATE zziplib zlib freetype freeimage)

add_library(rgpar SHARED ${OPENRBX_PARTICLEFX_SOURCES})
target_include_directories(rgpar PRIVATE
  "${OGRE}/PlugIns/ParticleFX/include"
  "${OGRE}/OgreMain/include"
)
target_compile_definitions(rgpar PRIVATE WIN32 NDEBUG _WINDOWS _USRDLL OGRE_PARTICLEFXPLUGIN_EXPORTS)
target_compile_options(rgpar PRIVATE /O2 ${OPENRBX_OGRE_COMPILE_OPTIONS} /GL)
target_link_options(rgpar PRIVATE ${OPENRBX_OGRE_LINK_OPTIONS} /SUBSYSTEM:WINDOWS /LTCG)
target_link_libraries(rgpar PRIVATE rgmain)

add_library(rggl SHARED ${OPENRBX_GL_SOURCES})
target_include_directories(rggl PRIVATE
  "${OGRE}/RenderSystems/GL/include"
  "${OGRE}/RenderSystems/GL/src/win32"
  "${OGRE}/OgreMain/include"
  "${OGRE}/RenderSystems/GL/src/nvparse"
  "${OGRE}/RenderSystems/GL/src/nvparse/winheaders"
  "${OGRE}/RenderSystems/GL/src/atifs/include"
  "${OGRE}/RenderSystems/GL/src/GLSL/include"
)
target_compile_definitions(rggl PRIVATE WIN32 NDEBUG _WINDOWS _USRDLL OGRE_GLPLUGIN_EXPORTS _MBCS)
target_compile_options(rggl PRIVATE /Ox ${OPENRBX_OGRE_COMPILE_OPTIONS} /GL)
target_link_options(rggl PRIVATE ${OPENRBX_OGRE_LINK_OPTIONS} /SUBSYSTEM:WINDOWS /LTCG)
target_link_libraries(rggl PRIVATE rgmain opengl32.lib glu32.lib)

add_library(rgdx SHARED ${OPENRBX_D3D9_SOURCES})
target_include_directories(rgdx PRIVATE
  "${OGRE}/RenderSystems/Direct3D9/include"
  "${OGRE}/OgreMain/include"
  "${DXSDK}/Include"
)
target_compile_definitions(rgdx PRIVATE WIN32 NDEBUG _WINDOWS _USRDLL OGRED3DENGINEDLL_EXPORTS _MBCS)
target_compile_options(rgdx PRIVATE /Ox ${OPENRBX_OGRE_COMPILE_OPTIONS} /GL)
target_link_options(rgdx PRIVATE ${OPENRBX_OGRE_LINK_OPTIONS} /SUBSYSTEM:WINDOWS /LTCG)
target_link_directories(rgdx PRIVATE "${DXSDK_LIB}")
target_link_libraries(rgdx PRIVATE rgmain d3d9 d3dx9 dxerr9 dxguid legacy_stdio_definitions)

set_target_properties(rgmain rgpar rggl rgdx PROPERTIES
  VERSION 0.99
  MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
)
