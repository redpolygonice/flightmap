#ifndef COMMON_H
#define COMMON_H

#include "types.h"

namespace common
{

string CurrentTime();
string currentTimeMs();
int64_t Timestamp();
bool IsFileExists(const string &fileName);
StringList GetComPorts();
string GetCurrentDir();
void CopyFile(const string &from, const string &to);
void CreateDir(const string &dir);
bool Unzip(const string &fileName, const string &destDir);
inline void Sleep(long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
StringList SplitString(const std::string &text, const std::string &separator);
bool replace(string& str, const string& from, const string& to);

}

#endif // COMMON_H
