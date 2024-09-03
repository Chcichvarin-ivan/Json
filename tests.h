/*
 * @Author: Ivan Chichvarin ichichvarin@humanplus.ru
 * @Date: 2024-07-29 23:45:01
 * @LastEditors: Ivan Chichvarin ichichvarin@humanplus.ru
 * @LastEditTime: 2024-08-04 19:59:00
 * @FilePath: /Json/tests.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "json.h"
json::Document LoadJSON(const std::string& s);
std::string Print(const json::Node& node);
void MustFailToLoad(const std::string& s);
void TestNull();
void TestNumbers();
void TestStrings();
void TestBool();
void TestArray();
void TestMap();
void TestErrorHandling();
void Benchmark();