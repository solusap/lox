#include "Scanner.h"
#include "Token.h"

Scanner::Scanner(const string& source1, const std::function<void(int, const string&)>& loxerr)
    : source(source1), start(1), current(0), line(1), loxerror(loxerr)
{
    keywords.insert({"and", AND});
    keywords.insert({"class", CLASS});
    keywords.insert({"else", ELSE});
    keywords.insert({"false", FALSE});
    keywords.insert({"for", FOR});
    keywords.insert({"fun", FUN});
    keywords.insert({"if", IF});
    keywords.insert({"nil", NIL});
    keywords.insert({"or", OR});
    keywords.insert({"print", PRINT});
    keywords.insert({"return", RETURN});
    keywords.insert({"super", SUPER});
    keywords.insert({"this", THIS});
    keywords.insert({"true", TRUE});
    keywords.insert({"var", VAR});
    keywords.insert({"while", WHILE}); 
}

char Scanner::advance()
{
    return source[current++];
}

void Scanner::addToken(TokenType type)
{
    addToken(type, std::any());
}

void Scanner::addToken(TokenType type, const std::any& literal)
{
    string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line));
}

bool Scanner::isAtEnd()
{
    return current >= source.length();
}

bool Scanner::match(char expected)
{
    if (isAtEnd()) {
        return false;
    }
    if (source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
}

char Scanner::peek()
{
    if (isAtEnd()) {
        return '\0';
    }
    return source[current];
}

char Scanner::peekNext()
{
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source[current + 1];
}

void Scanner::getstring()
{
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }
    if (isAtEnd()) {
        loxerror(line, "Unterminated string.");
        return;
    }
    advance();
    string val = source.substr(start + 1, current - start - 2);
    addToken(STRING, std::any(val));
}

bool Scanner::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c)
{
    return (c >='a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

bool Scanner::isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}

void Scanner::getNumber()
{
    while (isDigit(peek())) {
        advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) {
            advance();
        }
    }
    addToken(NUMBER, std::any(std::stod(source.substr(start, current - start))));
}

void Scanner::identifier()
{
    while (isAlphaNumeric(peek())) {
        advance();
    }
    string text = source.substr(start, current - start);
    TokenType type =  keywords.count(text) != 0 ? keywords.at(text) : IDENTIFIER;
    addToken(type);
}

void Scanner::scanToken()
{
    char c = advance();
    switch (c) {
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ',': addToken(COMMA); break;
        case '.': addToken(DOT); break;
        case '-': addToken(MINUS); break;
        case '+': addToken(PLUS); break;
        case ';': addToken(SEMICOLON); break;
        case '*': addToken(STAR); break;
        case '!': addToken(match('=') ? BANG_EQUAL : BANG); break;
        case '=': addToken(match('=') ? EQUAL_EQUAL : EQUAL); break;
        case '<': addToken(match('=') ? LESS_EQUAL : LESS); break;
        case '>': addToken(match('=') ? GREATER_EQUAL : GREATER); break;
        case '/':
            if (match('/')) {
                // skip comment until the end of line
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else {
                addToken(SLASH);
            }
            break;
        case ' ':
        case '\t':
        case '\r':
            // fall throug to skip the whitespace
            break;
        case '\n':
            line++;
            break;
        case '"':
            getstring();
            break;
        default:
            if (isDigit(c)) {
                getNumber();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                loxerror(line, "Unexpected character!");
            }
            break;
    }
}

vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(TEOF, "", std::any(), line));
    return tokens;
}