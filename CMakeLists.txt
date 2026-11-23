cmake_minimum_required(VERSION 3.25...4.2 FATAL_ERROR)

cmake_policy(SET CMP0091 NEW)

project(openrbx C CXX)

include(CheckCXXSourceCompiles)
include(CMakeDependentOption)
include(CMakePushCheckState)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
option(ENABLE_CLANG_TIDY "Enable clang-tidy")
if (ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy")
  set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_BIN}")
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_BIN}" "--extra-arg=/EHsc" "--extra-arg=/std:c++latest")

  set_source_files_properties(Client/App/v8world/Contact.cpp PROPERTIES SKIP_LINTING ON)
endif()

if (NOT ANDROID AND NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "OpenRBX only builds 64-bit binaries")
endif()

function(add_cxx_warning WARNING)
  if (OPENRBX_WERROR)
    set(compiler_option "-Werror=${WARNING}")
  else()
    set(compiler_option "-W${WARNING}")
  endif()
  string(MAKE_C_IDENTIFIER "COMPILER_SUPPORTS${compiler_option}" varname)

  cmake_push_check_state(RESET)
  set(CMAKE_REQUIRED_FLAGS "${compiler_option} ")
  if (MSVC)
    string(APPEND CMAKE_REQUIRED_FLAGS "/WX")
  else()
    string(APPEND CMAKE_REQUIRED_FLAGS "-Werror")
  endif()
  check_cxx_source_compiles("int main() { return 0; }" ${varname})
  cmake_pop_check_state()

  if (${varname})
    set(OPENRBX_WARNING_OPTIONS ${OPENRBX_WARNING_OPTIONS} ${compiler_option} PARENT_SCOPE)
  endif()
endfunction()

function(openrbx_compile_defaults NAME)
  if (OPENRBX_WARNING_OPTIONS)
    target_compile_options(${NAME} PRIVATE ${OPENRBX_WARNING_OPTIONS})
  endif()
  if (MSVC)
    target_compile_options(${NAME} PRIVATE /W3)
    target_compile_definitions(${NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
  elseif (ANDROID)
    target_compile_options(${NAME} PRIVATE -Wno-c++11-narrowing -Wno-register -Wno-deprecated-declarations)
  endif()
endfunction()

option(OPENRBX_WERROR "Treat warnings as errors" OFF)

add_cxx_warning(parentheses)


set(OPENRBX_3RDPARTY "${PROJECT_SOURCE_DIR}/3rdparty")

set(OPENRBX_G3D_INCLUDE_DIR   "${PROJECT_SOURCE_DIR}/Client/Rendering/g3d/include")
set(OPENRBX_LUA_INCLUDE_DIR   "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src")
set(OPENRBX_G3D_EXTRA_INCLUDE_DIRS
  "${PROJECT_SOURCE_DIR}/Client/Rendering/g3d/png"
  "${PROJECT_SOURCE_DIR}/Client/Rendering/g3d/zlib"
)

set(OPENRBX_RUNTIME_FILES
  Client/win/AppSettings.xml
  Client/win/ReflectionMetadata.xml
  Client/win/copyrights.txt
)
if (NOT WIN32)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  if (NOT ANDROID)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)
    find_package(X11 REQUIRED)
  endif()
endif()
set(OPENRBX_RUNTIME_OUTPUTS)
foreach(file ${OPENRBX_RUNTIME_FILES})
  get_filename_component(name "${file}" NAME)
  add_custom_command(OUTPUT "${PROJECT_BINARY_DIR}/${name}"
    COMMAND "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/${file}" "${PROJECT_BINARY_DIR}"
    DEPENDS "${PROJECT_SOURCE_DIR}/${file}"
  )
  list(APPEND OPENRBX_RUNTIME_OUTPUTS "${PROJECT_BINARY_DIR}/${name}")
endforeach()
add_custom_target(runtime_files DEPENDS ${OPENRBX_RUNTIME_OUTPUTS})

add_subdirectory(3rdparty)

if (NOT ANDROID)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core5Compat)

file(GLOB OPENRBX_SCINTILLA_SOURCES
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/src/*.cxx"
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/qt/ScintillaEditBase/*.cpp"
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/qt/ScintillaEditBase/*.h"
  "${PROJECT_SOURCE_DIR}/3rdparty/lexilla/src/Lexilla.cxx"
  "${PROJECT_SOURCE_DIR}/3rdparty/lexilla/lexlib/*.cxx"
  "${PROJECT_SOURCE_DIR}/3rdparty/lexilla/lexers/*.cxx"
)
add_library(scintilla STATIC ${OPENRBX_SCINTILLA_SOURCES})
set_target_properties(scintilla PROPERTIES AUTOMOC ON CXX_STANDARD 17)
target_compile_definitions(scintilla PRIVATE SCINTILLA_QT=1 SCI_LEXER=1 LEXILLA_NO_EXPORT=1 _CRT_SECURE_NO_DEPRECATE=1)
target_compile_definitions(scintilla PUBLIC EXPORT_IMPORT_API=)
target_include_directories(scintilla PUBLIC
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/include"
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/src"
  "${PROJECT_SOURCE_DIR}/3rdparty/scintilla/qt/ScintillaEditBase"
  "${PROJECT_SOURCE_DIR}/3rdparty/lexilla/include"
)
target_include_directories(scintilla PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/lexilla/lexlib")
target_link_libraries(scintilla PUBLIC Qt6::Widgets Qt6::Core5Compat)
endif() # NOT ANDROID

set(OPENRBX_APP_SOURCES
  Client/App/security/SecurityContext.cpp
  Client/App/v8datamodel/GuiBuilder.cpp
  Client/App/v8datamodel/Hopper.cpp
  Client/App/gui/Button.cpp
  Client/App/gui/ChatOutput.cpp
  Client/App/gui/ChatWidget.cpp
  Client/App/gui/ArrowPanel.cpp
  Client/App/gui/EquationDisplay.cpp
  Client/App/gui/GUI.cpp
  Client/App/gui/HealthHud.cpp
  Client/App/gui/GuiDraw.cpp
  Client/App/gui/MenuItem.cpp
  Client/App/gui/ProfanityFilter.cpp
  Client/App/gui/ScoreHud.cpp
  Client/App/gui/Widget.cpp
  Client/App/humanoid/Humanoid.cpp
  Client/App/reflection/reflection_function.cpp
  Client/App/reflection/reflection_object.cpp
  Client/App/reflection/reflection_property.cpp
  Client/App/reflection/signal.cpp
  Client/App/reflection/type.cpp
  Client/App/util/Debug.cpp
  Client/App/v8tree/enumproperty.cpp
  Client/App/v8xml/XmlElement.cpp
  Client/App/v8xml/SerializerV2.cpp
  Client/App/v8xml/XmlSerializer.cpp
  Client/App/util/IRenderable.cpp
  Client/App/v8tree/Instance.cpp
  Client/App/v8tree/Verb.cpp
  Client/App/util/Thread.cpp
  Client/App/util/ExponentialRunningAverage.cpp
  Client/App/util/Extents.cpp
  Client/App/util/Face.cpp
  Client/App/util/FileSystem.cpp
  Client/App/util/FrameInterpolator.cpp
  Client/App/util/Guid.cpp
  Client/App/util/Handle.cpp
  Client/App/util/Hash.cpp
  Client/App/util/Http.cpp
  Client/App/util/IndexBox.cpp
  Client/App/util/Log.cpp
  Client/App/util/Math.cpp
  Client/App/util/MeshId.cpp
  Client/App/util/Name.cpp
  Client/App/util/NormalId.cpp
  Client/App/util/Quaternion.cpp
  Client/App/util/ContentProvider.cpp
  Client/App/util/Utilities.cpp
  Client/App/util/Profiling.cpp
  Client/App/util/Rect.cpp
  Client/App/util/RunningAverage.cpp
  Client/App/util/standardout.cpp
  Client/App/v8kernel/Body.cpp
  Client/App/util/Debug.cpp
  Client/App/v8datamodel/Accoutrement.cpp
  Client/App/v8datamodel/BrickColor.cpp
  Client/App/v8datamodel/custommesh.cpp
  Client/App/v8datamodel/Decal.cpp
  Client/App/v8datamodel/Enums.cpp
  Client/App/v8datamodel/TimeState.cpp
  Client/App/v8datamodel/IEquipable.cpp
  Client/App/v8datamodel/Team.cpp
  Client/App/v8datamodel/Flag.cpp
  Client/App/v8datamodel/FlagStand.cpp
  Client/App/v8datamodel/DebugSettings.cpp
  Client/App/v8datamodel/FaceInstance.cpp
  Client/App/v8datamodel/Feature.cpp
  Client/App/v8datamodel/ICharacterSubject.cpp
  Client/App/v8datamodel/ModelInstance.cpp
  Client/App/v8datamodel/RootInstance.cpp
  Client/App/v8datamodel/JointInstance.cpp
  Client/App/v8datamodel/Message.cpp
  Client/App/v8datamodel/PVInstance.cpp
  Client/App/v8datamodel/PartInstance.cpp
  Client/App/v8datamodel/SpawnLocation.cpp
  Client/App/v8datamodel/Tool.cpp
  Client/App/v8datamodel/Workspace.cpp
  Client/App/v8kernel/Cofm.cpp
  Client/App/v8kernel/Connector.cpp
  Client/App/v8kernel/Constants.cpp
  Client/App/v8kernel/Kernel.cpp
  Client/App/v8kernel/Link.cpp
  Client/App/v8kernel/Pair.cpp
  Client/App/v8kernel/Point.cpp
  Client/App/v8kernel/SimBody.cpp
  Client/App/v8world/Assembly2.cpp
  Client/App/v8world/AssemblyStage2.cpp
  Client/App/v8world/Ball.cpp
  Client/App/v8world/Block.cpp
  Client/App/v8world/Clump2.cpp
  Client/App/v8world/ClumpStage2.cpp
  Client/App/v8world/ContactManager.cpp
  Client/App/v8world/Contact.cpp
  Client/App/v8world/Controller.cpp
  Client/App/v8world/Edge.cpp
  Client/App/v8world/IMoving.cpp
  Client/App/v8world/GlueJoint.cpp
  Client/App/v8world/IPipelined.cpp
  Client/App/v8world/IWorldStage.cpp
  Client/App/v8world/Joint.cpp
  Client/App/v8world/JointBuilder.cpp
  Client/App/v8world/JointStage.cpp
  Client/App/v8world/MacroTypes.cpp
  Client/App/v8world/Mechanism.cpp
  Client/App/v8world/MotorJoint.cpp
  Client/App/v8world/MultiJoint.cpp
  Client/App/v8world/Primitive.cpp
  Client/App/v8world/SimJobStage.cpp
  Client/App/v8world/SleepStage2.cpp
  Client/App/v8world/RigidJoint.cpp
  Client/App/v8world/RotateJoint.cpp
  Client/App/v8world/SnapJoint.cpp
  Client/App/v8world/WeldJoint.cpp
  Client/App/v8world/SpatialHash.cpp
  Client/App/v8world/World.cpp
  Client/App/util/Timer.cpp
  Client/App/util/Units.cpp
  Client/App/util/UserInputBase.cpp
  Client/App/humanoid/Balancing.cpp
  Client/App/humanoid/FallingDown.cpp
  Client/App/humanoid/Flying.cpp
  Client/App/humanoid/Freefall.cpp
  Client/App/humanoid/GettingUp.cpp
  Client/App/humanoid/Jumping.cpp
  Client/App/humanoid/Running.cpp
  Client/App/humanoid/Seated.cpp
  Client/App/script/LuaAtomicClasses.cpp
  Client/App/script/LuaSignalBridge.cpp
  Client/App/script/LuaInstanceBridge.cpp
  Client/App/script/LuaMemory.cpp
  Client/App/script/Script.cpp
  Client/App/script/ScriptEvent.cpp
  Client/App/script/ThreadRef.cpp
  Client/App/script/ScriptContext.cpp
  Client/App/tool/DragTool.cpp
  Client/App/tool/DragUtilities.cpp
  Client/App/tool/Dragger.cpp
  Client/App/tool/GroupDragTool.cpp
  Client/App/tool/MegaDragger.cpp
  Client/App/tool/NullTool.cpp
  Client/App/tool/PartDragTool.cpp
  Client/App/tool/RunDragger.cpp
  Client/App/tool/ToolsArrow.cpp
  Client/App/tool/ToolsSurface.cpp
  Client/App/util/RunStateOwner.cpp
  Client/App/util/AudioSystem.cpp
  Client/App/util/Sound.cpp
  Client/App/util/SoundWorld.cpp
  Client/App/v8datamodel/Backpack.cpp
  Client/App/v8datamodel/Camera.cpp
  Client/App/v8datamodel/CharacterAppearance.cpp
  Client/App/v8datamodel/Commands.cpp
  Client/App/v8datamodel/DataModel.cpp
  Client/App/v8datamodel/DebrisService.cpp
  Client/App/v8datamodel/Explosion.cpp
  Client/App/v8datamodel/Filters.cpp
  Client/App/v8datamodel/ForceField.cpp
  Client/App/v8datamodel/GameSettings.cpp
  Client/App/v8datamodel/GeometryService.cpp
  Client/App/v8datamodel/GlobalSettings.cpp
  Client/App/v8datamodel/Gyro.cpp
  Client/App/v8datamodel/ICameraOwner.cpp
  Client/App/v8datamodel/JointsService.cpp
  Client/App/v8datamodel/Lighting.cpp
  Client/App/v8datamodel/LocakBackpack.cpp
  Client/App/v8datamodel/Mouse.cpp
  Client/App/v8datamodel/MouseCommand.cpp
  Client/App/v8datamodel/SafeChat.cpp
  Client/App/v8datamodel/ScriptMouseCommand.cpp
  Client/App/v8datamodel/Seat.cpp
  Client/App/v8datamodel/Selection.cpp
  Client/App/v8datamodel/Stats.cpp
  Client/App/v8datamodel/Surface.cpp
  Client/App/v8datamodel/Surfaces.cpp
  Client/App/v8datamodel/Sky.cpp
  Client/App/v8datamodel/Teams.cpp
  Client/App/v8datamodel/TimerService.cpp
  Client/App/v8datamodel/ToolMouseCommand.cpp
  Client/App/v8datamodel/ToolsPart.cpp
  Client/App/v8datamodel/UserController.cpp
  Client/App/v8datamodel/Value.cpp
  Client/App/v8datamodel/Visit.cpp
  Client/App/v8datamodel/DocumentVerbs.cpp
  Client/App/v8datamodel/factoryregistration.cpp
  Client/App/v8tree/Service.cpp
)
set(OPENRBX_NETWORK_SOURCES
  Client/Network/Player.cpp
  Client/Network/Players.cpp
  Client/Network/API.cpp
  Client/Network/Client.cpp
  Client/Network/ClientPhysics.cpp
  Client/Network/CrashReporter.cpp
  Client/Network/NetworkSettings.cpp
  Client/Network/Replicator.cpp
  Client/Network/Server.cpp
  Client/Network/Streaming.cpp
)
if (NOT WIN32)
  list(REMOVE_ITEM OPENRBX_NETWORK_SOURCES Client/Network/CrashReporter.cpp)
endif()

set(OPENRBX_RAKNET_SOURCES
  Client/Network/raknet30/Source/BitStream.cpp
  Client/Network/raknet30/Source/CheckSum.cpp
  Client/Network/raknet30/Source/DS_HuffmanEncodingTree.cpp
  Client/Network/raknet30/Source/DataBlockEncryptor.cpp
  Client/Network/raknet30/Source/FileOperations.cpp
  Client/Network/raknet30/Source/GetTime.cpp
  Client/Network/raknet30/Source/NetworkIDManager.cpp
  Client/Network/raknet30/Source/PacketLogger.cpp
  Client/Network/raknet30/Source/PluginInterface.cpp
  Client/Network/raknet30/Source/RPCMap.cpp
  Client/Network/raknet30/Source/RakNetTypes.cpp
  Client/Network/raknet30/Source/RakPeer.cpp
  Client/Network/raknet30/Source/RakSleep.cpp
  Client/Network/raknet30/Source/Rand.cpp
  Client/Network/raknet30/Source/ReliabilityLayer.cpp
  Client/Network/raknet30/Source/SHA1.cpp
  Client/Network/raknet30/Source/SimpleMutex.cpp
  Client/Network/raknet30/Source/SocketLayer.cpp
  Client/Network/raknet30/Source/StringCompressor.cpp
  Client/Network/raknet30/Source/StringTable.cpp
  Client/Network/raknet30/Source/rijndael.cpp
)
if (NOT WIN32)
  list(REMOVE_ITEM OPENRBX_RAKNET_SOURCES Client/Network/raknet30/Source/FileOperations.cpp)
  list(APPEND OPENRBX_RAKNET_SOURCES Client/Network/raknet30/Source/LinuxStrings.cpp)
endif()
set(OPENRBX_NETWORK_CLIENT_SOURCES
  Client/Network/Player.cpp
  Client/Network/Players.cpp
  Client/Network/CrashReporter.cpp
  Client/Network/API.cpp
  Client/Network/raknet30/Source/PluginInterface.cpp
  Client/Network/Client.cpp
  Client/Network/Server.cpp
  Client/Network/raknet30/Source/BitStream.cpp
  Client/Network/Streaming.cpp
  Client/Network/raknet30/Source/RakNetTypes.cpp
  Client/Network/raknet30/Source/FileOperations.cpp
  Client/Network/NetworkSettings.cpp
  Client/Network/Replicator.cpp
  Client/Network/raknet30/Source/PacketLogger.cpp
  Client/Network/raknet30/Source/StringCompressor.cpp
  Client/Network/raknet30/Source/GetTime.cpp
  Client/Network/ClientPhysics.cpp
  Client/Network/raknet30/Source/RakPeer.cpp
  Client/Network/raknet30/Source/DS_HuffmanEncodingTree.cpp
  Client/Network/raknet30/Source/SocketLayer.cpp
  Client/Network/raknet30/Source/ReliabilityLayer.cpp
  Client/Network/raknet30/Source/RPCMap.cpp
  Client/Network/raknet30/Source/Rand.cpp
  Client/Network/raknet30/Source/NetworkIDManager.cpp
  Client/Network/raknet30/Source/StringTable.cpp
  Client/Network/raknet30/Source/SHA1.cpp
  Client/Network/raknet30/Source/RakSleep.cpp
  Client/Network/raknet30/Source/DataBlockEncryptor.cpp
  Client/Network/raknet30/Source/rijndael.cpp
  Client/Network/raknet30/Source/CheckSum.cpp
  Client/Network/raknet30/Source/SimpleMutex.cpp
)
set(OPENRBX_RBXVIEW_SOURCES
  Client/RbxView/BevelMesh.cpp
  Client/RbxView/BrickMesh.cpp
  Client/RbxView/CylinderMesh.cpp
  Client/RbxView/FileMesh.cpp
  Client/RbxView/HeadMesh.cpp
  Client/RbxView/MaterialFactory.cpp
  Client/RbxView/PBBMesh.cpp
  Client/RbxView/QuadVolume.cpp
  Client/RbxView/SphereMesh.cpp
  Client/RbxView/TorsoMesh.cpp
  Client/RbxView/View.cpp
  Client/RbxView/WedgeMesh.cpp
  Client/RbxView/Part.cpp
)
set(OPENRBX_RBXVIEWNEW_SOURCES
  Client/Rendering/RenderLibNew/BevelMesh.cpp
  Client/Rendering/RenderLibNew/BrickMesh.cpp
  Client/Rendering/RenderLibNew/CylinderMesh.cpp
  Client/Rendering/RenderLibNew/FileMesh.cpp
  Client/Rendering/RenderLibNew/HeadMesh.cpp
  Client/Rendering/RenderLibNew/MaterialFactory.cpp
  Client/Rendering/RenderLibNew/Part.cpp
  Client/Rendering/RenderLibNew/PBBMesh.cpp
  Client/Rendering/RenderLibNew/QuadVolume.cpp
  Client/Rendering/RenderLibNew/SphereMesh.cpp
  Client/Rendering/RenderLibNew/TorsoMesh.cpp
  Client/Rendering/RenderLibNew/View.cpp
  Client/Rendering/RenderLibNew/WedgeMesh.cpp
)
set(OPENRBX_RENDERLIBNEW_SOURCES
  Client/Rendering/RenderLibNew/AggregatingSceneManager.cpp
  Client/Rendering/RenderLibNew/Chunk.cpp
  Client/Rendering/RenderLibNew/Clusterer.cpp
  Client/Rendering/RenderLibNew/EffectSettings.cpp
  Client/Rendering/RenderLibNew/Material.cpp
  Client/Rendering/RenderLibNew/Mesh.cpp
  Client/Rendering/RenderLibNew/RenderScene.cpp
  Client/Rendering/RenderLibNew/TextureProxy.cpp
)
set(OPENRBX_RENDERLIB_SOURCES
  Client/Rendering/RenderLib/AggregatingSceneManager.cpp
  Client/Rendering/RenderLib/Chunk.cpp
  Client/Rendering/RenderLib/Clusterer.cpp
  Client/Rendering/RenderLib/DepthBlur.cpp
  Client/Rendering/RenderLib/EffectSettings.cpp
  Client/Rendering/RenderLib/Material.cpp
  Client/Rendering/RenderLib/Mesh.cpp
  Client/Rendering/RenderLib/Profiler.cpp
  Client/Rendering/RenderLib/RenderScene.cpp
  Client/Rendering/RenderLib/RenderStats.cpp
  Client/Rendering/RenderLib/RenderSurface.cpp
  Client/Rendering/RenderLib/TextureProxy.cpp
)
set(OPENRBX_APPDRAW_RBXGFX_SOURCES
  Client/Rendering/AppDraw/AdornRbxGfx.cpp
)
set(OPENRBX_APPDRAW_CLIENT_SOURCES
  Client/Rendering/AppDraw/RbxEntity.cpp
  Client/Rendering/AppDraw/RbxFont.cpp
  Client/Rendering/AppDraw/RbxSceneManager.cpp
  Client/Rendering/AppDraw/RbxSceneNode.cpp
  Client/Rendering/AppDraw/RbxSky.cpp
  Client/Rendering/AppDraw/RbxSubEntity.cpp
  Client/Rendering/AppDraw/TwoDManager.cpp
  Client/Rendering/AppDraw/Quadric.cpp
  Client/Rendering/AppDraw/VisualEngine.cpp
)

set(OPENRBX_APPDRAW_SOURCES
  Client/Rendering/AppDraw/AdornG3D.cpp
  Client/Rendering/AppDraw/Draw.cpp
  Client/Rendering/AppDraw/DrawAdorn.cpp
  Client/Rendering/AppDraw/DrawPrimitives.cpp
  Client/Rendering/AppDraw/Fonts.cpp
  Client/Rendering/AppDraw/HitTest.cpp
  Client/Rendering/AppDraw/Textures.cpp
)

set(OPENRBX_ENGINE_INCLUDE_DIRS
  "${PROJECT_SOURCE_DIR}/Client/App/include"
  "${PROJECT_SOURCE_DIR}/Client/Network/include"
  "${PROJECT_SOURCE_DIR}/Client/RbxView/include"
  "${PROJECT_SOURCE_DIR}/Client/RbxViewBase"
  "${PROJECT_SOURCE_DIR}/Client/Rendering/RenderLib/include"
  "${PROJECT_SOURCE_DIR}/Client/Rendering/AppDraw/include"
  "${PROJECT_SOURCE_DIR}/Client/RbxGraphics/include"
  "${PROJECT_SOURCE_DIR}/Client/Network/raknet30/Source"
)

set(OPENRBX_3RDPARTY_INCLUDE_DIRS
  "${OPENRBX_G3D_INCLUDE_DIR}"
  "${OPENRBX_LUA_INCLUDE_DIR}"
  ${OPENRBX_G3D_EXTRA_INCLUDE_DIRS}
)
if (NOT WIN32)
  list(APPEND OPENRBX_3RDPARTY_INCLUDE_DIRS "${OPENRBX_3RDPARTY}/glu/include")
endif()

if (WIN32)
  set(OPENRBX_COMMON_DEFINITIONS
    WIN32
    NDEBUG
    _LIB
    _VC80_UPGRADE=0x0710
    _MBCS
  )
elseif (ANDROID)
  set(OPENRBX_COMMON_DEFINITIONS
    NDEBUG
    _LIB
    G3D_DEBUG_NOGUI
    G3D_EGL
    G3D_GLES_COMPAT
  )
else()
  set(OPENRBX_COMMON_DEFINITIONS
    NDEBUG
    _LIB
    G3D_DEBUG_NOGUI
  )
endif()

set(OPENRBX_ENGINE_LIBRARIES)

function(add_engine_library NAME)
  cmake_parse_arguments(ARGS "" "" "DEFINITIONS;SOURCES" ${ARGN})
  if (NOT ARGS_SOURCES)
    message(STATUS "Engine library '${NAME}' has no sources yet, skipping")
    return()
  endif()
  add_library(${NAME} OBJECT ${ARGS_SOURCES})
  openrbx_compile_defaults(${NAME})
  set_target_properties(${NAME} PROPERTIES CXX_STANDARD 23 CXX_SCAN_FOR_MODULES OFF)
  target_compile_definitions(${NAME} PRIVATE ${OPENRBX_COMMON_DEFINITIONS} ${ARGS_DEFINITIONS})
  target_include_directories(${NAME} PRIVATE ${OPENRBX_ENGINE_INCLUDE_DIRS} ${OPENRBX_3RDPARTY_INCLUDE_DIRS})
  if (MSVC)
    target_compile_options(${NAME} PRIVATE /EHsc)
  endif()
  set(OPENRBX_ENGINE_LIBRARIES ${OPENRBX_ENGINE_LIBRARIES} ${NAME} PARENT_SCOPE)
endfunction()

set(OPENRBX_LUA_SOURCES
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lapi.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lauxlib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lbaselib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lcode.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ldebug.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ldo.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ldump.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lfunc.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lgc.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/liolib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/llex.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lmathlib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lmem.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lobject.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lopcodes.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lparser.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lstate.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lstring.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lstrlib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ltable.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ltablib.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/ltm.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lundump.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lvm.c"
  "${PROJECT_SOURCE_DIR}/Client/App/lua-5.1.1/src/lzio.c"
)

set_source_files_properties(${OPENRBX_LUA_SOURCES} PROPERTIES LANGUAGE CXX)

add_engine_library(lua
  DEFINITIONS _CRT_SECURE_NO_WARNINGS _RELEASE
  SOURCES ${OPENRBX_LUA_SOURCES}
)

add_engine_library(app
  DEFINITIONS _CRT_SECURE_NO_WARNINGS _RELEASE
  SOURCES ${OPENRBX_APP_SOURCES}
)

if (TARGET app)
  target_include_directories(app PRIVATE "${OPENRBX_3RDPARTY}/dr_libs")
  target_link_libraries(app PRIVATE OpenAL::OpenAL openmpt stb_vorbis CURL::libcurl OpenSSL::Crypto)
endif()

add_engine_library(network
  SOURCES ${OPENRBX_NETWORK_SOURCES} ${OPENRBX_RAKNET_SOURCES}
)

if (TARGET network AND OPENRBX_RAKNET_SOURCES)
  set_source_files_properties(${OPENRBX_RAKNET_SOURCES}
    PROPERTIES COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
endif()

if (WIN32)
  add_engine_library(network_client
    SOURCES ${OPENRBX_NETWORK_CLIENT_SOURCES}
  )
  list(REMOVE_ITEM OPENRBX_ENGINE_LIBRARIES network_client)
endif()

add_engine_library(rbxview
  DEFINITIONS _RELEASE
  SOURCES ${OPENRBX_RBXVIEW_SOURCES}
)

add_engine_library(appdraw
  DEFINITIONS _RELEASE
  SOURCES ${OPENRBX_APPDRAW_SOURCES}
)

add_engine_library(renderlib
  SOURCES ${OPENRBX_RENDERLIB_SOURCES}
)

if (ANDROID)
  add_library(openrbx_android SHARED
    Client/android/AndroidEngine.cpp
    Client/android/AndroidUserInput.cpp
    Client/android/AndroidWindow.cpp
    Client/android/gl_es_compat.cpp
    Client/android/jni_bridge.cpp
    Client/qt/FactoryRegistration.cpp
    Client/win/ReflectionMetadata.cpp
  )
  openrbx_compile_defaults(openrbx_android)
  set_target_properties(openrbx_android PROPERTIES CXX_STANDARD 23 CXX_SCAN_FOR_MODULES OFF)
  target_compile_definitions(openrbx_android PRIVATE ${OPENRBX_COMMON_DEFINITIONS} _RELEASE)
  target_include_directories(openrbx_android PRIVATE
    "${PROJECT_SOURCE_DIR}/Client/android"
    "${PROJECT_SOURCE_DIR}/Client/win"
    "${PROJECT_SOURCE_DIR}/Client/qt"
    ${OPENRBX_ENGINE_INCLUDE_DIRS}
    ${OPENRBX_3RDPARTY_INCLUDE_DIRS}
  )
  string(REPLACE ";" "," OPENRBX_ANDROID_LINK_GROUP "${OPENRBX_ENGINE_LIBRARIES}")
  target_link_libraries(openrbx_android PRIVATE
    "$<LINK_GROUP:RESCAN,${OPENRBX_ANDROID_LINK_GROUP}>"
    glg3d_egl
    graphics3d
    glu
    OpenAL::OpenAL
    openmpt
    stb_vorbis
    CURL::libcurl
    OpenSSL::Crypto
    android
    log
    EGL
    GLESv2
    OpenSLES
    Threads::Threads
    ${CMAKE_DL_LIBS}
    m
  )
  return()
endif()

add_executable(webservice
  WebService/ThumbnailGenerator.cpp
  WebService/WebService.cpp
)
openrbx_compile_defaults(webservice)
set_target_properties(webservice PROPERTIES CXX_STANDARD 23 CXX_SCAN_FOR_MODULES OFF OUTPUT_NAME "WebService")
target_compile_definitions(webservice PRIVATE ${OPENRBX_COMMON_DEFINITIONS})

target_include_directories(webservice PRIVATE
  "${PROJECT_SOURCE_DIR}/WebService"
  ${OPENRBX_ENGINE_INCLUDE_DIRS}
  ${OPENRBX_3RDPARTY_INCLUDE_DIRS}
  "${OPENRBX_3RDPARTY}/cpp-httplib"
  "${OPENRBX_3RDPARTY}/nlohmann-json/include"
)

target_link_libraries(webservice PRIVATE
  ${OPENRBX_ENGINE_LIBRARIES}
  glg3d_egl
  graphics3d
  glu
  OpenAL::OpenAL
  openmpt
  stb_vorbis
  CURL::libcurl
)
add_dependencies(webservice runtime_files)

if (WIN32)
  target_sources(webservice PRIVATE WebService/WebService.rc)
  target_link_libraries(webservice PRIVATE
    advapi32
    dbghelp
    gdi32
    ole32
    setupapi
    shell32
    shlwapi
    user32
    version
    ws2_32
  )
else()
  target_link_libraries(webservice PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
  set_target_properties(webservice PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN")
endif()

if (MSVC)
  target_link_options(webservice PRIVATE "-MAP:$<TARGET_FILE_DIR:webservice>/$<TARGET_FILE_BASE_NAME:webservice>.map" "/OPT:REF,ICF")
  foreach(lib zlib.lib G3D.lib G3D-debug.lib GLG3D.lib GLG3D-debug.lib glut32.lib)
    target_link_options(webservice PRIVATE "/NODEFAULTLIB:${lib}")
  endforeach()
  set_property(TARGET webservice PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

find_program(CLANGFORMAT_BIN NAMES clang-format)
if (EXISTS "${CLANGFORMAT_BIN}")
  execute_process(COMMAND "${CLANGFORMAT_BIN}" --version
    OUTPUT_VARIABLE "CLANGFORMAT_VERSION_OUTPUT"
    RESULT_VARIABLE "CLANGFORMAT_RESULT"
  )
  if (CLANGFORMAT_RESULT EQUAL 0 AND CLANGFORMAT_VERSION_OUTPUT MATCHES "version ([0-9\\.]+)")
    set(CLANGFORMAT_VERSION "${CMAKE_MATCH_1}")
    set(CLANGFORMAT_VERSION_REQUIRED "17.0")
    message(DEBUG "Found clang-format version ${CLANGFORMAT_VERSION} (needs ${CLANGFORMAT_VERSION_REQUIRED}")
    if (CLANGFORMAT_VERSION VERSION_GREATER_EQUAL "${CLANGFORMAT_VERSION_REQUIRED}")
      file(GLOB_RECURSE openrbx_sources CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/WebService/*.cpp"
        "${PROJECT_SOURCE_DIR}/WebService/*.h"
        "${PROJECT_SOURCE_DIR}/Client/*.cpp"
        "${PROJECT_SOURCE_DIR}/Client/*.h"
      )
      string(REPLACE ";" "\n" openrbx_sources_lines "${openrbx_sources}")
      file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/openrbx_sources.txt" "${openrbx_sources_lines}\n")
      add_custom_target(clang-format ${CLANGFORMAT_BIN} -i "--files=${CMAKE_CURRENT_BINARY_DIR}/openrbx_sources.txt")
    endif()
  endif()
endif()

add_engine_library(appdraw_rbxgfx
  DEFINITIONS _RELEASE
  SOURCES ${OPENRBX_APPDRAW_RBXGFX_SOURCES}
)

add_engine_library(appdraw_client
  DEFINITIONS _RELEASE
  SOURCES ${OPENRBX_APPDRAW_CLIENT_SOURCES}
)

set(OPENRBX_RENDERLIBNEW_INCLUDE_DIRS
  "${PROJECT_SOURCE_DIR}/Client/Rendering/RenderLibNew"
  "${PROJECT_SOURCE_DIR}/Client/Rendering/RenderLibNew/include"
)

add_engine_library(rbxviewnew
  DEFINITIONS _RELEASE
  SOURCES ${OPENRBX_RBXVIEWNEW_SOURCES}
)

add_engine_library(renderlibnew
  SOURCES ${OPENRBX_RENDERLIBNEW_SOURCES}
)

list(REMOVE_ITEM OPENRBX_ENGINE_LIBRARIES rbxviewnew renderlibnew appdraw_rbxgfx appdraw_client)
foreach(lib rbxviewnew renderlibnew appdraw_rbxgfx appdraw_client)
  if (TARGET ${lib})
    target_include_directories(${lib} PRIVATE ${OPENRBX_RENDERLIBNEW_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Client/Rendering/OgreSDK/include")
  endif()
endforeach()
if (WIN32)
  foreach(lib appdraw_rbxgfx appdraw_client)
    target_include_directories(${lib} PRIVATE "${OPENRBX_3RDPARTY}/Microsoft DirectX SDK (November 2007)/Include")
  endforeach()
endif()

set(OPENRBX_OGRE "${PROJECT_SOURCE_DIR}/Client/Rendering/OgreSDK")

set(OPENRBX_CLIENT_ENGINE_LIBRARIES)
foreach(lib rbxview renderlib rbxviewnew renderlibnew network_client app appdraw_rbxgfx appdraw appdraw_client lua)
  if (TARGET ${lib})
    list(APPEND OPENRBX_CLIENT_ENGINE_LIBRARIES ${lib})
  endif()
endforeach()


file(GLOB OPENRBX_ROBLOX_QT_RESOURCES
  "${PROJECT_SOURCE_DIR}/Client/win/res/*.bmp"
  "${PROJECT_SOURCE_DIR}/Client/win/res/*.cur"
  "${PROJECT_SOURCE_DIR}/Client/win/res/*.htm"
  "${PROJECT_SOURCE_DIR}/Client/win/res/*.ico"
  "${PROJECT_SOURCE_DIR}/Client/win/res/*.png"
)

add_executable(roblox WIN32
  Client/qt/AboutRobloxDialog.cpp
  Client/qt/ActiveDocView.cpp
  Client/qt/BrowserView.cpp
  Client/qt/ChatPrompt.cpp
  Client/qt/ChildFrm.cpp
  Client/qt/ClipboardVerbs.cpp
  Client/qt/ColorSelector.cpp
  Client/qt/Command.cpp
  Client/qt/CommandLineComboBox.cpp
  Client/qt/CustomizeSheet.cpp
  Client/qt/Dialog.cpp
  Client/qt/DisplayMode.cpp
  Client/qt/DocView.cpp
  Client/qt/ExplorerPane.cpp
  Client/qt/FileDialog.cpp
  Client/qt/External.cpp
  Client/qt/FactoryRegistration.cpp
  Client/qt/HtmlDialog.cpp
  Client/qt/HtmlView.cpp
  Client/qt/HtmlWindow.cpp
  Client/qt/BrowserScrollBar.cpp
  Client/qt/ImageManager.cpp
  Client/qt/InputDevice.cpp
  Client/qt/InsertObjectDialog.cpp
  Client/qt/InstanceExplorer.cpp
  Client/qt/LayoutImport.cpp
  Client/qt/MainFrm.cpp
  Client/qt/MultiPlayerPane.cpp
  Client/qt/NativeInput.cpp
  Client/qt/NullDoc.cpp
  Client/qt/PaneCaption.cpp
  Client/qt/PopupToolBar.cpp
  Client/qt/ObjectBrowser.cpp
  Client/qt/Printing.cpp
  Client/qt/Profile.cpp
  Client/qt/PropertySheet.cpp
  Client/qt/PropGrid.cpp
  Client/qt/Resources.cpp
  Client/qt/Roblox.cpp
  Client/qt/RobloxDoc.cpp
  Client/qt/RobloxReportView.cpp
  Client/qt/RobloxTreeCtrl.cpp
  Client/qt/RobloxView.cpp
  Client/qt/RobloxWnd.cpp
  Client/qt/ScintillaCtrl.cpp
  Client/qt/ScintillaDocView.cpp
  Client/qt/ScriptDoc.cpp
  Client/qt/SelectionCaption.cpp
  Client/qt/SelectionTreeCtrl.cpp
  Client/qt/NativeWindow.cpp
  Client/qt/ReportControl.cpp
  Client/qt/SettingsDialog.cpp
  Client/qt/SplitterPane.cpp
  Client/qt/SystemColorDialog.cpp
  Client/qt/SystemMessageBox.cpp
  Client/qt/TearOffGripper.cpp
  Client/qt/ImageEditorDialog.cpp
  Client/qt/MenuEditItem.cpp
  Client/qt/MenuIconGrid.cpp
  Client/qt/MenuExpandButton.cpp
  Client/qt/TreeWidget.cpp
  Client/qt/UserInput.cpp
  Client/qt/StandardOutputView.cpp
  Client/qt/StatusBar.cpp
  Client/qt/VerbBinder.cpp
  Client/qt/WebToolbox.cpp
  Client/qt/WhidbeyStyle.cpp
  Client/qt/WorkspaceTabButtons.cpp
  Client/qt/Workspace.cpp
  Client/win/ErrorUploader.cpp
  Client/win/LDraw2RobloxColorMap.cpp
  Client/win/LDraw2RobloxMapRoot.cpp
  Client/win/LDraw2RobloxPartMap.cpp
  Client/win/LDrawCommand.cpp
  Client/win/LDrawParser.cpp
  Client/win/LogManager.cpp
  Client/win/LuaWriter.cpp
  Client/win/ReflectionMetadata.cpp
  Client/win/RenderSettings.cpp
  Client/win/AuthenticationNegotiator.cpp
  Client/win/UserInputUtil.cpp
  Client/win/VersionInfo.cpp
)
if (WIN32)
  target_sources(roblox PRIVATE Client/qt/PreviewView.cpp Client/qt/Roblox.rc)
endif()
qt_add_resources(roblox "roblox_resources" PREFIX "/" BASE "${PROJECT_SOURCE_DIR}/Client/win" FILES ${OPENRBX_ROBLOX_QT_RESOURCES})
openrbx_compile_defaults(roblox)
qt_disable_unicode_defines(roblox)
set_target_properties(roblox PROPERTIES CXX_STANDARD 23 CXX_SCAN_FOR_MODULES OFF OUTPUT_NAME "Roblox")
target_compile_definitions(roblox PRIVATE ${OPENRBX_COMMON_DEFINITIONS} QT_NO_KEYWORDS)
if (WIN32)
  target_compile_options(roblox PRIVATE /permissive)
  target_compile_definitions(roblox PRIVATE "_WINSOCKAPI_=")
else()
  set_target_properties(roblox PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN;$ORIGIN/lib")
endif()
target_include_directories(roblox PRIVATE
  "${PROJECT_SOURCE_DIR}/Client/qt"
  "${PROJECT_SOURCE_DIR}/Client/win"
  ${OPENRBX_ENGINE_INCLUDE_DIRS}
  ${OPENRBX_RENDERLIBNEW_INCLUDE_DIRS}
  ${OPENRBX_3RDPARTY_INCLUDE_DIRS}
  "${OPENRBX_OGRE}/include"
  "${OPENRBX_3RDPARTY}/webview/core/include"
)
if (WIN32)
  target_include_directories(roblox PRIVATE
    "${OPENRBX_3RDPARTY}/Microsoft DirectX SDK (November 2007)/Include"
    "${OPENRBX_3RDPARTY}/webview2/include"
  )
endif()
add_dependencies(roblox runtime_files)
if (NOT WIN32)
  find_package(PkgConfig REQUIRED)
  string(REPLACE ";" "," OPENRBX_CLIENT_LINK_GROUP "${OPENRBX_CLIENT_ENGINE_LIBRARIES}")
  pkg_check_modules(OPENRBX_WEBKIT REQUIRED IMPORTED_TARGET gtk+-3.0 webkit2gtk-4.1)
  find_package(Qt6 REQUIRED COMPONENTS XcbQpaPrivate PrintSupport)
  target_sources(roblox PRIVATE Client/qt/GlxWindow.cpp)
  add_dependencies(roblox rggl rgpar)
  target_link_libraries(roblox PRIVATE
    PkgConfig::OPENRBX_WEBKIT
    Qt6::Widgets
    Qt6::PrintSupport
    scintilla
    SDL3::SDL3-static
    rgmain
    "$<LINK_GROUP:RESCAN,${OPENRBX_CLIENT_LINK_GROUP},network>"
    glg3d
    graphics3d
    glu
    OpenAL::OpenAL
    openmpt
    stb_vorbis
    CURL::libcurl
    OpenGL::GL
    X11::X11
    X11::Xrandr
    Threads::Threads
    ${CMAKE_DL_LIBS}
  )
  add_custom_command(TARGET roblox POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/lib"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_SONAME_FILE:Qt6::Core>"
            "$<TARGET_SONAME_FILE:Qt6::Gui>" "$<TARGET_SONAME_FILE:Qt6::Widgets>"
            "$<TARGET_SONAME_FILE:Qt6::Core5Compat>" "$<TARGET_SONAME_FILE:Qt6::XcbQpaPrivate>"
            "$<TARGET_SONAME_FILE:Qt6::PrintSupport>" "$<TARGET_FILE_DIR:roblox>/lib"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/plugins/platforms"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QXcbIntegrationPlugin>"
            "$<TARGET_FILE_DIR:roblox>/plugins/platforms"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/plugins/printsupport"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QCupsPrinterSupportPlugin>"
            "$<TARGET_FILE_DIR:roblox>/plugins/printsupport"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/plugins/imageformats"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QICOPlugin>"
            "$<TARGET_FILE_DIR:roblox>/plugins/imageformats"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/plugins/platforminputcontexts"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QComposePlatformInputContextPlugin>"
            "$<TARGET_FILE_DIR:roblox>/plugins/platforminputcontexts"
  )
  return()
endif()
target_link_libraries(roblox PRIVATE
  Qt6::Widgets
  scintilla
  SDL3::SDL3-static
  rgmain
  ${OPENRBX_CLIENT_ENGINE_LIBRARIES}
  graphics3d
  glg3d
  opengl32
  glu32
  OpenAL::OpenAL
  openmpt
  stb_vorbis
  CURL::libcurl
  advapi32
  dbghelp
  gdi32
  iphlpapi
  ole32
  oleaut32
  setupapi
  shell32
  shlwapi
  user32
  version
  winmm
  ws2_32
)
target_link_options(roblox PRIVATE "-MAP:$<TARGET_FILE_DIR:roblox>/$<TARGET_FILE_BASE_NAME:roblox>.map" "/FIXED:NO" "/MANIFEST:NO" "/OPT:REF,ICF" "/HIGHENTROPYVA:NO" "/SUBSYSTEM:WINDOWS,5.02")
foreach(lib zlib.lib G3D.lib G3D-debug.lib GLG3D.lib GLG3D-debug.lib glut32.lib)
  target_link_options(roblox PRIVATE "/NODEFAULTLIB:${lib}")
endforeach()
set_property(TARGET roblox PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
add_custom_command(TARGET roblox POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::Core>" "$<TARGET_FILE:Qt6::Gui>"
          "$<TARGET_FILE:Qt6::Widgets>" "$<TARGET_FILE:Qt6::Core5Compat>" "$<TARGET_FILE_DIR:roblox>"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/platforms"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>"
          "$<TARGET_FILE_DIR:roblox>/platforms"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/imageformats"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QICOPlugin>"
          "$<TARGET_FILE_DIR:roblox>/imageformats"
  COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:roblox>/styles"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:Qt6::QModernWindowsStylePlugin>"
          "$<TARGET_FILE_DIR:roblox>/styles"
)
