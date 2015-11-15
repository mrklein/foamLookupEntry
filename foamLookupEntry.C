/*---------------------------------------------------------------------------*\
Application
    foamLookupEntry

Description
    Look up entry with a given key in a given dictionary.

Usage
    - foamLookupEntry -dict [DICTIONARY] -key [KEY] -batch

    \param -dict \n
    Specify dictionary file, otherwise it is read from Sin

    \param -key \n
    Key to look up. Use dot-syntax to access sub-dictionaries. I.e. a.b.c.d
    looks up d keyword in c sub-dictionary of b sub-dictionary of a in the file
    specified by dict parameter.

    \param -batch \n
    Fail silently printing empty string and positive return value.
\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "IOstreams.H"
#include "IFstream.H"
#include "dictionary.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

static void writeEntry(const entry&);
static int lookupPlainKeyword(const dictionary&, const string&, bool);
static int lookupDottedKeyword(const dictionary&, const string&, bool);

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Read the specified dictionary file and look up a key value."
    );

    argList::addOption("dict", "dictionary",
                       "Dictionary file where to look for the key.");
    argList::addOption("key", "value", "Key to look up.");
    argList::addBoolOption("batch", "Fail silently with exist status > 0.");
    argList::removeOption("case");
    argList::removeOption("noFunctionObjects");

    argList::noBanner();
    argList::noParallel();

    argList args(argc, argv);

    bool batch = args.optionFound("batch");

    if (!args.optionFound("key"))
    {
        if (!batch)
        {
            args.printUsage();
        }
        return 1;
    }

    dictionary dict;
    if (args.optionFound("dict"))
    {
        if (!exists(args.option("dict"), false))
        {
            if (!batch)
            {
                Serr<< "File: " << args.option("dict")
                    << " is not found." << endl;
            }
            Sout << endl;
            return 2;
        }

        IFstream Fin(args.option("dict"));
        dict.name() = args.option("dict");
        dict.read(static_cast<Istream&>(Fin));
    }
    else
    {
        dict.read(Sin);
    }

    string key = args.option("key");

    // The key is plain keyword, so just use lookup
    if (key.count('.') == 0)
    {
        return lookupPlainKeyword(dict, key, batch);
    }
    else
    {
        return lookupDottedKeyword(dict, key, batch);
    }
}


int lookupPlainKeyword(const dictionary& dict, const string& key, bool batch)
{
    if (!dict.found(key))
    {
        if (!batch)
        {
            Serr<< "Key " << key << " was not found";
            if (dict.name() != "")
            {
                Serr<< " in " << dict.name();
            }
            Serr<< "." << Foam::endl;
        }
        return 1 << 2;
    }

    writeEntry(dict.lookupEntry(key, true, true));
    return 0;
}


int lookupDottedKeyword(const dictionary& dict, const string& key, bool batch)
{
    string local_key(key);
    string tkey;
    const dictionary *dict_ptr = &dict;

    while(local_key.count('.') > 0)
    {
        size_t idx = local_key.find(".");
        tkey = local_key(idx);
        local_key = local_key(idx + 1, local_key.size() - idx - 1);

        if (dict_ptr->isDict(tkey))
        {
            dict_ptr = dict_ptr->subDictPtr(tkey);
        }
        else
        {
            if (!batch)
            {
                Serr<< tkey << " sub-dictionary was not found." << endl;
            }
            return 1 << 3;
        }
    }

    if (!dict_ptr->found(local_key))
    {
        if (!batch)
        {
            Serr<< "Key " << local_key << " was not found in "
                << dict_ptr->name() << " dictionary." << endl;
        }
        return 1 << 2;
    }

    writeEntry(dict_ptr->lookupEntry(local_key, true, true));
    return 0;
}


void writeEntry(const entry& ent)
{
    if (ent.isStream())
    {
        token t;
        ent.stream().read(t);
        Sout<< t << endl;
    }
    else if (ent.isDict())
    {
        Sout<< ent.dict() << endl;
    }
    else
    {
        Sout<< ent << endl;
    }
}

// ************************************************************************* //
