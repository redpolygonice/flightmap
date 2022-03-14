#ifndef COMMON_H
#define COMMON_H

#include "types.h"

namespace common
{

string i2s(int number);
int s2i(const string &text);
string currentTime();
string currentTimeMs();
int64_t timestamp();
bool isFileExists(const string &fileName);
StringList getComPorts();
string getCurrentDir();
void copyFile(const string &from, const string &to);
void createDir(const string &dir);
bool unzip(const string &fileName, const string &destDir);

}

#endif // COMMON_H
