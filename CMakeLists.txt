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

add_tomahawk_plugin(facebook
    TYPE account
    EXPORT_MACRO ACCOUNTDLLEXPORT_PRO
    SOURCES
        facebook/FacebookAccount.cpp
        facebook/FacebookAccountConfig.cpp
        facebook/FacebookSip.cpp
        facebook/FacebookAuthDialog.cpp
    UI
        facebook/FacebookAccountConfig.ui
    LINK_LIBRARIES
        ${TOMAHAWK_LIBRARIES}
        tomahawk_account_tomahawk
)
