#ifndef COUNTER_H
#define COUNTER_H

#include <cstdlib>
#include <limits>

using count_t = size_t;
const count_t MAX_COUNT = std::numeric_limits<count_t>::max();

/*!
 * \brief Класс счетчика, подсчитывает количество ссылок на него
 */
struct RefCounter {
    count_t* counter = nullptr;
    RefCounter() : counter(new count_t(1)) {}
    RefCounter(const RefCounter& c);
    RefCounter& operator = (const RefCounter& c);
    RefCounter(RefCounter&& c);
    RefCounter& operator = (RefCounter&& c);
    bool empty() { return counter == nullptr; }
    count_t count() { return *counter; }
    ~RefCounter();
};

#endif // COUNTER_H
