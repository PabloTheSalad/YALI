#ifndef PAIRLIST_H
#define PAIRLIST_H

#include "lispvalue.hpp"

struct PairList {
    LispValue head;
    LispValue tail;
    PairList(const PairList& o) : head(o.head), tail(o.tail) {}
    PairList(const std::initializer_list<LispValue>& list);
    PairList(const LispValue& h, const LispValue& t) : head(h), tail(t) {}
    class iterator {
        LispValue node;
        friend struct PairList;
    public:
        LispValue& operator * () { return node.list().head; }
        const LispValue* operator -> () { return  &node.list().head; }
        iterator operator ++ () { node = node.list().tail; return *this; } //Прекремент
        iterator operator ++ (int) {
            auto tmp = node;
            node = node.list().tail;
            return iterator(tmp);
        }
        bool operator == (const iterator& other) { return node == other.node; }
        bool operator != (const iterator& other) { return node != other.node; }
        LispValue get() const { return node; }
        iterator(const LispValue& n) : node(n) {}
    };
    iterator begin() const { return iterator(*this); }
    iterator end() const { return iterator({}); }
    void push_back(const LispValue& v);
    void set_tail(const LispValue&);
    void set_end(const LispValue&);
    //Использование нежелательно
    size_t size() const;

    inline LispValue& front() { return head; }
    inline const LispValue& front() const { return head; }
    LispValue& back();
    const LispValue& back() const;
    bool operator == (const PairList& o) const;
    inline LispValue rest() const { return tail; }
};

namespace std {

template <>
struct iterator_traits<PairList::iterator> {
    using iterator_category = std::forward_iterator_tag;
    using value_type = LispValue;
    using difference_type = size_t;
};

}

#endif // PAIRLIST_H
