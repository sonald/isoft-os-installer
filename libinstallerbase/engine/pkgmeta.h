#ifndef _pkgmeta_h
#define _pkgmeta_h 

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

struct Token {
    string val;
};

/** 
 * stores info about meta groups and pkg names in each groups 
 */
class PkgsMeta {
    public:
        PkgsMeta(const string& config);
        vector<string> operator()(const string& groupName);

    private:
        unordered_map<string, vector<string>> _meta;
        string _config;
        vector<Token> _tokens;
        mutable int _cursor{0};

        //helper
        void parse();
        void parseGroup();

        bool eot() const;
        string next() const;
        string peek() const;
};

#endif
