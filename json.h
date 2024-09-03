/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-05-18 17:30:50
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-08-10 17:08:04
 * @FilePath: /Json/json.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <istream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
namespace json{
    class Node;
    
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };
class Node {
public:
    using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
    Node() = default;
    Node(bool value);
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(double value);
    Node(std::nullptr_t);
    Node(std::string value);

    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool AsBool() const;
    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const;

    bool IsNull() const;
    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsMap() const;

    const Value& GetValue() const { return value_; }

private:
    Value value_;
};

class Document {
public:
    Document() = default;
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

inline bool operator==(const Node& lhs, const Node& rhs) { 
    return lhs.GetValue() == rhs.GetValue();
}  
inline bool operator!=(const Node& lhs, const Node& rhs) {
    return !(lhs == rhs);
} 


Document Load(std::istream& input);
void Print(const Document& doc, std::ostream& output);

inline bool operator==(const Document& lhs, const Document& rhs) {
    return lhs.GetRoot() == rhs.GetRoot();
}
inline bool operator!=(const Document& lhs, const Document& rhs) {
    return !(lhs == rhs);
}

}//namespace json