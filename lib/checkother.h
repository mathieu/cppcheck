/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjamäki and Cppcheck team.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//---------------------------------------------------------------------------
#ifndef CheckOtherH
#define CheckOtherH
//---------------------------------------------------------------------------

#include "check.h"
#include "settings.h"

class Token;

/// @addtogroup Checks
/// @{


/** @brief Various small checks */

class CheckOther : public Check
{
public:
    /** @brief This constructor is used when registering the CheckClass */
    CheckOther() : Check(myName())
    { }

    /** @brief This constructor is used when running checks. */
    CheckOther(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
        : Check(myName(), tokenizer, settings, errorLogger)
    { }

    /** @brief Run checks against the normal token list */
    void runChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
    {
        CheckOther checkOther(tokenizer, settings, errorLogger);

        // Coding style checks
        checkOther.warningOldStylePointerCast();
        checkOther.checkUnsignedDivision();
        checkOther.checkCharVariable();
        checkOther.functionVariableUsage();
        checkOther.checkStructMemberUsage();
        checkOther.strPlusChar();
        checkOther.sizeofsizeof();
        checkOther.sizeofCalculation();
        checkOther.checkRedundantAssignmentInSwitch();
        checkOther.checkAssignmentInAssert();
        checkOther.checkSizeofForArrayParameter();
        checkOther.checkSizeofForNumericParameter();
        checkOther.checkSelfAssignment();
        checkOther.checkDuplicateIf();
        checkOther.checkDuplicateBranch();
        checkOther.checkDuplicateExpression();
        checkOther.checkDuplicateBreak();

        // information checks
        checkOther.checkVariableScope();

        checkOther.clarifyCondition();   // not simplified because ifAssign
    }

    /** @brief Run checks against the simplified token list */
    void runSimplifiedChecks(const Tokenizer *tokenizer, const Settings *settings, ErrorLogger *errorLogger)
    {
        CheckOther checkOther(tokenizer, settings, errorLogger);

        checkOther.clarifyCalculation();

        // Coding style checks
        checkOther.checkConstantFunctionParameter();
        checkOther.checkIncompleteStatement();

        checkOther.invalidFunctionUsage();
        checkOther.checkZeroDivision();
        checkOther.checkMathFunctions();
        checkOther.checkFflushOnInputStream();
        checkOther.invalidScanf();

        checkOther.checkIncorrectLogicOperator();
        checkOther.checkMisusedScopedObject();
        checkOther.checkCatchExceptionByValue();
        checkOther.checkMemsetZeroBytes();
        checkOther.checkIncorrectStringCompare();
        checkOther.checkIncrementBoolean();
        checkOther.checkComparisonOfBoolWithInt();
        checkOther.checkSwitchCaseFallThrough();
        checkOther.checkAlwaysTrueOrFalseStringCompare();
    }

    /** @brief Clarify calculation for ".. a * b ? .." */
    void clarifyCalculation();
    void clarifyCalculationError(const Token *tok, const std::string &op);

    /** @brief Suspicious condition (assignment+comparison) */
    void clarifyCondition();
    void clarifyConditionError(const Token *tok);

    /** @brief Are there C-style pointer casts in a c++ file? */
    void warningOldStylePointerCast();

    /**
     * @brief Invalid function usage (invalid radix / overlapping data)
     *
     * %Check that given function parameters are valid according to the standard
     * - wrong radix given for strtol/strtoul
     * - overlapping data when using sprintf/snprintf
     */
    void invalidFunctionUsage();

    /** @brief %Check for unsigned division */
    void checkUnsignedDivision();

    /** @brief %Check for unused function variables */
    void functionVariableUsage();
    void unusedVariableError(const Token *tok, const std::string &varname);
    void allocatedButUnusedVariableError(const Token *tok, const std::string &varname);
    void unreadVariableError(const Token *tok, const std::string &varname);
    void unassignedVariableError(const Token *tok, const std::string &varname);

    /** @brief %Check scope of variables */
    void checkVariableScope();

    /** @brief %Check for constant function parameter */
    void checkConstantFunctionParameter();

    /** @brief %Check that all struct members are used */
    void checkStructMemberUsage();

    /** @brief Using char variable as array index / as operand in bit operation */
    void checkCharVariable();

    /** @brief Incomplete statement. A statement that only contains a constant or variable */
    void checkIncompleteStatement();

    /** @brief str plus char (unusual pointer arithmetic) */
    void strPlusChar();

    /** @brief %Check zero division*/
    void checkZeroDivision();

    /** @brief %Check for parameters given to math function that do not make sense*/
    void checkMathFunctions();

    void lookupVar(const Token *tok1, const std::string &varname);

    /** @brief %Check for using fflush() on an input stream*/
    void checkFflushOnInputStream();

    /** @brief %Check for 'sizeof sizeof ..' */
    void sizeofsizeof();
    void sizeofsizeofError(const Token *tok);

    /** @brief %Check for calculations inside sizeof */
    void sizeofCalculation();
    void sizeofCalculationError(const Token *tok);

    /** @brief scanf can crash if width specifiers are not used */
    void invalidScanf();
    void invalidScanfError(const Token *tok);

    /** @brief %Check for assigning to the same variable twice in a switch statement*/
    void checkRedundantAssignmentInSwitch();

    /** @brief %Check for switch case fall through without comment */
    void checkSwitchCaseFallThrough();

    /** @brief %Check for assigning a variable to itself*/
    void checkSelfAssignment();

    /** @brief %Check for assignment to a variable in an assert test*/
    void checkAssignmentInAssert();

    /** @brief %Check for testing for mutual exclusion over ||*/
    void checkIncorrectLogicOperator();

    /** @brief %Check for objects that are destroyed immediately */
    void checkMisusedScopedObject();

    /** @brief %Check for exceptions that are caught by value instead of by reference */
    void checkCatchExceptionByValue();

    /** @brief %Check for filling zero bytes with memset() */
    void checkMemsetZeroBytes();

    /** @brief %Check for using sizeof with array given as function argument */
    void checkSizeofForArrayParameter();

    /** @brief %Check for using sizeof with numeric given as function argument */
    void checkSizeofForNumericParameter();

    /** @brief %Check for using bad usage of strncmp and substr */
    void checkIncorrectStringCompare();

    /** @brief %Check for using postfix increment on bool */
    void checkIncrementBoolean();

    /** @brief %Check for suspicious comparison of a bool and a non-zero (and non-one) value (e.g. "if (!x==4)") */
    void checkComparisonOfBoolWithInt();

    /** @brief %Check for suspicious code where multiple if have the same expression (e.g "if (a) { } else if (a) { }") */
    void checkDuplicateIf();

    /** @brief %Check for suspicious code where if and else branch are the same (e.g "if (a) b = true; else b = true;") */
    void checkDuplicateBranch();

    /** @brief %Check for suspicious code with the same expression on both sides of operator (e.g "if (a && a)") */
    void checkDuplicateExpression();

    /** @brief %Check for suspicious code that compares string literals for equality */
    void checkAlwaysTrueOrFalseStringCompare();

    /** @brief %Check for duplicate break statements in a switch or loop */
    void checkDuplicateBreak();

    /** @brief check if token is a record type without side effects */
    bool isRecordTypeWithoutSideEffects(const Token *tok);


    // Error messages..
    void cstyleCastError(const Token *tok);
    void dangerousUsageStrtolError(const Token *tok);
    void sprintfOverlappingDataError(const Token *tok, const std::string &varname);
    void udivError(const Token *tok);
    void unusedStructMemberError(const Token *tok, const std::string &structname, const std::string &varname);
    void passedByValueError(const Token *tok, const std::string &parname);
    void constStatementError(const Token *tok, const std::string &type);
    void charArrayIndexError(const Token *tok);
    void charBitOpError(const Token *tok);
    void variableScopeError(const Token *tok, const std::string &varname);
    void conditionAlwaysTrueFalse(const Token *tok, const std::string &truefalse);
    void strPlusChar(const Token *tok);
    void zerodivError(const Token *tok);
    void mathfunctionCallError(const Token *tok, const unsigned int numParam = 1);
    void fflushOnInputStreamError(const Token *tok, const std::string &varname);
    void redundantAssignmentInSwitchError(const Token *tok, const std::string &varname);
    void switchCaseFallThrough(const Token *tok);
    void selfAssignmentError(const Token *tok, const std::string &varname);
    void assignmentInAssertError(const Token *tok, const std::string &varname);
    void incorrectLogicOperatorError(const Token *tok, bool always);
    void misusedScopeObjectError(const Token *tok, const std::string &varname);
    void catchExceptionByValueError(const Token *tok);
    void memsetZeroBytesError(const Token *tok, const std::string &varname);
    void sizeofForArrayParameterError(const Token *tok);
    void sizeofForNumericParameterError(const Token *tok);
    void incorrectStringCompareError(const Token *tok, const std::string& func, const std::string &string, const std::string &len);
    void incrementBooleanError(const Token *tok);
    void comparisonOfBoolWithIntError(const Token *tok, const std::string &varname);
    void duplicateIfError(const Token *tok1, const Token *tok2);
    void duplicateBranchError(const Token *tok1, const Token *tok2);
    void duplicateExpressionError(const Token *tok1, const Token *tok2, const std::string &op);
    void alwaysTrueFalseStringCompare(const Token *tok, const std::string& str1, const std::string& str2);
    void duplicateBreakError(const Token *tok);

    void getErrorMessages(ErrorLogger *errorLogger, const Settings *settings)
    {
        CheckOther c(0, settings, errorLogger);

        // error
        c.sprintfOverlappingDataError(0, "varname");
        c.udivError(0);
        c.zerodivError(0);
        c.mathfunctionCallError(0);
        c.fflushOnInputStreamError(0, "stdin");
        c.misusedScopeObjectError(NULL, "varname");
        c.sizeofForArrayParameterError(0);
        c.sizeofForNumericParameterError(0);

        // style/warning
        c.cstyleCastError(0);
        c.dangerousUsageStrtolError(0);
        c.unusedStructMemberError(0, "structname", "variable");
        c.passedByValueError(0, "parametername");
        c.constStatementError(0, "type");
        c.charArrayIndexError(0);
        c.charBitOpError(0);
        c.variableScopeError(0, "varname");
        c.conditionAlwaysTrueFalse(0, "true/false");
        c.strPlusChar(0);
        c.sizeofsizeofError(0);
        c.sizeofCalculationError(0);
        c.redundantAssignmentInSwitchError(0, "varname");
        c.switchCaseFallThrough(0);
        c.selfAssignmentError(0, "varname");
        c.assignmentInAssertError(0, "varname");
        c.invalidScanfError(0);
        c.incorrectLogicOperatorError(0, true);
        c.unusedVariableError(0, "varname");
        c.allocatedButUnusedVariableError(0, "varname");
        c.unreadVariableError(0, "varname");
        c.unassignedVariableError(0, "varname");
        c.catchExceptionByValueError(0);
        c.memsetZeroBytesError(0, "varname");
        c.clarifyCalculationError(0, "+");
        c.clarifyConditionError(0);
        c.incorrectStringCompareError(0, "substr", "\"Hello World\"", "12");
        c.incrementBooleanError(0);
        c.comparisonOfBoolWithIntError(0, "varname");
        c.duplicateIfError(0, 0);
        c.duplicateBranchError(0, 0);
        c.duplicateExpressionError(0, 0, "&&");
        c.alwaysTrueFalseStringCompare(0, "str1", "str2");
        c.duplicateBreakError(0);
    }

    std::string myName() const
    {
        return "Other";
    }

    std::string classInfo() const
    {
        return "Other checks\n"

               // error
               "* [[OverlappingData|bad usage of the function 'sprintf' (overlapping data)]]\n"
               "* division with zero\n"
               "* using fflush() on an input stream\n"
               "* scoped object destroyed immediately after construction\n"
               "* assignment in an assert statement\n"
               "* sizeof for array given as function argument\n"
               "* sizeof for numeric given as function argument\n"
               "* incorrect length arguments for 'substr' and 'strncmp'\n"

               // style
               "* C-style pointer cast in cpp file\n"
               "* redundant if\n"
               "* bad usage of the function 'strtol'\n"
               "* [[CheckUnsignedDivision|unsigned division]]\n"
               "* Dangerous usage of 'scanf'\n"
               "* unused struct member\n"
               "* passing parameter by value\n"
               "* [[IncompleteStatement|Incomplete statement]]\n"
               "* [[charvar|check how signed char variables are used]]\n"
               "* variable scope can be limited\n"
               "* condition that is always true/false\n"
               "* unusal pointer arithmetic. For example: \"abc\" + 'd'\n"
               "* redundant assignment in a switch statement\n"
               "* look for 'sizeof sizeof ..'\n"
               "* look for calculations inside sizeof()\n"
               "* assignment of a variable to itself\n"
               "* mutual exclusion over || always evaluating to true\n"
               "* exception caught by value instead of by reference\n"
               "* Clarify calculation with parentheses\n"
               "* using increment on boolean\n"
               "* comparison of a boolean with a non-zero integer\n"
               "* suspicious condition (assignment+comparison)\n"
               "* suspicious condition (runtime comparison of string literals)\n"
               "* duplicate break statement\n"

               // optimisations
               "* optimisation: detect post increment/decrement\n";
    }

private:

    /**
     * @brief Used in warningRedundantCode()
     * Iterates through the %var% tokens in a fully qualified name and concatenates them.
     */
    std::string concatNames(const Token **tok) const
    {
        std::string varname;
        while (Token::Match(*tok, "%var% ::|."))
        {
            varname.append((*tok)->str());
            varname.append((*tok)->next()->str());
            *tok = (*tok)->tokAt(2);
        }

        if (Token::Match(*tok, "%var%"))
            varname.append((*tok)->str());

        return varname;
    }
};
/// @}
//---------------------------------------------------------------------------
#endif

