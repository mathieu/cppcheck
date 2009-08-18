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

#ifndef __tokenutils__
#define __tokenutils__
#include "token.h"
#include "tokenize.h"
#include <list>
#include <iostream>
#include <stdio.h>
#include <sstream>
namespace TUtils
{
static const std::string TObjectId="TObject";
static const std::string TParId="TPar";
static const std::string TCallId="TCall";
static const std::string TContextId="TContext";
static const std::string TSwitchContextId="TSwitchContext";
static const std::string TSwitchCaseId="TSwitchCase";
struct TObject
{
    std::string ObjectId;
    TObject()
    {
        ObjectId=TObjectId;
    }
    std::string toString()
    {
        return ObjectId;
    }
};
struct TPar : public TObject
{
    std::string name;
    bool isPointer;
    bool isContent;
    bool isArray;
    TPar()
    {
        isPointer = false;
        isContent = false;
        isArray = false;
        ObjectId = TParId;
    }
    void Print()
    {
        printf("[%s P%d C%d A%d] ", name.c_str(), isPointer, isContent, isArray);
    }
};
struct TCall : public TObject
{
    std::string name;
    const Token *token;
    int line;
    std::list<TPar*> parametersList;
    TCall()
    {
        line = -1;
        token = NULL;
        ObjectId = TCallId;
    }
    void Print()
    {
        printf("Call to %s at line %d ", name.c_str(), line);
        std::list<TPar*>::iterator parIt;
        for (parIt = parametersList.begin(); parIt != parametersList.end(); parIt++)
        {
            (*parIt)->Print();
        }
        printf("\n");
    }
};
struct TAssignement
{
    std::string left;
    std::string right;
    int line;
    bool leftIsAddress;
    bool leftIsContent;
    bool rightIsAddress;
    bool rightIsContent;

    TAssignement()
    {
        left = "";
        right = "";
        leftIsAddress = false;
        leftIsContent = false;
        rightIsAddress = false;
        rightIsContent = false;
        line = -1;
    }

    void Print()
    {
        printf("Assignement at line %d. Left [%s] A %d C %d Right [%s] A %d C %d\n",
               line,
               left.c_str(),
               leftIsAddress,
               leftIsContent,
               right.c_str(),
               rightIsAddress,
               rightIsContent
              );
    }
};

struct TDeclaration
{
    std::string name;
    std::string type;
    const Token *token;
    bool isPointer;
    bool isSigned;
    bool isUnsigned;
    bool isShort;
    bool isLong;
    bool isLongLong;
    bool isStatic;
    bool isConst;
    bool isArray;
    TDeclaration* alias;
    int line;

    TDeclaration()
    {
        std::string name = "";
        std::string type = "";
        int line;
        isPointer = false;
        isSigned = false;
        isUnsigned = false;
        isShort = false;
        isLong = false;
        isLongLong = false;
        isStatic = false;
        isConst = false;
        isArray = false;
        line = 0;
        token = NULL;
        alias = NULL;
    }

    TDeclaration* getRealDeclaration()
    {
        TDeclaration* decl = alias;
        while (decl->alias)
        {
            decl = decl->alias;
        }
        return decl;
    }
    void Print()
    {
        std::string str = "";
        if (alias)
            str = alias->name;
        printf("Var %s (type %s) declared at %d: P %d SIGNED %d UNSIGNED %d SHORT %d LONG %d LL %d STATIC %d CONST %d ARRAY %d ALIAS %s\n",
               name.c_str(),
               type.c_str(),
               line,
               isPointer,
               isSigned,
               isUnsigned,
               isShort,
               isLong,
               isLongLong,
               isStatic,
               isConst,
               isArray,
               str.c_str());

    }
};

struct TContext : public TObject
{
    int beginsAt;
    int endsAt;
    TContext* parent;
    bool isFirstContext;
    std::list<TDeclaration*> declarationList;
    std::list<TAssignement*> assignementList;
    std::list<TCall*> callList;
    std::list<TContext*> contextList;

    TDeclaration * getDeclaration(std::string name)
    {
        std::list<TDeclaration*>::iterator declIt;
        for (declIt = declarationList.begin(); declIt != declarationList.end(); declIt++)
        {
            if ((*declIt)->name == name)
                return *declIt;
        }
        if (!isFirstContext)
        {
            return parent->getDeclaration(name);
        }
        return NULL;
    }
    TCall* getFirstCall(std::string name)
    {
        std::list<TCall*>::iterator callIt;
        for (callIt = callList.begin();callIt != callList.end();callIt++)
        {
            if ((*callIt)->name == name)
                return *callIt;
        }
        return NULL;
    }

    std::list<TCall*> getAllCalls(std::string name)
    {
        std::list<TCall*> list;
        std::list<TCall*>::iterator callIt;
        for (callIt = callList.begin();callIt != callList.end();callIt++)
        {
            if ((*callIt)->name == name)
                list.push_back(*callIt);
        }
        return list;
    }

    TContext()
    {
        beginsAt = -1;
        endsAt = -1;
        parent = NULL;
        isFirstContext = true;
        ObjectId=TContextId;
    }

    ~TContext()
    {
        std::list<TDeclaration*>::iterator it;
        for (it = declarationList.begin(); it != declarationList.end(); it++)
        {
            delete *it;
        }
        declarationList.clear();
        std::list<TContext*>::iterator it2;
        for (it2 = contextList.begin(); it2 != contextList.end(); it2++)
        {
            delete *it2;
        }
        contextList.clear();
    }

    virtual void Print()
    {
        printf("Context from line %d to line %d {\n", beginsAt, endsAt);
        std::list<TDeclaration*>::iterator declIt;
        printf("Declarations{\n");
        for (declIt = declarationList.begin(); declIt != declarationList.end(); declIt++)
        {
            (*declIt)->Print();
        }
        printf("}\n");
        printf("Assignements{\n");
        std::list<TAssignement*>::iterator assIt;
        for (assIt = assignementList.begin();assIt != assignementList.end();assIt++)
        {
            (*assIt)->Print();
        }
        printf("}\n");

        printf("Calls{\n");
        std::list<TCall*>::iterator callIt;
        for (callIt = callList.begin();callIt != callList.end();callIt++)
        {
            (*callIt)->Print();
        }
        printf("}\n");

        printf("Sub Context{\n");
        std::list<TContext*>::iterator scIt;
        for (scIt = contextList.begin(); scIt != contextList.end(); scIt++)
        {
            (*scIt)->Print();
        }
        printf("}\n");
        printf("}\n");
    }
};
struct TSwitchCase : public TObject
{
    int beginsAt;
    int endsAt;
    bool hasBreak;
    bool isDefault;
    TSwitchCase()
    {
        beginsAt=-1;
        endsAt=-1;
        hasBreak=false;
        isDefault=false;
        ObjectId=TSwitchCaseId;
    }
    void Print()
    {
        printf("case B %d E %d HB %d ID %d\n",beginsAt,endsAt,hasBreak,isDefault);
    }
};
struct TSwitchContext : public TContext
{
    bool isSwitch;
    std::list<TSwitchCase*> caseList;
    TSwitchContext()
    {
        TContext();
        isSwitch=true;
        ObjectId=TSwitchContextId;
    }
    void Print()
    {
        printf ("Switch ");
        TContext::Print();
        printf("Switch cases{\n");
        std::list<TSwitchCase*>::iterator cIt;
        for (cIt = caseList.begin();cIt != caseList.end();cIt++)
        {
            (*cIt)->Print();
        }
        printf("}");
    }
};
void checkSingleContext(TContext* context);

class tokenutils
{
private:
    std::list<TDeclaration*> _list;
    bool first;
    TContext* context;
    static const std::string keywords[];
    static const std::string operators[];
public:

    TDeclaration* getDeclData(std::string varName);

    const Token *createContext(const Token *tok, TContext* parent, TContext* current, bool first);

    TContext* getContext();

    tokenutils(const Tokenizer *tokenizer);

    ~tokenutils();

    static bool isLanguageKeyword(const Token *tok);

    static bool isLanguageOperator(const Token *tok);

    static TAssignement* getAssignement(const Token *tok, int* shift);

    static TDeclaration* getDeclaration(const Token *tok, int* shift);

    static TCall* getCall(const Token *tok, int* shift);
};
}
#endif // __tokenutils__
