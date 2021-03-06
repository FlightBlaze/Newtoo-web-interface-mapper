#include "interfaceunit.h"
#include "interface.h"
#include "function.h"
#include "idl.h"
#include "iostream"

namespace NewtooWebInterfaceMapper_core
{
    const char whitespace = ' ';

    std::string removeSpaces(const std::string raw)
    {
        std::string str = raw;

        while(str[0] == whitespace)
            str.erase(0, 1);
        while(str[str.size() - 1] == whitespace)
            str.erase(str.size() - 1, 1);
        return str;
    }

    InterfaceUnit::InterfaceUnit(std::string decl, IDL* idl) : mExtattrs(0), mIsReadonlyOrStatic(false),
        mUnitType(BAD_UNIT_TYPE)
    {
        IDL::removeSpaces(decl);

        std::size_t openExtattrBracket = decl.find("[");
        std::size_t closeExtattrBracket = decl.find("]");
        if(openExtattrBracket == 0 and closeExtattrBracket != std::string::npos)
        {
            mExtattrs = new ExtAttrMap(decl.substr(openExtattrBracket + 1,
                                                    closeExtattrBracket - openExtattrBracket - 1));

            decl.erase(openExtattrBracket, closeExtattrBracket - openExtattrBracket + 1);
            while(decl[0] == ' ')
                decl.erase(0, 1);
        }
        else mExtattrs = new ExtAttrMap("");

        std::size_t openBracetIndex = decl.find_first_of('(', 1);
        std::size_t closeBracetIndex = decl.find_last_of(')');

        std::size_t afterIndex = decl.substr(0, openBracetIndex).find_last_of(' ');

        if(afterIndex == std::string::npos)
        {
            idl->error(idl->invalidSyntaxError(decl));
            return;
        }

        if(openBracetIndex != std::string::npos and closeBracetIndex != std::string::npos)
        {
            std::string identiferPrefix;
            bool reindex = false;

            if(decl.find("static ") == 0)
            {
                mIsReadonlyOrStatic = true;
                decl.erase(0, 7);
                reindex = true;
            }

            if(decl.find("getter ") == 0)
            {
                identiferPrefix = "get";
                decl.erase(0, 7);
                reindex = true;
            }
            else if(decl.find("setter ") == 0)
            {
                identiferPrefix = "set";
                decl.erase(0, 7);
                reindex = true;
            }
            else if(decl.find("deleter ") == 0)
            {
                identiferPrefix = "remove";
                decl.erase(0, 8);
                reindex = true;
            }
            else if(decl.find("legacycaller ") == 0)
            {
                decl.erase(0, 13);
                reindex = true;
            }

            if(reindex)
            {
                openBracetIndex = decl.find_last_of('(');
                closeBracetIndex = decl.find_last_of(')');
                afterIndex = decl.find_last_of(' ');
            }

            mUnitType = FUNCTION_TYPE;
            std::string before = decl.substr(0, openBracetIndex);
            afterIndex = before.find_last_of(' ');

            if(afterIndex == std::string::npos)
            {
                idl->error(idl->invalidSyntaxError(decl));
                return;
            }

            mType = Function::toC_StylePlainType(decl.substr(0, afterIndex), idl);
            mIdentifer = decl.substr(afterIndex + 1, openBracetIndex - afterIndex - 1);
            mArgs = Function::convertArguments(decl.substr(openBracetIndex + 1, closeBracetIndex
                                                           - openBracetIndex - 1), idl);

            IDL::removeSpaces(mIdentifer);

            // Установить префикс
            if(!identiferPrefix.empty())
            {
                if(!mIdentifer.empty())
                    mIdentifer[0] = toupper(mIdentifer[0]);
                mIdentifer = identiferPrefix + mIdentifer;
            }

            // Устарелый параметр
            if(decl.find("raises(") != std::string::npos)
            {
                if(mType != "void")
                    mType = idl->settings().getExceptionTemplateClass() + '<' + mType + '>';
                else
                    mType = idl->settings().getExceptionOrVoid();
            }
        }
        else
        {
            if(decl.find("const ") == 0)
            {
                mUnitType = CONSTANT_TYPE;
                decl.erase(0, 6);

                std::size_t equalsSign = decl.find('=');

                if(equalsSign == std::string::npos)
                {
                    idl->error("Missing equals sign"+idl->atLineSuffix(decl));
                    return;
                }

                std::string beforeEquals = removeSpaces(decl.substr(0, equalsSign));
                std::string afterEquals = removeSpaces(decl.substr(equalsSign + 1, decl.size()
                                                                   - equalsSign - 1));

                std::size_t typeEndIndex = beforeEquals.find_last_of(whitespace);

                mType = Function::toC_StyleType(decl.substr(0, typeEndIndex), idl);
                mIdentifer = decl.substr(typeEndIndex + 1, beforeEquals.size() - typeEndIndex - 1);
                mArgs = afterEquals;
            }
            else
            {
                mUnitType = ATTRIBUTE_TYPE;

                if(decl.find("readonly ") == 0)
                {
                    mIsReadonlyOrStatic = true;
                    decl.erase(0, 9);
                }
                if(decl.find("attribute ") == 0)
                {
                    decl.erase(0, 10);
                }

                afterIndex = decl.find_last_of(' ');

                mType = Function::toC_StyleType(decl.substr(0, afterIndex), idl);
                mIdentifer = decl.substr(afterIndex + 1, decl.size() - afterIndex - 1);
            }
        }
        //std::cout << mType << " " << mIdentifer << '(' << mArgs << ')'  << std::endl;
    }

    InterfaceUnit::~InterfaceUnit()
    {
        delete mExtattrs;
    }

    ExtAttrMap& InterfaceUnit::extattrs()
    {
        return *mExtattrs;
    }

    std::string& InterfaceUnit::type()
    {
        return mType;
    }
    std::string& InterfaceUnit::identifer()
    {
        return mIdentifer;
    }
    std::string& InterfaceUnit::args()
    {
        return mArgs;
    }

    bool InterfaceUnit::isReadonlyOrStatic() const
    {
        return mIsReadonlyOrStatic;
    }

    InterfaceUnitType InterfaceUnit::unitType() const
    {
        return mUnitType;
    }

    void splitUnitString(std::vector<std::string>& list, std::string& target)
    {
        std::size_t splitterIndex = 0;

        bool found = false;
        for(unsigned i = 0; i < target.size(); i++)
        {
            if(target[i] != ';')
                continue;

            splitterIndex = i;
            found = true;
            break;
        }

        if(!found)
            return;

        std::string next = target.substr(splitterIndex + 1, target.size() - splitterIndex - 1);
        target = target.substr(0, splitterIndex);
        list.push_back(next);
        splitUnitString(list, list.back());
    }

    void InterfaceInner::parseInner(std::string str, Interface *assignedInterface)
    {
        std::vector<std::string> list;
        list.push_back(str);
        splitUnitString(list, list.back());

        // удалить пустую строку
        list.pop_back();

        for(unsigned i = 0; i < list.size(); i++)
        {
            assignedInterface->appendUnit(InterfaceUnit(list[i], assignedInterface->idl()));
        }
    }
}
