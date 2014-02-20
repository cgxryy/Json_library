/*
 * =====================================================================================
 *
 *       Filename:  cgxr_json.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月29日 18时17分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  changgongxiaorong, cgxryy@gmail.com
 *        Company:  Class 1203 of Network Engineering
 *
 * =====================================================================================
 */

#ifndef cgxr_json__H
#define cgxr_json__H

#ifdef __cplusplus
extern "C"
{
#endif

//The description of the json_value node type

#define	JSON_FALSE 	0
#define	JSON_TRUE 	1
#define	JSON_NULL 	2
#define	JSON_NUMBER 	3
#define	JSON_STRING 	4
#define	JSON_ARRAY 	5
#define	JSON_OBJECT 	6


typedef struct cgxr_json
{
	//it is a chain,it allow you to jump to the next or the previous node
	struct cgxr_json *next, *prev;
	//the object 's item
	struct cgxr_json *child;
	//the type of the item 
	int type;
	//the item's string if type==JSON_STRING 
	char *valuestring;
	//the item's number if type==JSON_NUMBER
	int valueint;
	double valuedouble;
	//the item's name string, if this item is the child of an object
	char *string;
}cjson;

/*function for user*/
cjson*	json_Parse(const char *text);
/*
const char* json_ParseValue(cjson* item, const char *text);
const char* json_ParseObject(cjson* item, const char *text);
const char* json_ParseArray(cjson* item, const char *text);
const char* json_ParseNumber(cjson* item, const char *text);
const char* json_ParseString(cjson* item, const char *text);
*/

char*  	json_Print(cjson* item);
char* 	json_PrintUnformat(cjson* item);
void 	json_Delete(cjson* item);

/*function for cjson*/

cjson* json_CreateNull();
cjson* json_CreateTrue();
cjson* json_CreateFalse();
cjson* json_CreateString(char* text);
cjson* json_CreateNumber(double num);
cjson* json_CreateObject(char* string);
cjson* json_CreateArray(int num);
cjson* json_CreateIntArray(const int* number, int count);
cjson* json_CreateDoubleArray(const double* number, int count);
cjson* json_CreateFloatArray(const float* number, int count);
void json_AddItemToObject(cjson* object, char* item);
void json_AddItemToArray(cjson* object, cjson* item);
cjson* json_GetItemFromArray(cjson* array, int where);
void json_ReplaceItemFromArray(cjson* array, int where, cjson* item);

void json_DeleteItemFromArray(cjson* array, int where);
/*function to check wrong place*/

#ifdef __cpluslplus
}
#endif 

#endif

