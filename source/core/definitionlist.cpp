#include "definitionlist.h"
#include "idl.h"
#include "interface.h"
#include "dictionary.h"

namespace NewtooWebInterfaceMapper_core
{
    DefinitionList::DefinitionList(IDL* aIdl)
        :mIdl(aIdl)
    {}

    DefinitionList::~DefinitionList()
    {
        for(unsigned i = 0; i < mList.size(); i++)
            delete mList[i];
    }

    void DefinitionList::newDefinition(std::string text)
    {
        switch(Definition::defineType(text))
        {
            case INTERFACE:
            {
                mList.push_back(new Interface(mIdl, text));
                break;
            }
            case DICTIONARY:
            {
                mList.push_back(new Dictionary(mIdl, text));
                break;
            }
            case ENUMERATION:
            {
                break;
            }
            case TYPEDEFINE:
            {
                break;
            }
            case IMPLEMENTS:
            {
                break;
            }
            case INCLUDES:
            {
                break;
            }
            case UNKNOWN_TYPE:
            {
                break;
            }
        }
    }

    void DefinitionList::serialize()
    {
        for(unsigned i = 0; i < mList.size(); i++)
            idl()->header() += mList[i]->serializeHeader();

        for(unsigned i = 0; i < mList.size(); i++)
            idl()->source() += mList[i]->serializeSource();

        for(unsigned i = 0; i < mList.size(); i++)
            idl()->aboveSource() += mList[i]->serializeAboveSource();
    }

    void DefinitionList::cascade()
    {
        for(unsigned i = 0; i < mList.size(); i++)
            mList[i]->cascade();
    }

    Interface* DefinitionList::findInterface(std::string name)
    {
        for(unsigned i = 0; i < mList.size(); i++)
        {
            if(mList[i]->type() != INTERFACE)
                continue;

            Interface* in = (Interface*)mList[i];

            if(in->interfaceName() == name and !in->isPartial())
                return in;
        }
        return 0;
    }

    Dictionary* DefinitionList::findDictionary(std::string name)
    {
        for(unsigned i = 0; i < mList.size(); i++)
        {
            if(mList[i]->type() != DICTIONARY)
                continue;

            Dictionary* dict = (Dictionary*)mList[i];

            if(dict->dictionaryName() == name and !dict->isPartial()
                    and !dict->convertedText().empty())
                return dict;
        }
        return 0;
    }

    IDL* DefinitionList::idl() const
    {
        return mIdl;
    }

}
