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

#include "checktu.h"

// Register this check class into cppcheck by creating a static instance of it..
namespace
{
static CheckTU instance;
}

CheckTU::~CheckTU()
{
}
void CheckTU::checks()
{
    TUtils::tokenutils* tu = new TUtils::tokenutils(_tokenizer);
    TUtils::TContext* context = tu->getContext();
    //context->Print();
    checkContext(context);
    checkSwitchContext(context);
}
bool CheckTU::invalid_deallication_check(TUtils::TDeclaration* decl, TUtils::TPar* par)
{
    if ((decl && decl->isArray) || (decl && !decl->isPointer && par->isPointer))
    {
        return true;
    }
    return false;
}
void CheckTU::checkSwitchContext(TUtils::TContext* context)
{
    if (context->ObjectId == TUtils::TSwitchContextId)
    {
        TUtils::TSwitchContext* sc = (TUtils::TSwitchContext*)context;
        APPLY_TO_LIST(TUtils::TSwitchCase*,sc->caseList,switchCaseIt)
        {
            int case_begins=(*switchCaseIt)->beginsAt;
            int case_ends=(*switchCaseIt)->endsAt;
            std::list<TUtils::TSwitchCase*>::iterator next_it=switchCaseIt;
            next_it++;
            /*If this switch case has no break and the next case exists*/
            if ((!(*switchCaseIt)->hasBreak) && (next_it!=sc->caseList.end()))
            {
                int nextCaseBegins=(*next_it)->beginsAt;
                int nextCaseEnds=(*next_it)->endsAt;
                /*Find all assignements in this case*/
                std::list<TUtils::TAssignement*>* caseAssigns = sc->getAllAssignementsWithin(case_begins,case_ends);
                std::list<TUtils::TAssignement*>* nextCaseAssigns = sc->getAllAssignementsWithin(nextCaseBegins,nextCaseEnds);
                APPLY_TO_LIST(TUtils::TAssignement*,(*caseAssigns),caseAssignIt)
                {
                    /*If the same assignements is done in the next case probably it is an error*/
                    TUtils::TAssignement* ass=*caseAssignIt;
                    APPLY_TO_LIST(TUtils::TAssignement*,(*nextCaseAssigns),nextCaseAssignIt)
                    {
                        TUtils::TAssignement* nextAss=*nextCaseAssignIt;
                        if (ass->left == nextAss->left)
                        {
                            reportError(nextAss->token,
                                Severity::error,
                                "Checks using TokenUtils",
                                "Possible case without break");
                        }
                    }
                }
                delete caseAssigns;
                delete nextCaseAssigns;
            }
        }
    }
    APPLY_TO_LIST(TUtils::TContext*,context->contextList,It)
    {
        checkSwitchContext(*It);
    }
}
void CheckTU::checkContext(TUtils::TContext* context)
{
    std::list<TUtils::TCall*> list = context->getAllCalls("free");
    std::list<TUtils::TCall*>::iterator callIt;
    for (callIt = list.begin();callIt != list.end();callIt++)
    {
        if ((*callIt)->parametersList.size() != 1)
        {
            std::cout << "Errore" << std::endl;
        }
        else
        {
            TUtils::TCall* call = *callIt;
            TUtils::TPar* par = *call->parametersList.begin();
            TUtils::TDeclaration* decl = context->getDeclaration(par->name);
            if (decl && decl->alias == NULL && invalid_deallication_check(decl, par))
            {
                //decl->Print();
                reportError(call->token,
                            Severity::error,
                            "autoVariables",
                            "Invalid deallocation");
            }
            else if (decl && decl->alias && invalid_deallication_check(decl->getRealDeclaration(), par))
            {
                reportError(call->token,
                            Severity::error,
                            "autoVariables",
                            "Invalid deallocation");
            }
        }
    }

    APPLY_TO_LIST(TUtils::TContext*,context->contextList,It)
    {
        checkContext(*It);
    }
}