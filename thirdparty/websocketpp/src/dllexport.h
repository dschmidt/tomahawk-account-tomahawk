#ifndef _WEBSOCKETPP_DLLEXPORT_H
#define _WEBSOCKETPP_DLLEXPORT_H
#ifdef _WIN32
  #define _DLLEXPORT __declspec(dllexport)
#elif __GNUC__ >= 4
  #define _DLLEXPORT __attribute__ ((visibility("default")))
#else
  #define _DLLEXPORT
#endif
#endif
