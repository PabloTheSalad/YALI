#ifndef STRINGPOOL_HPP
#define STRINGPOOL_HPP

#include <string>
#include <unordered_set>
#include <iostream>
#include "counter.hpp"
#include "types/number.hpp"

using LispString = std::string;
using LispNumber = class Number;

/*!
 * \brief Структура для хранения указателя на объект
 */
template<class T>
struct DataPtr {
    const T* value;
    const T& operator * () const { return *value; }
    const T* operator -> () const { return value; }
    DataPtr(const T& s) : value(&s) {}
    DataPtr(const T* s) : value(s) {}
};

/*!
 * \brief Класс предназначенный для хранения неизменяемых данных
 */
template<class T>
class DataPool : public std::unordered_set<DataPtr<T>> {
    using Parent = std::unordered_set<DataPtr<T>>;
public:
    ~DataPool() {
        for (auto node : *this) {
            delete node.value;
        }
    }
    /*!
     * \brief Находит данные в хранилище либо добавляет их
     * \param v данные которые необходимо добавить
     * \return Возвращает указатель на искомый объект
     */
    void* findOrAdd(const T& v) {
        auto res = Parent::find(DataPtr<T>(v));
        if (res != Parent::end()) return (void*)res->value;
        else {
            std::tie(res, std::ignore) = Parent::insert(DataPtr<T>(new T(v)));
            return (void*)res->value;
        }
    }
};

using LispStringPtr = DataPtr<LispString>;
using LispNumberPtr = DataPtr<LispNumber>;

/*!
 * \brief Структуры небоходимые для работы с стандартным std::unordered_set
 */
namespace std {
  template<>
  struct hash<LispStringPtr> {
  public:
      size_t operator()(const LispStringPtr& v) const {
          std::hash<LispString> h;
          return h(*v);
      }
  };
  template<>
  struct equal_to<LispStringPtr> {
  public:
      bool operator()(const LispStringPtr& l, const LispStringPtr& r) const {
           return *l == *r;
      }
      bool operator()(const LispStringPtr& l, const LispString& r) const {
           return *l == r;
      }
  };

  template<>
  struct hash<LispNumberPtr> {
  public:
      size_t operator()(const LispNumberPtr& v) const {
          std::hash<LispNumber> h;
          return h(*v);
      }
  };
  template<>
  struct equal_to<LispNumberPtr> {
  public:
      bool operator()(const LispNumberPtr& l, const LispNumberPtr& r) const {
           return l->type() == r->type() and *l == *r;
      }
      bool operator()(const LispNumberPtr& l, const LispNumber& r) const {
           return l->type() == r.type() and *l == r;
      }
  };
}

//! Пул строк, при создании строки проверяется содержится ли она в пуле,
//! если нет, то она добавляется туда,
//! иначе возвращается указатель на существующую строку
static DataPool<LispString> stringPool;
//! Пул чисел, при создании числа проверяется содержится ли оно в пуле,
//! если нет, то оно добавляется туда,
//! иначе возвращается указатель на существующее число
static DataPool<LispNumber> numberPool;

#endif // STRINGPOOL_HPP
