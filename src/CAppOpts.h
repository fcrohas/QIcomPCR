/**********************************************************************************************
    Copyright (C) 2009 Joerg Wunsch <j@uriah.heep.sax.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
#ifndef CAPPOPTS_H
#define CAPPOPTS_H
/*
 * This class holds the options passed from the command-line,
 * including the positional arguments.
 */

class CAppOpts
{
    public:
        const bool debug;        // -d, print debug messages
        const int monitor;       // -m FD, monitor file descriptor
        const bool nosplash;     // -n, do not display splash screen
                                 // positional arguments (files to load)
        const QString configfile;
        const QStringList arguments;

        CAppOpts(bool d, int m, bool n, const QString& c, const QStringList& a)
            : debug(d)
            , monitor(m)
            , nosplash(n)
            , configfile(c)
            , arguments(a) {}
        virtual ~CAppOpts() {}
};

extern CAppOpts *qlOpts;

#endif //CAPPOPTS_H
