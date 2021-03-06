include(QmlPlugins)

# Set up and optionally install a mock plugin for unity8.
#
# add_unity8_mock(plugin version path
#     [PREFIX string]  # Install the plugin to the specified prefix under unity8 private plugin directory
#     [...]            # See export_qmlfiles and export_qmlplugin documentation for more options
# )

macro(add_unity8_mock PLUGIN VERSION PATH)
    set(single PREFIX)
    cmake_parse_arguments(MOCK "" "${single}" "" ${ARGN})

    if (NOT MOCK_PREFIX)
        set(MOCK_PREFIX mocks)
    endif()

    export_qmlfiles(${PLUGIN} ${PATH}
        DESTINATION ${SHELL_INSTALL_QML}/${MOCK_PREFIX}
        TARGET_PREFIX ${MOCK_PREFIX}-Mock
        ${MOCK_UNPARSED_ARGUMENTS}
    )
    export_qmlplugin(${PLUGIN} ${VERSION} ${PATH}
        DESTINATION ${SHELL_INSTALL_QML}/${MOCK_PREFIX}
        TARGET_PREFIX ${MOCK_PREFIX}-Mock
        ${MOCK_UNPARSED_ARGUMENTS}
    )
endmacro()

add_subdirectory(AccountsService)
add_subdirectory(Biometryd)
add_subdirectory(Cursor)
add_subdirectory(GSettings.1.0)
add_subdirectory(indicator-service)
add_subdirectory(liblightdm)
add_subdirectory(libusermetrics)
add_subdirectory(LightDMController)
add_subdirectory(Lights)
add_subdirectory(MeeGo)
add_subdirectory(Powerd)
add_subdirectory(QMenuModel)
add_subdirectory(SessionBroadcast)
add_subdirectory(Ubuntu)
add_subdirectory(Unity)
add_subdirectory(QtMultimedia)
add_subdirectory(Wizard)
add_subdirectory(Utils)
add_subdirectory(UInput)

install(
  DIRECTORY data
  DESTINATION ${SHELL_APP_DIR}/mocks
)
