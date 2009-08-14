#include "tokenutils.h"
namespace TUtils
{

    const std::string tokenutils::keywords[]=
        {
        "if",
        "else",
        "switch",
        "case",
        "for",
        "do",
        "while",
        "break",
        "continue",
        "try",
        "catch",
        "typedef",
        "class",
        "struct",
        "union",
        "new",
        "delete",
        ""};
    const std::string tokenutils::operators[]=
    {
      "+","-","*","/","\\",",","&","|","&&","||",""
    };
    bool isDeclaredInSubContext(TUtils::TContext* context,TUtils::TDeclaration* decl, TUtils::TDeclaration** other)
    {
            bool res=false;
            std::list<TUtils::TDeclaration*>::iterator it;
            for(it=context->declarationList.begin();it!=context->declarationList.end();it++)
            {
                    if ((*it)->name==decl->name)
                    {
                            *other=*it;
                            return true;
                    }
            }

            std::list<TUtils::TContext*>::iterator it2;
            for(it2=context->contextList.begin();it2!=context->contextList.end();it2++)
            {
                    res=res||isDeclaredInSubContext(*it2,decl,other);
            }

            return res;
    }
    void checkSingleContext(TUtils::TContext* context)
    {
        std::list<TUtils::TDeclaration*>::iterator declarationIt;
        for (declarationIt = context->declarationList.begin(); declarationIt != context->declarationList.end(); declarationIt++)
        {
            TUtils::TDeclaration* decl = *declarationIt;
            //decl->Print();
            TUtils::TDeclaration* res = NULL;

            std::list<TUtils::TContext*>::iterator subcontextIt;
            for (subcontextIt = context->contextList.begin(); subcontextIt != context->contextList.end(); subcontextIt++)
            {
                if (isDeclaredInSubContext(*subcontextIt, decl, &res) && (res->type == decl->type))
                {
                    printf("Variable %s declared at %d hides declaration at %d\n", res->name.c_str(), res->line, decl->line);
                    //std::cout << "Variable declared at " << res->line << " hides declaration at " << decl->line << std::endl;
                }
                checkSingleContext(*subcontextIt);
            }

        }
    }

    TDeclaration* tokenutils::getDeclData(std::string varName)
    {
        std::list<TDeclaration*>::iterator it;
        for (it = _list.begin(); it != _list.end(); it++)
        {
            if ((*it)->name == varName)
                return *it;
        }
        return NULL;
    }

    const Token * tokenutils::createContext(const Token *tok, TContext* parent, TContext* current, bool first)
    {
        const Token* t=tok;
        while(t)
        {
            if (Token::Match(t, "{"))
            {

                //std::cout << t->str() << std::endl;
                TContext* cont = new TContext();
                cont->beginsAt = t->linenr();
                t=createContext(t->next(),current,cont,false);
                t=t->next();
                //return;
            } else if (Token::Match(t, "}"))
            {
                //Context closed
                current->endsAt = t->linenr();
                current->parent=parent;
                current->isFirstContext=first;
                parent->contextList.push_back(current);
                //current->Print();
                return t;
            } else
            {
                int shift;
                TDeclaration* decl = getDeclaration(t, &shift);
                if (decl)
                {
                    //decl->Print();
                    current->declarationList.push_back(decl);
                    t = t->tokAt(shift);
                }
                TAssignement* assign = getAssignement(t, &shift);
                if (assign)
                {
                    TDeclaration* rdecl=current->getDeclaration(assign->right);
                    TDeclaration* ldecl=current->getDeclaration(assign->left);
                    if (ldecl && rdecl)
                    {
                        ldecl->alias=rdecl;
                    }
                    current->assignementList.push_back(assign);
                    //assign->Print();
                    t=t->tokAt(shift);
                }
                TCall* call=getCall(t, &shift);
                if (call)
                {
                    current->callList.push_back(call);
                    t = t->tokAt(shift);
                }
                if (!decl && !assign && !call)
                    t=t->next();
            }
        }
        return NULL;
    }

    TContext* tokenutils::getContext()
    {
        return this->context;
    }

    tokenutils::tokenutils(const Tokenizer *tokenizer)
    {
        first = true;
        context = new TContext();
        const Token *tok = tokenizer->tokens();
        const Token* last_tok;
        last_tok=createContext(tok, context,context,true);
    }

    tokenutils::~tokenutils()
    {
        std::list<TDeclaration*>::iterator it;
        for (it = _list.begin(); it != _list.end(); it++)
        {
            delete *it;
        }
        _list.clear();
    }

    TAssignement* tokenutils::getAssignement(const Token *tok, int* shift)
    {
        TAssignement* res = NULL;
        if (Token::Match(tok, " = "))
        {
            res = new TAssignement();
            res->line = tok->linenr();
            std::ostringstream oss;
            const Token *t = tok->previous();
            //Backward
            while (!Token::Match(t, ";|)|}"))
            {
                t = t->previous();
            }
            //Farward
            t = t->next();
            while (!Token::Match(t, " = "))
            {
                if (Token::Match(t, " & "))
                {
                    res->leftIsAddress = true;
                } else if (Token::Match(t, " * "))
                {
                    res->leftIsContent = true;
                } else
                {
                    oss << t->str();
                }
                t = t->next();
            }
            res->left = oss.str();

            std::ostringstream oss2;
            t = tok->tokAt(1);
            while (!Token::Match(t, ";"))
            {
                if (Token::Match(t, " & "))
                {
                    res->rightIsAddress = true;
                } else if (Token::Match(t, " * "))
                {
                    res->rightIsContent = true;
                } else
                {
                    oss2 << t->str();
                }
                t = t->next();
            }
            res->right = oss2.str();
            *shift = 3;
        }
        return res;
    }
    TDeclaration* tokenutils::getDeclaration(const Token *tok, int* shift)
    {
        TDeclaration* res = NULL;
        //std::cout << tok->str() << std::endl;
        if (Token::Match(tok, "%type% %var% ;|[")) {
            res = new TDeclaration();
            res->name = tok->tokAt(1)->str();
            res->type = tok->tokAt(0)->str();
            res->line = tok->linenr();
            res->token = tok;
            *shift = 2;
            if (tok->tokAt(2)->str() == "[") {
                res->isArray = true;
                (*shift)++;
            }
        }//TODO * *, * * * (pointer pointer pointer)
        else if (Token::Match(tok, "%type% * %var% ;|["))
        {
            res = new TDeclaration();
            res->name = tok->tokAt(2)->str();
            res->type = tok->tokAt(0)->str();
            res->isPointer = true;
            res->line = tok->linenr();
            res->token = tok;
            *shift = 3;
            if (tok->tokAt(3)->str() == "[")
            {
                res->isArray = true;
                (*shift)++;
            }
        }
        if (res == NULL)
            return NULL;
        //std::cout << "Token iniziale " << tok->str() << std::endl;

        const Token *t = tok->previous();
        while (!Token::Match(t, ";|)|}|{|("))
        {
            if (Token::Match(t, "signed"))
                res->isSigned = true;
            else if (Token::Match(t, "unsigned"))
                res->isUnsigned = true;
            else if (Token::Match(t, "short"))
                res->isShort = true;
            else if (Token::Match(t, "long"))
                res->isLong = true;
            else if (Token::Match(t, "longlong")) //TODO check
                res->isLongLong = true;
            else if (Token::Match(t, "static"))
                res->isStatic = true;
            else if (Token::Match(t, "const"))
                res->isConst = true;

            t = t->previous();
        }

        return res;
    }

    bool tokenutils::isLanguageOperator(const Token *tok)
    {
        bool res = false;

        for (unsigned int i = 0; operators[i] != ""; i++)
        {
            if (tok->str() == operators[i])
            {
                res = true;
                break;
            }
        }
        return res;
    }

    bool tokenutils::isLanguageKeyword(const Token *tok)
    {
        bool res = false;

        for (unsigned int i = 0; keywords[i] != ""; i++)
        {
            if (tok->str() == keywords[i])
            {
                res = true;
                break;
            }
        }
        return res;
    }

    TCall* tokenutils::getCall(const Token *tok, int* shift)
    {
        TCall* res = NULL;
        int indent = 0;
        int line_nr = 0;
        std::string name;
        std::list<TUtils::TPar*> pList;
        int ls = 0;

        if (Token::Match(tok, "%any% ("))
        {
            if (isLanguageKeyword(tok) && !Token::Match(tok->tokAt(2), "%any% ("))
                return NULL;

            line_nr = tok->linenr();
            name = tok->str();

            for (const Token *t = tok; t; t = t->next())
            {
                ls++;
                if (Token::Match(t, " ( "))
                    indent++;
                else if (Token::Match(t, " ) ; "))
                {
                    indent--;
                    if (indent == 0)
                    {
                        res = new TCall();
                        res->line = line_nr;
                        res->name = name;
                        std::list<TPar*>::iterator parIt;
                        for (parIt = pList.begin(); parIt != pList.end(); parIt++)
                        {
                            res->parametersList.push_back(*parIt);
                        }
                        *shift = ls;
                        break;
                    }
                }
                else if (indent==1 && (Token::Match(t, "%any% ,") || Token::Match(t, "%any% )")))
                {
                    TPar* par=new TPar();
                    par->name=t->str();
                    pList.push_back(par);
                    //t = t->next();
                }
                else if (indent==1 && (Token::Match(t, "* %any% ,") || Token::Match(t, "* %any% )")))
                {
                    TPar* par=new TPar();
                    par->name=t->strAt(1);
                    par->isContent=true;
                    pList.push_back(par);
                    //t = t->next();
                    t = t->next();
                }
                else if (indent==1 && (Token::Match(t, "& %any% ,") || Token::Match(t, "& %any% )")))
                {
                    TPar* par=new TPar();
                    par->name=t->strAt(1);
                    par->isPointer=true;
                    pList.push_back(par);
                    //t = t->next();
                    t = t->next();
                }
                else if (Token::Match(t, " ) "))
                {
                    indent--;
                    //t->printOut();
                    if (indent == 0 && (isLanguageOperator(t->next()) || Token::Match(t->next(), ")")))
                    //if (indent == 0 && (Token::Match(t->next(), " ) ")))
                    {
                        res = new TCall();
                        res->line = line_nr;
                        res->name = name;
                        std::list<TPar*>::iterator parIt;
                        for (parIt = pList.begin(); parIt != pList.end(); parIt++)
                        {
                            res->parametersList.push_back(*parIt);
                        }
                        *shift = ls;
                        break;
                    }
                    else if (indent == 0)
                            break;
                }
            }
        }
        return res;
    }
}
