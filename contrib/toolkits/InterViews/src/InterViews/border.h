/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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
 * A border is simply a infinitely stretchable line.
 */

#ifndef border_h
#define border_h

#include <InterViews/interactor.h>

class Border : public Interactor {
protected:
    int thickness;

    Border(int);
    Border(const char*, int);
    Border(Painter*, int);

    virtual void Redraw(Coord, Coord, Coord, Coord);
};

class HBorder : public Border {
public:
    HBorder(int thick = 1);
    HBorder(const char*, int thick = 1);
    HBorder(Painter* out, int thick = 1);
protected:
    virtual void Reconfig();
private:
    void Init();
};

class VBorder : public Border {
public:
    VBorder(int thick = 1);
    VBorder(const char*, int thick = 1);
    VBorder(Painter* out, int thick = 1);
protected:
    virtual void Reconfig();
private:
    void Init();
};

#endif
