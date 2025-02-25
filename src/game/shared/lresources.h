#ifndef LRESOURCES_H
#define LRESOURCES_H
#ifdef _WIN32
#pragma once
#endif

int ResourcesGetFilesCount();
const char *ResourcesGetFile( int i );
void ResourcesFreeFiles();

#endif  // LRESOURCES_H
