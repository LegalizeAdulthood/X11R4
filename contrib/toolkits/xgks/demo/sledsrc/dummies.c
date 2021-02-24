/*
 *              Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * This file includes:
 *      init_locator
 *      req_locator
 *      in_bounds_delta
 */

#include "changes.h"
#include "trans.h" /* Defines MIN,MAX and includes <xgks.h> */

void init_locator(WSID id, int dev, Gpoint *pos, IDX ntrans, int prompt, Gfloat xmin, Gfloat xmax, Gfloat ymin, Gfloat ymax)
{
    Gloc init;
    Glimit area;
    Glocrec data;

    init.position.x = pos->x;
    init.position.y = pos->y;
    init.transform = ntrans;
    area.xmin = xmin;
    area.xmax = xmax;
    area.ymin = ymin;
    area.ymax = ymax;

    data.pet3.data = NULL;

    ginitloc(id, dev, &init, prompt, &area, &data);
}

void req_locator(WSID id, int dev, Gistat *status, IDX *ntrans, Gpoint *pos)
{
    Gqloc response;

    greqloc(id, dev, &response);

    *status = response.status;
    *ntrans = response.loc.transform;
    pos->x = response.loc.position.x;
    pos->y = response.loc.position.y;
}

BOOLEAN in_bounds_delta(Gpoint *ext, Gpoint point, Gfloat delt)
{
    if (((ext[MIN].x - delt) <= point.x) && ((ext[MIN].y - delt) <= point.y) && ((ext[MAX].x + delt) >= point.x) && ((ext[MAX].y + delt) >= point.y))
        return (TRUE);
    else
        return (FALSE);
}
