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
    struct TPar
    {
        std::string name;
        bool isPointer;
        bool isContent;
        bool isArray;
        TPar()
        {
            isPointer=false;
            isContent=false;
            isArray=false;
        }
        void Print()
        {
            printf("[%s P%d C%d A%d] ",name.c_str(),isPointer,isContent,isArray);
        }
    };
    struct TCall
    {
        std::string name;
        int line;
        std::list<TPar*> parametersList;
        TCall()
        {
            line=-1;
        }
        void Print()
        {
            printf("Call to %s at line %d ",name.c_str(),line);
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
        }

        void Print()
        {
            printf("Var %s (type %s) declared at %d: P %d SIGNED %d UNSIGNED %d SHORT %d LONG %d LL %d STATIC %d CONST %d ARRAY %d\n",
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
                    isArray);

        }
    };

    struct TContext
    {
        int beginsAt;
        int endsAt;
        std::list<TDeclaration*> declarationList;
        std::list<TAssignement*> assignementList;
        std::list<TCall*> callList;
        std::list<TContext*> contextList;

        TDeclaration * getDeclaration(std::string name)
        {
            std::list<TDeclaration*>::iterator declIt;
            for (declIt = declarationList.begin(); declIt != declarationList.end(); declIt++)
            {
                if ((*declIt)->name==name)
                    return *declIt;
            }
            return NULL;
        }
	TCall* getFirstCall(std::string name)
	{
            std::list<TCall*>::iterator callIt;
            for(callIt=callList.begin();callIt!=callList.end();callIt++)
            {
                if ((*callIt)->name==name)
                    return *callIt;
            }
            return NULL;
        }

        TContext()
        {
            beginsAt = -1;
            endsAt = -1;
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

        void Print()
        {
            printf("Context from line %d to line %d {\n",beginsAt,endsAt);
            std::list<TDeclaration*>::iterator declIt;
            printf("Declarations{\n");
            for (declIt = declarationList.begin(); declIt != declarationList.end(); declIt++)
            {
                (*declIt)->Print();
            }
            printf("}\n");
            printf("Assignements{\n");
            std::list<TAssignement*>::iterator assIt;
            for(assIt=assignementList.begin();assIt!=assignementList.end();assIt++)
            {
                (*assIt)->Print();
            }
            printf("}\n");

            printf("Calls{\n");
            std::list<TCall*>::iterator callIt;
            for(callIt=callList.begin();callIt!=callList.end();callIt++)
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

        const Token *createContext(const Token *tok, TContext* parent,TContext* current);

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
