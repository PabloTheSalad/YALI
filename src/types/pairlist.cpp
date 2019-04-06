#include "pairlist.hpp"
#include "exception.hpp"

PairList::PairList(const std::initializer_list<LispValue>& list) {
    if (list.size() == 0) head = {};
    else head = *list.begin();
    if (list.size() > 1) {
        tail = PairList{*(list.begin()+1)};
        for (auto it = list.begin()+2; it != list.end(); it++) {
            tail.list().push_back(*it);
        }
    } else tail = {};
}

void PairList::push_back(const LispValue& v) {
    if (tail.isNull()) {
        tail = PairList{v};
    } else tail.list().push_back(v);
}

void PairList::set_tail(const LispValue& v) {
    tail = v;
}

void PairList::set_end(const LispValue& v) {
    if (tail.isNull()) {
        tail = v;
    } else tail.list().set_end(v);
}

size_t PairList::size() const {
    if (tail.isNull()) return 1;
    else if (tail.type != LispValue::List) {
        throw badArg("length", tail, 1, "Expected list");
    }
    else return tail.list().size() + 1;
}

LispValue& PairList::back() {
    if (tail.isNull()) return head;
    else return tail.list().back();
}

const LispValue& PairList::back() const {
    if (tail.isNull()) return head;
    else return tail.list().back();
}

bool PairList::operator == (const PairList& o) const {
    return head == o.head and tail == o.tail;
}
