#include "json.h"

using namespace std;

json::Node::Node(bool value)
    : value_(value){
}

json::Node::Node(Array array)
    : value_(move(array)) {
}

json::Node::Node(Dict map)
    : value_(move(map)) {
}

json::Node::Node(int value)
    : value_(value) {
}

json::Node::Node(double value)
    : value_(value){
}

json::Node::Node(string value)
    : value_(move(value)) {
}

json::Node::Node(std::nullptr_t)
    : Node(){

}

bool json::Node::IsNull() const{
    return holds_alternative<std::nullptr_t>(value_);
}

bool json::Node::IsInt() const{
    return holds_alternative<int>(value_);
}

bool json::Node::IsDouble() const{
    return holds_alternative<double>(value_)||holds_alternative<int>(value_);;
}

bool json::Node::IsPureDouble() const{
    return holds_alternative<double>(value_);
}

bool json::Node::IsBool() const{
    return holds_alternative<bool>(value_);
}

bool json::Node::IsString() const{
    return holds_alternative<std::string>(value_);
}

bool json::Node::IsArray() const{
    return holds_alternative<Array>(value_);
}

bool json::Node::IsMap() const{
    return holds_alternative<Dict>(value_);
}

bool json::Node::AsBool() const{
    if(!IsBool()){
        throw std::logic_error("value is not a map"s);
    }else{
        return get<bool>(value_);
    }
} 

const json::Array& json::Node::AsArray() const {
    if(!IsArray()){
        throw std::logic_error("value is not an array"s);
    }else{
        return get<Array>(value_);
    }
}

const json::Dict& json::Node::AsMap() const {
    if(!IsMap()){
        throw std::logic_error("value is not a map"s);
    }else{
        return get<Dict>(value_);
    }
}

int json::Node::AsInt() const {
    if(!IsInt()){
        throw std::logic_error("value is not an integer"s);
    }else{
        return get<int>(value_);
    }
}

double json::Node::AsDouble() const {
    if(!IsDouble()){
        throw std::logic_error("value is not a double"s);
    }else{
        return IsPureDouble() ? get<double>(value_) : AsInt();
    }
}

const string& json::Node::AsString() const {
    if(!IsString()){
        throw std::logic_error("value is not a string"s);
    }else{
        return get<string>(value_);
    }
}

json::Document::Document(json::Node root)
    : root_(move(root)) {
}

const json::Node& json::Document::GetRoot() const {
    return root_;
}

json::Node LoadNode(istream& input);

json::Node LoadArray(istream& input) {
    vector<json::Node> result;

    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }

    if (!input) {
            throw json::ParsingError("Failed to read number from stream"s);
    }
    return json::Node(move(result));
}

json::Node LoadInt(istream& input) {
    int result = 0;
    while (isdigit(input.peek())) {
        result *= 10;
        result += input.get() - '0';
    }
    return json::Node(result);
}

// Считывает содержимое строкового литерала JSON-документа
// Функцию следует использовать после считывания открывающего символа ":
json::Node LoadString(std::istream& input) {
    using namespace std::literals;
    
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw json::ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw json::ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw json::ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw json::ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return json::Node(move(s));
}

json::Node LoadDict(istream& input) {
    json::Dict result;

    for (char ch; input >> ch && ch != '}';) {
        
        if (ch == '"') {
            std::string key = LoadString(input).AsString();
 
            if (input >> ch && ch == ':') {
                
                if (result.find(key) != result.end()) {
                    throw json::ParsingError("duplicate key '"s + key + "' has been found");
                }
 
                result.emplace(std::move(key), LoadNode(input));
                
            } else {
                throw json::ParsingError("expected \":\". but '"s + ch + "' has been found"s);
            }
            
        } else if (ch != ',') {
            throw json::ParsingError("',' expected. but '"s + ch + "' has been0ppo8 found"s);
        }
    }
 
    if (!input) {
        throw json::ParsingError("Failed to read number from stream"s);
    }

    return json::Node(move(result));
}

std::string LoadChar(std::istream& input) {
    std::string str;
    while (std::isalpha(input.peek())) {
        str.push_back(static_cast<char>(input.get()));
    }
    return str;
}

json::Node LoadNull(std::istream& input) {
    if (auto literal = LoadChar(input); literal == "null"sv) {
        return json::Node(nullptr);
    } else {
        throw json::ParsingError("unable to parse '"s + literal + "' as null"s);
    }
}
 
json::Node LoadBool(std::istream& input) {
    const auto str = LoadChar(input);
 
    if (str == "true"sv) {
        return json::Node(true);
    } else if (str == "false"sv) {
        return json::Node(false);
    } else {
        throw json::ParsingError("unable to parse '"s + str + "' as bool"s);
    }
}


json::Node LoadNumber(std::istream& input) {
    using namespace std::literals;

    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw json::ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw json::ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return std::stoi(parsed_num);
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return std::stod(parsed_num);
    } catch (...) {
        throw json::ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

json::Node LoadNode(istream& input) {
    char c;
    if (!(input >> c)) {
        throw json::ParsingError(""s);
    } else {
        switch (c) {
            case '[':
                return LoadArray(input);
            case '{':
                return LoadDict(input);
            case '"':
                return LoadString(input);
            case 't':
                [[fallthrough]];
            case 'f':
                input.putback(c);
                return LoadBool(input);
            case 'n':
                input.putback(c);
                return LoadNull(input);
            default:
                input.putback(c);
                return LoadNumber(input);
        }
    }
}

json::Document json::Load(istream& input) {
    return json::Document{LoadNode(input)};
}

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};


template <typename Value>
void PrintValue(const Value& value,const PrintContext& ctx) {
    ctx.out << value;
}

void PrintValue(const std::string& value, const PrintContext& ctx){
    ctx.out << '"';
    string out;
    //process escape sequences
    for(const char letter : value){
        out.push_back(letter);
        switch (letter) {
            case '\r':
                ctx.out << "\\r"sv;
                break;
            case '\n':
                ctx.out << "\\n"sv;
                break;
            case '\t':
                ctx.out << "\\t"sv;
                break;
            case '"':
                // Символы " и \ выводятся как \" или \\, соответственно
                [[fallthrough]];
            case '\\':
                ctx.out << '\\';
                [[fallthrough]];
            default:
                ctx.out << letter;
                break;
        }
    }
    ctx.out << '"';
}

void PrintNode(const json::Node& node, const PrintContext& ctx);

void PrintValue(std::nullptr_t, const PrintContext& ctx) {
    ctx.out << "null"sv;
}

void PrintValue(bool value, const PrintContext& ctx) {
    ctx.out << std::boolalpha << value;
}

void PrintValue(json::Array nodes, const PrintContext& ctx){
    std::ostream& out = ctx.out;
    out << "[\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const json::Node& node : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put(']');
}

void PrintValue(json::Dict nodes, const PrintContext& ctx) {
    std::ostream& out = ctx.out;
    out << "{\n"sv;
    bool first = true;
    auto inner_ctx = ctx.Indented();
    for (const auto& [key, node] : nodes) {
        if (first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_ctx.PrintIndent();
        PrintNode(key, ctx);
        out << ": "sv;
        PrintNode(node, inner_ctx);
    }
    out.put('\n');
    ctx.PrintIndent();
    out.put('}');

}

void PrintNode(const json::Node& node, const PrintContext& ctx) {
    std::visit(
        [&ctx](const auto& value){ 
            PrintValue(value, ctx); 
        },
        node.GetValue());
} 

void json::Print(const json::Document& document, std::ostream& output) {
    PrintNode(document.GetRoot(), PrintContext{output});
}