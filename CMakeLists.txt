add_tomahawk_plugin(tomahawk
    TYPE account
    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
    SOURCES
        TomahawkAccount.cpp
        TomahawkAccountConfig.cpp
        sip/TomahawkSip.cpp
    UI
        TomahawkAccountConfig.ui
    LINK_LIBRARIES
        ${TOMAHAWK_LIBRARIES}
    SHARED_LIB
)

