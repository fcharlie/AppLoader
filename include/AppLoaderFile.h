///
///
#ifndef APPLOADER_FILE_H
#define APPLOADER_FILE_H
#define WIN32_LEAN_AND_MEAN
#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>

class Object;
class AppLoaderFile;

#define MAXSIZEFILE (1UL<<20)

class Object {
public:
  enum ObjectType {
    Number, ////////////
    Boolean,
    String,
    Array
  };
  Object(int64_t i) {
    number_ = i;
    type_ = Number;
  }
  Object(bool b) {
    boolean_ = b;
    type_ = Boolean;
  }
  Object(wchar_t *str, size_t len) {
    string_ = new std::wstring(str, len);
    type_ = String;
  }
  Object() {
    array_ = new std::vector<Object>();
    type_ = Array;
  }
  Object(const Object &other) {
    type_ = other.type_;
    switch (type_) {
    case Number:
      number_ = other.number_;
      break;
    case Boolean:
      boolean_ = other.boolean_;
      break;
    case String: {
      string_ = new std::wstring();
      if (other.string_) {
        string_->assign(*(other.string_));
      }
    } break;
    case Array: {
      array_ = new std::vector<Object>();
      if (other.array_) {
        auto iter = other.array_->begin();
        auto end = other.array_->end();
        for (; iter != end; iter++) {
          array_->push_back(*iter);
        }
      }
    } break;
    default:
      break;
    }
  }
  Object(Object &&other) {
    type_ = other.type_;
    switch (type_) {
    case Number:
      number_ = other.number_;
      break;
    case Boolean:
      boolean_ = other.boolean_;
      break;
    case String: {
      string_ = new std::wstring();
      if (other.string_) {
        string_->assign(std::move(*(other.string_)));
      }
    } break;
    case Array: {
      array_ = new std::vector<Object>();
      if (other.array_) {
        for (auto &i : *other.array_) {
          array_->push_back(std::move(i));
        }
      }
    } break;
    default:
      break;
    }
  }
  ~Object() {
    if (type_ == String) {
      if (string_)
        delete string_;
    } else if (type_ == Array) {
      if (array_)
        delete array_;
    }
  }

  bool IsNumber() const { return type_ == Number; }
  bool IsBoolean() const { return type_ == Boolean; }
  bool IsString() const { return type_ == String; }
  bool IsArray() const { return type_ == Array; }
  template <class IntegerT> IntegerT AsNumber() const {
    return static_cast<IntegerT>(number_);
  }
  bool AsBoolean() const {
    assert(type_ == Boolean);
    return boolean_;
  }
  const std::wstring &AsString() const {
    assert(type_ == String && string_);
    return *string_;
  }
  bool add(Object &o) {
    assert(type_ == Array);
    array_->push_back(o);
    return true;
  }
  /// This Array Member ;
  std::vector<Object>::iterator begin() {
    assert(type_ == Array);
    return array_->begin();
  }

  std::vector<Object>::iterator end() {
    assert(type_ == Array);
    return array_->end();
  }

private:
  friend class AppLoaderFile;
  union {
    int64_t number_;
    bool boolean_;
    std::wstring *string_;
    std::vector<Object> *array_;
  };
  int type_;
};

class AppLoaderFileMemView;
class AppLoaderFile {
public:
  typedef std::unordered_map<std::wstring, Object> ObjectsT;
  AppLoaderFile(const wchar_t *file);
  ~AppLoaderFile();
  bool Parse();
  ObjectsT::iterator At(const std::wstring &k) { return Objects_.find(k); }
  ObjectsT::iterator ObjectEnd() { return Objects_.end(); }
  const ObjectsT &Objects() const { return Objects_; }

private:
  const wchar_t *metafile;
  AppLoaderFileMemView *view;
  ObjectsT Objects_;
};

BOOL WINAPI AppLoaderFilterIcon(LPCWSTR file, LPWSTR iconfile, DWORD counts);

#endif
