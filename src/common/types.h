#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <memory>
#include <fstream>
#include <atomic>
#include <any>
#include <variant>
#include <future>

using std::string;
using std::wstring;

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;

//#include <filesystem>
//namespace fs = std::filesystem;

#define BUFFER_SIZE 4096

// Path separator
#ifdef WIN32
#define PS "\\"
#else
#define PS "/"
#endif

// String list
typedef std::vector<string> StringList;
typedef std::vector<string>::iterator StringListIterator;
typedef std::vector<string>::const_iterator StringListConstIterator;
typedef std::shared_ptr<string> StringPtr;

// Byte array
typedef std::vector<unsigned char> ByteArray;
typedef std::vector<unsigned char>::iterator ByteArrayIterator;
typedef std::vector<unsigned char>::const_iterator ByteArrayConstIterator;

// Variant map
typedef std::variant<string, bool, int32_t, uint32_t, double, time_t> ParamVar;
typedef std::map<string, ParamVar> ParamMap;
typedef std::map<string, ParamVar>::iterator ParamMapIterator;

// Any map
typedef std::map<string, std::any> AnyMapType;
typedef std::map<string, std::any>::iterator AnyMapIterator;
typedef std::map<string, std::any>::const_iterator AnyMapConstIterator;

typedef std::function<void(const ByteArray &)> ReadFunction;

#endif // TYPES_H
