// Platform.h

#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(_WIN32) || defined(__WIN32__)
#ifndef __WINDOWS__
#define __WINDOWS__
#endif
#endif

#ifdef __WINDOWS__
#define FOLDER_SEPARATOR TEXT('\\')
#define FOLDER_SEPARATOR_STRING TEXT("\\")
#define FOLDER_POSSIBLE_SEPARATOR TEXT("\\/")
#else
#define FOLDER_SEPARATOR TEXT('/')
#define FOLDER_SEPARATOR_STRING TEXT("/")
#define FOLDER_POSSIBLE_SEPARATOR TEXT("/")
#endif

#endif // PLATFORM_H
