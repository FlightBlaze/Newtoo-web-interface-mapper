#pragma once

#include "definition.h"
#include "extattrmap.h"
#include "interfaceunit.h"

namespace NewtooWebInterfaceMapper_core
{
    class Interface : public Definition
    {
    public:
        Interface(IDL* aIdl, std::string decl);

        std::string& interfaceName();

        std::string& copyConstructorStart();
        std::string& copyConstructorInitFields();
        std::string& copyConstructorInitFieldsMembers();
        std::string& copyConstructorInitFieldsMembersAppendix();
        std::string& copyConstructorInitFieldsAppendix();
        std::string& copyConstructorEnd();
        std::string& headerStart();
        std::string& headerInherit();
        std::string& headerPublic();
        std::string& headerPublicPrefix();
        std::string& headerPublicAppendix();
        std::string& headerPrivate();
        std::string& headerPrivatePrefix();
        std::string& headerPrivateAppendix();
        std::string& headerEnd();
        std::string& source();

        std::string serializeHeader() override;
        /*
            mHeaderStart + mHeaderInherit + headerPublicPrefix() + mHeaderPublic + mHeaderPublicAppendix
            + mHeaderPrivate + mHeaderPrivateAppendix + mHeaderEnd
        */

        std::string serializeSource() override;
        /*
            mCopyConstructorStart + mCopyConstructorInitFields + mCopyConstructorInitFieldsAppendix
            + mCopyConstructorEnd + mSource
        */

        std::string serializeHeader_Interface();
        std::string serializeSource_Interface();

        void cascade() override;

        void append(std::string partialHeaderPublic, std::string partialHeaderPrivate,
                    std::string partialCopyConstructorInitFields,
                    std::string partialCopyConstructorInitFieldsMembers,
                    std::string partialSource);
        /*
            Нужен, чтобы объединять все частичные определения воедино через DefinitionList::cascade()
            Находин он нужное определение через DefinitionList::findDefinition(...);
        */

        void appendUnit(InterfaceUnit unit);
        /*
            Сделать этот класс хостом атрибута. Это означает, что для атрибута будет создано
            приватное поле, которое он будет возращать.
        */

        bool isPartial() const;
        bool isCallback() const;

        std::string includeDirective(); // ex. #include "Object.h"

        std::string serializeAboveSource() override;

    protected:
        void appendMemberToCopyConstructor(std::string member);

        /*
            For example:

            interface Object : AbstractObject
            {
                readonly attribute Object? parent;
            };
            partially inteface Object
            {
                void run();
            };
        */

        std::string mInterfaceName;

        std::string mCopyConstructorStart;              // ex. Object::Object(Object& ref)
        std::string mCopyConstructorInitFields;         // ex. : AbstractObject(ref)
        std::string mCopyConstructorInitFieldsMembers;  // ex. , mParent(0)
        std::string mCopyConstructorInitFieldsMembersAppendix;
        std::string mCopyConstructorInitFieldsAppendix;
        std::string mCopyConstructorEnd;                // ex. {}
        std::string mHeaderStart;                       // ex. class Object
        std::string mHeaderInherit;                     // ex. : public AbstractObject
        std::string mHeaderPublic;                      // ex. Object* parent() const;
        std::string mHeaderPublicPrefix;
        std::string mHeaderPublicAppendix;              // ex. void run();
        std::string mHeaderPrivate;                     // ex. Object* mParent;
        std::string mHeaderPrivatePrefix;
        std::string mHeaderPrivateAppendix;
        std::string mHeaderEnd;                         // ex. };
        std::string mSource;                            // ex. Object* Object::parent const
                                                        //     { return mParent;}
                                                        //     void Object::run()
                                                        //     {}

        bool mIsPartial;
        bool mIsCallback;
    };
}
