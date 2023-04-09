#include <cctype>
#include <vector>
#include <fmt/core.h>
#include <string>
#include <fstream>
#include <string_view>
#include <algorithm>

using std::string;
using std::vector;
using std::string_view;

namespace GenerateAst
{
    struct split
    {
        enum empties_t { empties_ok, no_empties };
    };

    template <typename Container>
    Container& split(
    Container&                            result,
    const typename Container::value_type& s,
    const typename Container::value_type& delimiters,
    split::empties_t                      empties = split::empties_ok ) {
        result.clear();
        size_t current;
        size_t next = -1;
        do {
            if (empties == split::no_empties) {
                next = s.find_first_not_of( delimiters, next + 1 );
                if (next == Container::value_type::npos) break;
                next -= 1;
            }
            current = next + 1;
            next = s.find(delimiters, current);
            result.push_back(s.substr( current, next - current ) );
        }
        while (next != Container::value_type::npos);
        return result;
    }

    vector<string> split(const string& s, const string& delimeters)
    {
        vector<string> ret;
        split(ret, s, delimeters);
        return ret;
    }

    string trim(const string& s) {
        string ret = s;
        if (ret.empty()) {
            return "";
        }
        ret.erase(0, ret.find_first_not_of(" "));
        ret.erase(ret.find_last_not_of(" ") + 1);
        return ret;
    }

    string tolowercase(const string& s) {
        string ret = s;
        for (auto&& c : ret) {
            c = std::tolower(c);
        }
        return ret;
    }

    void defineType(std::ofstream& fout, const string& baseName, const string& className, const string& fieldList) 
    {
        fout<<"struct " << className << " : public " << baseName<<"\n" <<"{\n";
        
        // constructor
        vector<string> fileds = split(fieldList, ", ");

        string ctor = className + "(";
        string ctor_initlist = "";
        for (auto && filed : fileds) {
            auto filePair = split(trim(filed), " ");
            string fType = trim(filePair[0]);
            string fName = trim(filePair[1]);

            if (fType.find(".") != string::npos) {
                fType.replace(fType.find("."), 1, "::");
            }
            
            ctor_initlist += fmt::format("{}({}),", fName, fName);
            if (fType == baseName) {
                ctor += fmt::format("shared_ptr<{}> {},", baseName, fName);
                fout<<fmt::format("    shared_ptr<{}> {};\n", baseName, fName);
            } else {
                ctor += fmt::format("{} {},", fType, fName);
                fout<<fmt::format("    {} {};\n", fType, fName);
            }
            
        }
        *ctor.rbegin() = ')';                   // replace last ',' to ')'
        *ctor_initlist.rbegin() = '\n';         // replace last ',' to '\n'
        ctor += "\n    : " + ctor_initlist + "    {}";


        fout << "    " << ctor << "\n";

        // visitor pattern
        fout << "\n";
        fout << "    any accept(Visitor& visitor) override {" << "\n";
        fout << "        return visitor.visit" + className + baseName + "(*this);" << "\n";
        fout<<"    }" <<"\n"; 
        fout << "};" << "\n";     
    }

    void defineVisitor(std::ofstream& fout, const string& baseName, const vector<string>& types) 
    {
        fout << "struct Visitor\n";
        fout << "{\n";
        fout << "    Visitor() = default;\n";
        fout << "    ~Visitor() = default;\n";
        for (auto t : types) {
            string type_name = trim(split(t, ":")[0]);
            fout <<fmt::format("    virtual any visit{}{}({}& {}) = 0;\n", 
                            type_name, baseName, type_name, tolowercase(baseName));
        }
        fout << "};" << "\n";
    }

    void defineAst(const string& outputdir, const string& baseName, vector<string> types, const string& header = "") 
    {
        string path = outputdir + "/" + baseName + ".h";
        std::ofstream fout(path, std::ios_base::out);
        string tmp1 = R"(#include <vector>
#include <fmt/core.h>
#include <string>
#include <any>
#include <memory>

#include "Token.h"

using std::string;
using std::vector;
using std::shared_ptr;
)";


        fout<<"#ifndef"<<" __"<<baseName<<"__h__"<<'\n';
        fout<<"#define"<<" __"<<baseName<<"__h__"<<'\n';
        if (header != "") {
            fout<< header <<"\n";
        }
        fout<<tmp1<<"\n";

        fout<<"namespace "<<baseName <<"{"<<"\n";

        fout<<"using std::any;"<<"\n";

        for (auto && type : types) {
            string className = trim(split(type, ":")[0]);
            fout << "struct " << className << ";" << "\n";
        }

        defineVisitor(fout, baseName, types);

        // define base class
        fout<<"struct "<<baseName<<"  \n{\n";
        fout<<"    "<<baseName<<"() = default;\n";
        fout<<"    ~"<<baseName<<"() = default;\n";
        fout<<"    virtual any accept(Visitor& visitor) = 0;" <<"\n";
        fout <<"};" << "\n";

        fout<<"\n";
        // define derived classes
        for (auto && type : types) {
            string className = trim(split(type, ":")[0]);
            string fileds = trim(split(type, ":")[1]);
            defineType(fout, baseName, className, fileds);
        }
        fout<<"\n}\n";
        fout<<"#endif";
        fout.close();
    }
}


int main(int argc, char** argv)
{
    if (argc != 2) {
        fmt::print("Usage: generate_ast <output directory>");
        exit(64);
    }
    char* outputdir = argv[1];
    GenerateAst::defineAst(outputdir, "Expr",  {
        //> Statements and State assign-expr
        "Assign   : Token name, Expr value",
        //< Statements and State assign-expr
        "Binary   : Expr left, Token oper, Expr right",
        //> Functions call-expr
        "Call     : Expr callee, Token paren, vector<Expr*> arguments",
        //< Functions call-expr
        //> Classes get-ast
        "Get      : Expr object, Token name",
        //< Classes get-ast
        "Grouping : Expr expression",
        "Literal  : any value",
        //> Control Flow logical-ast
        "Logical  : Expr left, Token oper, Expr right",
        //< Control Flow logical-ast
        //> Classes set-ast
        "Set      : Expr object, Token name, Expr value",
        //< Classes set-ast
        //> Inheritance super-expr
        "Super    : Token keyword, Token method",
        //< Inheritance super-expr
        //> Classes this-ast
        "This     : Token keyword",
        //< Classes this-ast
        /* Representing Code call-define-ast < Statements and State var-expr
            "Unary    : Token oper, Expr right"
        */
        //> Statements and State var-expr
        "Unary    : Token oper, Expr right",
        "Variable : Token name"
    });

#if 0
    GenerateAst::defineAst(outputdir, "Stmt", {
        //> block-ast
        "Block      : vector<shared_ptr<Stmt>> statements",
        //< block-ast
        /* Classes class-ast < Inheritance superclass-ast
                "CLASS      : Token name, vector<Stmt.Function> methods",
        */
        //> Inheritance superclass-ast
                "CLASSD      : Token name, Expr.Variable superclass, vector<Function> methods",
        //< Inheritance superclass-ast
                "Expression : shared_ptr<Expr.Expr> expression",
        //> Functions function-ast
                "Function   : Token name, vector<Token> params, vector<shared_ptr<Stmt>> body",
        //< Functions function-ast
        //> Control Flow if-ast
                "If         : shared_ptr<Expr.Expr> condition, Stmt thenBranch, Stmt elseBranch",
        //< Control Flow if-ast
        /* Statements and State stmt-ast < Statements and State var-stmt-ast
                "Print      : Expr expression"
        */
        //> var-stmt-ast
                "Print      : shared_ptr<Expr.Expr> expression",
        //< var-stmt-ast
        //> Functions return-ast
                "Return     : Token keyword, shared_ptr<Expr.Expr> value",
        //< Functions return-ast
        /* Statements and State var-stmt-ast < Control Flow while-ast
                "Var        : Token name, Expr initializer"
        */
        //> Control Flow while-ast
                "Var        : Token name, shared_ptr<Expr.Expr> initializer",
                "While      : shared_ptr<Expr.Expr> condition, Stmt body"
        //< Control Flow while-ast
    }, "#include \"Expr.h\"");

#endif

    GenerateAst::defineAst(outputdir, "Stmt", {
        "Expression : shared_ptr<Expr.Expr> expression",
        "Print : shared_ptr<Expr.Expr> expression",
        "Var : Token name, shared_ptr<Expr.Expr> initexpr",
        "Unary : Token oper, shared_ptr<Expr.Expr> right",
        "Block : vector<shared_ptr<Stmt>> statments"
    }, "#include \"Expr.h\"");


}