#pragma once

// ****************************************************************************
// A simple templated class to make sure your class cannot be copyable
// I.e: class MyClass: private TNonCopyable<MyClass>
// ****************************************************************************
template <class T>
class TNonCopyable {
public:
  TNonCopyable(const TNonCopyable &) = delete;
  T& operator= (const T&) = delete;

protected:
  TNonCopyable() = default;
  ~TNonCopyable() = default;
};