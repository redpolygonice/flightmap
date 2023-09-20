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

#include <mavlink/common/mavlink.h>
#include <mavlink/ardupilotmega/ardupilotmega.h>

#define BUFFER_SIZE 1024
#define MAX_BUFFER_SIZE 1024 * 1024

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
typedef std::unique_ptr<string> StringPtr;

// Byte array
typedef std::vector<unsigned char> ByteArray;
typedef std::vector<unsigned char>::iterator ByteArrayIterator;
typedef std::vector<unsigned char>::const_iterator ByteArrayConstIterator;
typedef std::unique_ptr<ByteArray> ByteArrayPtr;

// Variant map
typedef std::variant<string, bool, int32_t, uint32_t, double, time_t> ParamVar;
typedef std::map<string, ParamVar> ParamMap;
typedef std::map<string, ParamVar>::iterator ParamMapIterator;
typedef std::map<string, ParamVar>::const_iterator ParamMapConstIterator;

// Any map
typedef std::map<string, std::any> AnyMapType;
typedef std::map<string, std::any>::iterator AnyMapIterator;
typedef std::map<string, std::any>::const_iterator AnyMapConstIterator;

typedef std::function<void(const ByteArray &)> ReadFunction;

enum class KeyCode : int
{
	Up = 0x01000013,
	Down = 0x01000015,
	Left = 0x01000012,
	Right = 0x01000014,
	W = 0x57,
	S = 0x53,
	A = 0x41,
	D = 0x44
};

#endif // TYPES_H
