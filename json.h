/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-05-18 17:30:50
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-05-18 17:34:42
 * @FilePath: /Json/json.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <istream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class Node {
public:
    explicit Node(std::vector<Node> array);
    explicit Node(std::map<std::string, Node> map);
    explicit Node(int value);
    explicit Node(std::string value);

    const std::vector<Node>& AsArray() const;
    const std::map<std::string, Node>& AsMap() const;
    int AsInt() const;
    const std::string& AsString() const;

private:
    std::vector<Node> as_array_;
    std::map<std::string, Node> as_map_;
    int as_int_;
    std::string as_string_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

private:
    Node root_;
};

Document Load(std::istream& input);