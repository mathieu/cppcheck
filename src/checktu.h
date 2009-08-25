/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2009 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef _CHECKTU_H
#define	_CHECKTU_H
#include <list>
#include "check.h"
#include "token.h"
#include "tokenutils.h"

/// @addtogroup Checks
/// @{
class CheckTU : public Check
{
public:
    /** This constructor is used when registering the CheckClass */
    CheckTU() : Check()
    { }
    /** This constructor is used when running checks.. */
    CheckTU(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
            : Check(tokenizer, settings, errorLogger)
    { }
    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
    {
        CheckTU checkTU(tokenizer, settings, errorLogger);
        checkTU.checks();
    }
    void checks();
    void checkContext(TUtils::TContext* context);
    bool invalid_deallication_check(TUtils::TDeclaration* decl, TUtils::TPar* par);
    void checkSwitchContext(TUtils::TContext* context);
    virtual ~CheckTU();
private:
    void getErrorMessages()
    {

    }

    std::string name() const
    {
        return "Token Utils Checks";
    }

    std::string classInfo() const
    {
        return "";
    }
};
/// @}
//---------------------------------------------------------------------------
#endif	/* _CHECKTU_H */

