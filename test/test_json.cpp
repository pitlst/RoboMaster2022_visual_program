#include "json.hpp"
#include <iostream>
#include <string>

using namespace swq;

int main() {
	json temp_0;
	if (temp_0.get_typename() != "null"){return 1;}
	if (temp_0.get_type() != json::json_null){return 1;}
	if (temp_0.str() != "null"){return 1;}
	if (!temp_0.isNull()){return 1;}
	if (!temp_0.empty()){return 1;}
	if (temp_0 != json()){return 1;}
	temp_0 = 1;
	if (temp_0.get_typename() != "int"){return 1;}
	if (temp_0.get_type() != json::json_int){return 1;}
	if (temp_0.str() != "1"){return 1;}
	if (!temp_0.isInt()){return 1;}
	if (temp_0.empty()){return 1;}
	if (temp_0 != json(1)){return 1;}
	temp_0 = 1.23;
	if (temp_0.get_typename() != "double"){return 1;}
	if (temp_0.get_type() != json::json_double){return 1;}
	if (temp_0.str() != "1.23"){return 1;}
	if (!temp_0.isDouble()){return 1;}
	if (temp_0.empty()){return 1;}
	if (temp_0 != json(1.23)){return 1;}
	temp_0 = "abcdef";
	if (temp_0.get_typename() != "string"){return 1;}
	if (temp_0.get_type() != json::json_string){return 1;}
	if (temp_0.str() != std::string("\"abcdef\"")){return 1;}
	if (!temp_0.isString()){return 1;}
	if (temp_0.empty()){return 1;}
	if (temp_0 != json("abcdef")){return 1;}
    json arr;
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = "123";
    arr[3] = 3;
	if (arr.get_typename() != "array"){return 1;}
	if (arr.get_type() != json::json_array){return 1;}
	if (!arr.isArray()){return 1;}
	if (arr.empty()){return 1;}
	auto temp_1 = arr[0];
	if (temp_1.get_typename() != "int"){return 1;}
	if (temp_1.get_type() != json::json_int){return 1;}
	if (temp_1.str() != "1"){return 1;}
	if (!temp_1.isInt()){return 1;}
	if (temp_1.empty()){return 1;}
	if (temp_1 != json(1)){return 1;}
	if (arr[0].get_typename() != "int"){return 1;}
	if (arr[0].get_type() != json::json_int){return 1;}
	if (arr[0].str() != "1"){return 1;}
	if (!arr[0].isInt()){return 1;}
	if (arr[0].empty()){return 1;}
	if (arr[0] != json(1)){return 1;}
    json obj;
    obj["a"] = 5;
    obj["arr"] = arr;
	if (obj.get_typename() != "object"){return 1;}
	if (obj.get_type() != json::json_object){return 1;}
	if (!obj.isObject()){return 1;}
	if (obj.empty()){return 1;}
	auto temp_2 = obj["a"];
	if (temp_2.get_typename() != "int"){return 1;}
	if (temp_2.get_type() != json::json_int){return 1;}
	if (temp_2.str() != "5"){return 1;}
	if (!temp_2.isInt()){return 1;}
	if (temp_2.empty()){return 1;}
	if (temp_2 != json(5)){return 1;}
	if (obj["a"].get_typename() != "int"){return 1;}
	if (obj["a"].get_type() != json::json_int){return 1;}
	if (obj["a"].str() != "5"){return 1;}
	if (!obj["a"].isInt()){return 1;}
	if (obj["a"].empty()){return 1;}
	if (obj["a"] != json(5)){return 1;}
	if (obj["arr"].get_typename() != "array"){return 1;}
	if (obj["arr"].get_type() != json::json_array){return 1;}
	if (!obj["arr"].isArray()){return 1;}
	if (obj["arr"].empty()){return 1;}
	auto temp_3 = obj["arr"][0];
	if (temp_3.get_typename() != "int"){return 1;}
	if (temp_3.get_type() != json::json_int){return 1;}
	if (temp_3.str() != "1"){return 1;}
	if (!temp_3.isInt()){return 1;}
	if (temp_3.empty()){return 1;}
	if (temp_3 != json(1)){return 1;}
	if (obj["arr"][0].get_typename() != "int"){return 1;}
	if (obj["arr"][0].get_type() != json::json_int){return 1;}
	if (obj["arr"][0].str() != "1"){return 1;}
	if (!obj["arr"][0].isInt()){return 1;}
	if (obj["arr"][0].empty()){return 1;}
	if (obj["arr"][0] != json(1)){return 1;}
	arr.clear();
    obj.clear();
	temp_2.clear();
	temp_3.clear();
	return 0;
}