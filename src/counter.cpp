#include "counter.hpp"
#include <algorithm>

//! Создание копии, инкрементирует счетчик
RefCounter::RefCounter(const RefCounter& c) : counter(c.counter) {
    (*counter)++;
}

//! Копирует и при необходимости удаляет старый счетчик
RefCounter& RefCounter::operator = (const RefCounter& c) {
    if (counter != nullptr) {
        if (*counter == 1) delete counter;
        else (*counter)--;
    }
    counter = c.counter;
    (*counter)++;
    return *this;
}

//! Перемещение счетчика
RefCounter::RefCounter(RefCounter&& c) {
    std::swap(counter, c.counter);
    c.counter = nullptr;
}

//! Перемещение счетчика
RefCounter& RefCounter::operator = (RefCounter&& c) {
    std::swap(counter, c.counter);
    return *this;
}

//! Дикрементирует счетчик, либо удаляет его
RefCounter::~RefCounter() {
    if (counter != nullptr) {
        if (*counter == 1) delete counter;
        else (*counter)--;
    }
}
