/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-05-18 17:31:00
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-08-19 09:46:22
 * @FilePath: /Json/main.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>


#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>

#include "json.h"

#include "tests.h"

using namespace std;

struct Spending {
    string category;
    int amount;
};

int CalculateTotalSpendings(const vector<Spending>& spendings) {
    return accumulate(
        spendings.begin(), spendings.end(), 0, [](int current, const Spending& spending){
            return current + spending.amount;
        });
}

string FindMostExpensiveCategory(const vector<Spending>& spendings) {
    assert(!spendings.empty());
    auto compare_by_amount = [](const Spending& lhs, const Spending& rhs) {
        return lhs.amount < rhs.amount;
    };
    return max_element(begin(spendings), end(spendings), compare_by_amount)->category;
}

vector<Spending> LoadFromJson(istream& input) {
    vector<Spending> ret_val;
    
    json::Document in_json = json::Load(input);
    ret_val.reserve(in_json.GetRoot().AsArray().size());
    
    for(const json::Node& node : in_json.GetRoot().AsArray()){
        ret_val.push_back({node.AsMap().at("category"s).AsString(), node.AsMap().at("amount"s).AsInt()});
    }
    return ret_val;
}


int main() {
    TestNull();
    TestNumbers();
    TestStrings();
    TestBool();
    TestArray();
    TestMap();
    TestErrorHandling();
    Benchmark();

    const vector<Spending> spendings = LoadFromJson(cin);
    cout << "Total "sv << CalculateTotalSpendings(spendings) << '\n';
    cout << "Most expensive is "sv << FindMostExpensiveCategory(spendings) << '\n';
}