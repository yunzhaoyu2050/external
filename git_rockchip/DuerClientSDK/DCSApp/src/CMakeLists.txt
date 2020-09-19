if(BUILD_TTS_SDK)
    add_definitions(-DLOCALTTS)
if(NOT ${Platform} MATCHES Sengled)
    set(LocalSynthesizerPlayer "MediaPlayer/Impl/LocalTtsPlayerImpl.cpp")
    set(LocalTtsProxy "MediaPlayer/Proxy/LocalTtsProxy.cpp")
endif()
    set(TTS_SDK_LIB "ASRTTSManager")
endif()

if(BUILD_CRAB_SDK)
   set(CRAB_SDK_LIB CrabSdk)
   include_directories("${BREAKPAD_INCLUDE_DIR}")
   include_directories("${BREAKPAD_INCLUDE_DIR}/breakpad")
endif()

if(${Platform} STREQUAL Sengled
        OR ${Platform} STREQUAL Ingenic)
    set(vendor_AUDIO_INPUT_SOURCE_FILES "Vendor/${Platform}/src/AudioInputWrapper.cpp")
    set(DCSApplication_SOURCE_FILES "Vendor/${Platform}/src/DCSApplication.cpp")
    include_directories("Vendor/${Platform}/include")
    link_directories("Vendor/${Platform}/lib")
else()
    set(DCSApplication_SOURCE_FILES "DCSApplication.cpp")
endif()

if(${Platform} MATCHES "Box86")
    set(DuerLinkWrapper_SOURCE_FILES "${Platform}/DuerLinkWrapper.cpp")
else()
    set(DuerLinkWrapper_SOURCE_FILES "DuerLinkWrapper.cpp")
endif()

if(KITTAI_KEY_WORD_DETECTOR)
    set(KEYWORD_OBSERVER_CODE VoiceAndTouchWakeUpObserver.cpp)
endif()

#if(BDSAI_KEY_WORD_DETECTOR)
    set(KEYWORD_OBSERVER_CODE VoiceAndTouchWakeUpObserver.cpp)
#endif()

if(BUILD_DLNA)
    add_definitions(-DDUEROS_DLNA)
    set(DLNA_SOURCE_FILES 
            DlnaDmrOutputFfmpeg.cpp
            Dlna/DlnaDmrOutput.cpp
            Dlna/DlnaDmrSdk.cpp 
            Dlna/DlnaDmrSongMetaData.cpp 
            Dlna/DlnaDmrUpnp.cpp 
            Dlna/DlnaDmrUpnpConnMgr.cpp 
            Dlna/DlnaDmrUpnpControl.cpp 
            Dlna/DlnaDmrUpnpDevice.cpp 
            Dlna/DlnaDmrUpnpRender.cpp 
            Dlna/DlnaDmrUpnpTransport.cpp 
            Dlna/DlnaDmrVariableContainer.cpp 
            Dlna/DlnaDmrWebServer.cpp 
            Dlna/DlnaDmrXmlDoc.cpp 
            Dlna/DlnaDmrXmlEscape.cpp
            MediaPlayer/Impl/DlnaPlayerImpl.cpp
        )
    set(DLNA_SDK_LIB threadutil ixml upnp)
    set(DLNA_INCLUDE_DIR "${DCSApp_SOURCE_DIR}/include/Dlna")
endif()

add_subdirectory(Audio)
include_directories("${ASRTTSManager_SOURCE_DIR}/include")
add_executable(duer_linux
        main.cpp
        ApplicationManager.cpp
        ${DCSApplication_SOURCE_FILES}
        SoundController.cpp
        DeviceIoWrapper.cpp
        Configuration.cpp
        DuerLinkMtkInstance.cpp
        ${DuerLinkWrapper_SOURCE_FILES}
        SystemUpdateRevWrapper.cpp
        ThreadPoolExecutor.cpp
        ${KEYWORD_OBSERVER_CODE}
        ActivityMonitorSingleton.cpp
        RecordAudioManager.cpp
        MediaPlayer/Impl/AutoLock.cpp
        MediaPlayer/Impl/PthreadLock.cpp
        MediaPlayer/Impl/AlsaController.cpp
        MediaPlayer/Impl/StreamPool.cpp
        MediaPlayer/Impl/PcmFilePlayerImpl.cpp
        MediaPlayer/Impl/PcmUrlPlayer.cpp
        MediaPlayer/Impl/PcmTtsPlayerImpl.cpp
        MediaPlayer/Impl/Mp3TtsPlayerImpl.cpp
        MediaPlayer/Impl/PcmResampler.cpp
        MediaPlayer/Impl/BluetoothPlayerImpl.cpp
        MediaPlayer/Impl/AudioDecoder.cpp
        MediaPlayer/Impl/Mp3FilePlayerImpl.cpp
        MediaPlayer/Impl/PcmBufPool.cpp
        MediaPlayer/Impl/Mp3UrlPlayerImpl.cpp
        MediaPlayer/Proxy/TtsPlayerProxy.cpp
        MediaPlayer/Proxy/AlertsPlayerProxy.cpp
        MediaPlayer/Proxy/LocalPlayerProxy.cpp
        MediaPlayer/Proxy/MediaPlayerProxy.cpp
        MediaPlayer/Proxy/OffsetManager.cpp
        MediaPlayer/Proxy/BlueToothPlayerProxy.cpp
        get_event.cpp
        DCSKeyHandler.cpp
        ${DLNA_SOURCE_FILES}
        ${LocalTtsProxy}
        ${LocalSynthesizerPlayer}
        ${vendor_AUDIO_INPUT_SOURCE_FILES})

target_include_directories(duer_linux PUBLIC
    "${DCSApp_SOURCE_DIR}/include"
    "${DCSApp_SOURCE_DIR}/include/MediaPlayer"
    "${DCSApp_SOURCE_DIR}/include/MediaPlayer/Impl"
    "${DCSApp_SOURCE_DIR}/include/MediaPlayer/Proxy"
    "${DCSApp_SOURCE_DIR}/include/MediaPlayer/Interface"
    "${DCSApp_SOURCE_DIR}/include/MediaPlayer/Listener"
    "${DLNA_INCLUDE_DIR}"
    "${DeviceIo_SOURCE_DIR}/include"
    "${duerLink_SOURCE_DIR}/include"
    "${PORTAUDIO_INCLUDE_DIR}"
    "${FFMPEG_INCLUDE_DIR}"
    "${ASRTTSManager_SOURCE_DIR}/include"
    "${DeviceTools_SOURCE_DIR}/include"
    "${CrabSdk_SOURCE_DIR}/include"
    )

if(BUILD_ONE_LIB)
    set(DCS_SDK_LIB DcsSdk DeviceTools)
else()
    set(DCS_SDK_LIB DcsClient)
endif()

if(CPU_ARCH STREQUAL "aarch64")
	target_link_libraries(duer_linux utils_bd_ipc)
endif()

target_link_libraries(duer_linux
        ${DCS_SDK_LIB}
        DeviceIo
        duerLink
        "-L${FFMPEG_LIB_PATH}"
        asound
        avcodec
        avutil
        avformat
        swresample
        rt
        m
        pthread
        nghttp2
        LoggerUtils
        Configuration 
        "${TTS_SDK_LIB}"
        "${CRAB_SDK_LIB}"
        "${DLNA_SDK_LIB}"
        iconv
        bd_alsa_audio_client
        curl
        sqlite3
        z
        crypto
        ssl
        )

# use gperftools
if(BUILD_GPERFTOOLS)
    message("启用gperftools")
    target_link_libraries(duer_linux tcmalloc)
endif()

# for unbuntu platform
#target_link_libraries(duer_linux "-L${FFMPEG_LIB_PATH}" asound avcodec avutil avformat
#swresample -L/usr/lib/atlas-base f77blas cblas lapack_atlas atlas)

if(${Platform} STREQUAL Sengled)
   target_link_libraries(duer_linux sqlite3 z curl iconv sai_preprocess)
endif()

if(${Platform} STREQUAL Ingenic)
   target_link_libraries(duer_linux atomic four_mic_front_end_alg_client hardware utils cJSON network_manager)
endif()

install(TARGETS duer_linux DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/${DUEROS_SDK_LIB_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
# install the third libs
install(DIRECTORY ${CMAKE_SOURCE_DIR}/${DUEROS_THIRD_LIB_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}
        PATTERN "*.a" EXCLUDE
        PATTERN "preloadable_libiconv.so" EXCLUDE
        )
# PATTERN "engines" EXCLUDE
# FILES_MATCHING PATTERN "lib*.so*"
