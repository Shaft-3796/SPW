cmake_minimum_required(VERSION 3.25)
project(SPW)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(PlatformerEngine)
include_directories(RenderingEngine)
include_directories(SDL2)
include_directories(SDL2/include)
include_directories(SDL2_image)
include_directories(SDL2_image/include)
include_directories(SDL2_mixer)
include_directories(SDL2_mixer/include)
include_directories(SDL2_ttf)
include_directories(SDL2_ttf/include)
include_directories(SPW)

add_executable(SPW
        PlatformerEngine/PE.h
        PlatformerEngine/PE_Allocator.h
        PlatformerEngine/PE_Body.h
        PlatformerEngine/PE_BroadPhase.h
        PlatformerEngine/PE_Callback.h
        PlatformerEngine/PE_CapsuleShape.h
        PlatformerEngine/PE_CircleShape.h
        PlatformerEngine/PE_Collider.h
        PlatformerEngine/PE_ColliderProxy.h
        PlatformerEngine/PE_CollisionContainer.h
        PlatformerEngine/PE_CollisionManager.h
        PlatformerEngine/PE_CollisionPair.h
        PlatformerEngine/PE_DynamicTree.h
        PlatformerEngine/PE_IdStack.h
        PlatformerEngine/PE_List.h
        PlatformerEngine/PE_Math.h
        PlatformerEngine/PE_PolygonShape.h
        PlatformerEngine/PE_Settings.h
        PlatformerEngine/PE_Shape.h
        PlatformerEngine/PE_ShapeCollision.h
        PlatformerEngine/PE_ShapeDistance.h
        PlatformerEngine/PE_Solver.h
        PlatformerEngine/PE_TimeOfImpact.h
        PlatformerEngine/PE_UserData.h
        PlatformerEngine/PE_Vector.h
        PlatformerEngine/PE_World.h
        RenderingEngine/cJSON.h
        RenderingEngine/RE.h
        RenderingEngine/RE_AlphaAnim.h
        RenderingEngine/RE_AngleAnim.h
        RenderingEngine/RE_Animation.h
        RenderingEngine/RE_Animator.h
        RenderingEngine/RE_Atlas.h
        RenderingEngine/RE_BezierAnim.h
        RenderingEngine/RE_Renderer.h
        RenderingEngine/RE_ScaleAnim.h
        RenderingEngine/RE_Settings.h
        RenderingEngine/RE_ShiftAnim.h
        RenderingEngine/RE_TexAnim.h
        RenderingEngine/RE_Text.h
        RenderingEngine/RE_Timer.h
        RenderingEngine/RE_Utils.h
        SDL2/include/begin_code.h
        SDL2/include/close_code.h
        SDL2/include/SDL.h
        SDL2/include/SDL_assert.h
        SDL2/include/SDL_atomic.h
        SDL2/include/SDL_audio.h
        SDL2/include/SDL_bits.h
        SDL2/include/SDL_blendmode.h
        SDL2/include/SDL_clipboard.h
        SDL2/include/SDL_config.h
        SDL2/include/SDL_config_android.h
        SDL2/include/SDL_config_iphoneos.h
        SDL2/include/SDL_config_macosx.h
        SDL2/include/SDL_config_minimal.h
        SDL2/include/SDL_config_pandora.h
        SDL2/include/SDL_config_psp.h
        SDL2/include/SDL_config_windows.h
        SDL2/include/SDL_config_winrt.h
        SDL2/include/SDL_config_wiz.h
        SDL2/include/SDL_copying.h
        SDL2/include/SDL_cpuinfo.h
        SDL2/include/SDL_egl.h
        SDL2/include/SDL_endian.h
        SDL2/include/SDL_error.h
        SDL2/include/SDL_events.h
        SDL2/include/SDL_filesystem.h
        SDL2/include/SDL_gamecontroller.h
        SDL2/include/SDL_gesture.h
        SDL2/include/SDL_haptic.h
        SDL2/include/SDL_hidapi.h
        SDL2/include/SDL_hints.h
        SDL2/include/SDL_joystick.h
        SDL2/include/SDL_keyboard.h
        SDL2/include/SDL_keycode.h
        SDL2/include/SDL_loadso.h
        SDL2/include/SDL_locale.h
        SDL2/include/SDL_log.h
        SDL2/include/SDL_main.h
        SDL2/include/SDL_messagebox.h
        SDL2/include/SDL_metal.h
        SDL2/include/SDL_misc.h
        SDL2/include/SDL_mouse.h
        SDL2/include/SDL_mutex.h
        SDL2/include/SDL_name.h
        SDL2/include/SDL_opengl.h
        SDL2/include/SDL_opengl_glext.h
        SDL2/include/SDL_opengles.h
        SDL2/include/SDL_opengles2.h
        SDL2/include/SDL_opengles2_gl2.h
        SDL2/include/SDL_opengles2_gl2ext.h
        SDL2/include/SDL_opengles2_gl2platform.h
        SDL2/include/SDL_opengles2_khrplatform.h
        SDL2/include/SDL_pixels.h
        SDL2/include/SDL_platform.h
        SDL2/include/SDL_power.h
        SDL2/include/SDL_quit.h
        SDL2/include/SDL_rect.h
        SDL2/include/SDL_render.h
        SDL2/include/SDL_revision.h
        SDL2/include/SDL_rwops.h
        SDL2/include/SDL_scancode.h
        SDL2/include/SDL_sensor.h
        SDL2/include/SDL_shape.h
        SDL2/include/SDL_stdinc.h
        SDL2/include/SDL_surface.h
        SDL2/include/SDL_system.h
        SDL2/include/SDL_syswm.h
        SDL2/include/SDL_test.h
        SDL2/include/SDL_test_assert.h
        SDL2/include/SDL_test_common.h
        SDL2/include/SDL_test_compare.h
        SDL2/include/SDL_test_crc32.h
        SDL2/include/SDL_test_font.h
        SDL2/include/SDL_test_fuzzer.h
        SDL2/include/SDL_test_harness.h
        SDL2/include/SDL_test_images.h
        SDL2/include/SDL_test_log.h
        SDL2/include/SDL_test_md5.h
        SDL2/include/SDL_test_memory.h
        SDL2/include/SDL_test_random.h
        SDL2/include/SDL_thread.h
        SDL2/include/SDL_timer.h
        SDL2/include/SDL_touch.h
        SDL2/include/SDL_types.h
        SDL2/include/SDL_version.h
        SDL2/include/SDL_video.h
        SDL2/include/SDL_vulkan.h
        SDL2_image/include/SDL_image.h
        SDL2_mixer/include/SDL_mixer.h
        SDL2_ttf/include/SDL_ttf.h
        SPW/ApplicationInput.cpp
        SPW/ApplicationInput.h
        SPW/AssetManager.cpp
        SPW/AssetManager.h
        SPW/Background.cpp
        SPW/Background.h
        SPW/Bonus.cpp
        SPW/Bonus.h
        SPW/Brick.cpp
        SPW/Brick.h
        SPW/Button.cpp
        SPW/Button.h
        SPW/Camera.cpp
        SPW/Camera.h
        SPW/Checkpoint.cpp
        SPW/Checkpoint.h
        SPW/Collectable.cpp
        SPW/Collectable.h
        SPW/ControlsInput.cpp
        SPW/ControlsInput.h
        SPW/DebugCamera.cpp
        SPW/DebugCamera.h
        SPW/DebugInput.cpp
        SPW/DebugInput.h
        SPW/Enemy.cpp
        SPW/Enemy.h
        SPW/GameBody.cpp
        SPW/GameBody.h
        SPW/GameObject.cpp
        SPW/GameObject.h
        SPW/Graphics.cpp
        SPW/Graphics.h
        SPW/Heart.cpp
        SPW/Heart.h
        SPW/Image.cpp
        SPW/Image.h
        SPW/InputGroup.cpp
        SPW/InputGroup.h
        SPW/InputManager.cpp
        SPW/InputManager.h
        SPW/LevelCanvas.cpp
        SPW/LevelCanvas.h
        SPW/LevelData.cpp
        SPW/LevelData.h
        SPW/LevelEnd.cpp
        SPW/LevelEnd.h
        SPW/LevelHeader.cpp
        SPW/LevelHeader.h
        SPW/LevelParser.cpp
        SPW/LevelParser.h
        SPW/LevelScene.cpp
        SPW/LevelScene.h
        SPW/LevelSelection.cpp
        SPW/LevelSelection.h
        SPW/Main.cpp
        SPW/MainCamera.cpp
        SPW/MainCamera.h
        SPW/MouseInput.cpp
        SPW/MouseInput.h
        SPW/Nut.cpp
        SPW/Nut.h
        SPW/ObjectManager.cpp
        SPW/ObjectManager.h
        SPW/PauseMenu.cpp
        SPW/PauseMenu.h
        SPW/Player.cpp
        SPW/Player.h
        SPW/Scene.cpp
        SPW/Scene.h
        SPW/Settings.h
        SPW/StartScreen.cpp
        SPW/StartScreen.h
        SPW/StaticMap.cpp
        SPW/StaticMap.h
        SPW/Text.cpp
        SPW/Text.h
        SPW/TitleCamera.cpp
        SPW/TitleCamera.h
        SPW/TitleCanvas.cpp
        SPW/TitleCanvas.h
        SPW/TitleScene.cpp
        SPW/TitleScene.h
        SPW/UIObject.cpp
        SPW/UIObject.h
        SPW/Utils.cpp
        SPW/Utils.h)
