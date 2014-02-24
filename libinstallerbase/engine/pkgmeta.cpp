#include "pkgmeta.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cctype>

ostream& operator<<(ostream& os, const Token& t)
{
    return os << t.val;
}

istream& operator>>(istream& is, Token& t)
{
    static string sep = "=()";

    string val;
    bool start = true;
    int c;
    while (is) {
        c = is.get();
        if (isspace(c)) {
            if (start) continue;
            else {
                t.val = val;
                break;
            }
        }

        if (c == '#') {
            while ((c = is.get()) != '\n');
            continue;

        } else if (::find(sep.begin(), sep.end(), c) != sep.end()) {
            if (!val.empty()) {
                is.unget();
            } else 
                val = c;

            t.val = val;
            break;
        }

        val += c;
        start = false;
    }

    return is;
}

template<typename T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    os << "[" << endl;
    ::for_each(v.begin(), v.end(), [&](const T& t) { os << "\t" << t << endl; });
    os << "]" << endl;
    return os;
}

PkgsMeta::PkgsMeta(const string& config)
{
    ::ifstream is(config);
    if (!is) {
        cerr << config << " can not be opened";
        return;
    }

    _tokens = {::istream_iterator<Token>{is}, ::istream_iterator<Token>{}};
    cerr << _tokens;
    parse();
}

vector<string> PkgsMeta::operator()(const string& groupName)
{
    if (_meta.find(groupName) == _meta.end()) return {};
    return _meta[groupName];
}

void PkgsMeta::parse()
{
    while (!eot())
        parseGroup();

}

void PkgsMeta::parseGroup()
{
    string grp = next();
    cerr << "parsing group " << grp << endl;

    _meta[grp] = {};
    next(); // eat =
    next(); // eat (
    while (peek() != ")") {
        _meta[grp].push_back(next());
    }
    next(); // eat )
}

bool PkgsMeta::eot() const
{
    return _cursor == _tokens.size();
}

string PkgsMeta::next() const
{
    return _tokens[_cursor++].val;
}

string PkgsMeta::peek() const
{
    return _tokens[_cursor].val;
}
