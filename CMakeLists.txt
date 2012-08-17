add_tomahawk_plugin(tomahawk
    TYPE account
    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
    SOURCES
        TomahawkAccount.cpp
        TomahawkAccountConfig.cpp
    UI
        TomahawkAccountConfig.ui
    LINK_LIBRARIES
        ${TOMAHAWK_LIBRARIES}
)

