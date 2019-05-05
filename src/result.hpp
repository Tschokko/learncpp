// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#ifndef SRC_ERRORS_HPP_
#define SRC_ERRORS_HPP_

#include <memory>

namespace nsys::result {

template <typename T>
class UniquePtr {
 private:
  bool has_error_;
  union {
    T* value_;
    int error_;
  };
  /*template <typename T_>
  friend UniquePtr<T_> Value(T_* value);
  template <typename T_>
  friend UniquePtr<T_> Error(int error);*/

 public:
  /*bool matchLeft(T1& x) {
    if (isLeft) x = left;
    return isLeft;
  }
  bool matchRight(T2& x) {
    if (!isLeft) x = right;
    return !isLeft;
  }*/

  static UniquePtr<T> Ok(T* value) {
    UniquePtr<T> result;
    result.has_error_ = false;
    result.value_ = value;
    return result;
  }
  static UniquePtr<T> Err(int error) {
    UniquePtr<T> result;
    result.has_error_ = true;
    result.error_ = error;
    return result;
  }

  bool has_error() { return has_error_; }
  int error() { return error_; }
  std::unique_ptr<T> Unwrap() { return std::unique_ptr<T>(value_); }
};

/*template <typename T>
Result<T> Value(T* value) {
  Result<T> e;
  e.has_error_ = false;
  e.value_ = value;
  return e;
}

template <typename T>
Result<T> Error(int error) {
  Result<T> e;
  e.has_error_ = true;
  e.error_ = error;
  return e;
}*/

template <typename T>
class Result {
 private:
  bool has_error_;
  union {
    T result_;
    int error_;
  };

 public:
  static Result Ok(T res) {
    Result<T> r;
    r.has_error_ = false;
    r.result_ = res;
    return r;
  }

  static Result Error(int err) {
    Result<T> r;
    r.has_error_ = true;
    r.error_ = err;
    return r;
  }

  bool has_error() { return has_error_; }
  int error() { return error_; }
  T Unwrap() { return result_; }
};

}  // namespace nsys::result

#endif  // SRC_ERRORS_HPP_