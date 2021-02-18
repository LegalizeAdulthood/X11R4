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

#ifndef FontTable_h
#define FontTable_h

#include <InterViews/table.h>

class FontTableEntry;

class FontTable : public Table {
public:
    FontTable(int);
    void Insert(class StringId*, class FontRep*);
    boolean Find(class FontRep*&, class StringId*);
    void Remove(class StringId*);
};

inline FontTable::FontTable (int n) : (n) {}

inline void FontTable::Insert (class StringId* k, class FontRep* v) {
    Table::Insert((void*)k, (void*)v);
}

inline boolean FontTable::Find (class FontRep*& v, class StringId* k) {
    void* vv;

    boolean b = Table::Find(vv, (void*)k);
    if (b) {
	v = (class FontRep*)vv;
    }
    return b;
}

inline void FontTable::Remove (class StringId* k) {
    Table::Remove((void*)k);
}

#endif
