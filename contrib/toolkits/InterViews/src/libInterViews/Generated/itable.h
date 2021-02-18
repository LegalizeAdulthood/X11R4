/*
 * Copyright (c) 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Object association table.
 */

#ifndef InteractorTable_h
#define InteractorTable_h

#include <InterViews/table.h>

class InteractorTableEntry;

class InteractorTable : public Table {
public:
    InteractorTable(int);
    void Insert(void*, class Interactor*);
    boolean Find(class Interactor*&, void*);
    void Remove(void*);
};

inline InteractorTable::InteractorTable (int n) : (n) {}

inline void InteractorTable::Insert (void* k, class Interactor* v) {
    Table::Insert((void*)k, (void*)v);
}

inline boolean InteractorTable::Find (class Interactor*& v, void* k) {
    void* vv;

    boolean b = Table::Find(vv, (void*)k);
    if (b) {
	v = (class Interactor*)vv;
    }
    return b;
}

inline void InteractorTable::Remove (void* k) {
    Table::Remove((void*)k);
}

#endif
