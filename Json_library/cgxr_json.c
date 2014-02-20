/*
 * =====================================================================================
 *
 *       Filename:  cgxr_json.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年02月08日 09时07分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  changgongxiaorong, cgxryy@gmail.com
 *        Company:  Class 1203 of Network Engineering
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include "cgxr_json.h"

static const char* json_Skip(const char *text)
{
	while ((unsigned char)*text <= 32)
	{
		text++;
	}
	return text;
}

cjson* json_New()
{
	cjson* node;
	node = (cjson*)malloc(sizeof(cjson));
	node->valuestring = NULL;
	node->string = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->child = NULL;

	return node;
}

void json_Delete(cjson* item)
{
	if (item == NULL)
	      return;

	json_Delete(item->child);
	json_Delete(item->next);
	if (item->string != NULL)
	      free(item->string);
	if (item->valuestring != NULL)
	      free(item->valuestring);
	if (item->child != NULL)
	      free(item->child);
	
	if (item->next != NULL)
	{
		free(item->next);
		item->next = NULL;
	}
}

static const char* json_ParseValue(cjson* item, const char* text);
static const char* json_ParseString(cjson* item, const char* text);
static const char* json_ParseObject(cjson* item, const char* text);
static const char* json_ParseArray(cjson* item, const char* text);
static const char* json_ParseNumber(cjson*item, const char* text);

cjson* json_Parse(const char *text)
{
	cjson* root;

	root = json_New();

	json_ParseValue(root, text);
	
	if (!root)
//	      json_Error(__LINE__);
	      perror("root==NULL");   

	return root;
}

static const char* json_ParseString(cjson* item, const char* text)
{
	const char* str = text;
	char* 	out;
	int 	len;

	if (*str == '\"')
	{ 
		while (*++str != '\"') 
		      ;
		len = str-text;
		out = (char*)malloc(len);
		str = out; 
		str = text+1;
		strncpy(out, str, len-1);
		*(out+len-1) = '\0';
		item->valuestring = out;
		item->type = JSON_STRING;
		text = text + len;
		
		return text+1;
	}
	else 
	      return NULL;
	
}
static const char* json_ParseObject(cjson* item, const char* text)
{
	const char* rest;
	cjson* child;
	
	//处理** : **
	item->type = JSON_OBJECT;
	child = item->child = json_New();
	
	rest = json_Skip(json_ParseValue(item->child, json_Skip(text+1)));
	if (!rest)
	      return NULL;
	child->string = child->valuestring; 
	child->valuestring = NULL;
	if (*rest != ':')
	      return NULL;
	rest = json_Skip(json_ParseValue(item->child, json_Skip(rest+1)));//+1为了跳过':'
	if (!rest)
	      return NULL;

	//处理** , **并列
	while (*rest == ',')
	{
		child->next = json_New();
		child->next->prev = child;
		child = child->next;
		if (!rest) return NULL;
		rest = json_Skip(json_ParseString(child, json_Skip(rest+1)));
		child->string = child->valuestring; 
		child->valuestring = NULL;
		if (!rest) return NULL;
		rest = json_Skip(json_ParseValue(child, json_Skip(rest+1)));
		if (!rest) return NULL;
	}

	child->next = NULL;
	return rest+1;
}

static const char* json_ParseArray(cjson* item, const char* text)
{
	const char *rest;
	cjson* child;

	if (*text != '[')
	      return NULL;

	item->type = JSON_ARRAY;
	child = item->child = json_New();
	rest = json_Skip(json_ParseValue(item->child, json_Skip(text+1)));
	if (!rest)
	      return NULL;

	while (*rest == ',')
	{
		child->next = json_New();
		child->next->prev = child;
		child = child->next;
		rest = json_Skip(json_ParseValue(child, json_Skip(rest+1)));
		if (!rest)
		      return NULL;
	}
	child->next = NULL;

	if (*rest != ']')
	      return NULL;

	return text+1;	
}
static const char* json_ParseNumber(cjson*item, const char* text)
{
	const char* number = text;
	double 	n = 0;
	int 	power_decimal = 0, power_index = 0;
	int 	sign = 1, sign_power = 1;
	if (!((*text >= '0' && *text <= '9') || (*text == '-')))
	      return NULL;

	item->type = JSON_NUMBER;

	//一般数字好办，重点是小数和E指数
	if (*number == '-')
	{
		sign = -1;
		number++;
	}
	if (*number == '0')
	{
		n = 0.0;
		number++;
	}
	else if(*number > '0' && *number <= '9')
	{
		while (*number <= '9' && *number >= '0')
		{
			n = n*10.0+*number-'0';
			number++;
		}
	}
	if (*number == '.')
	{
		number++;
		while (*number <= '9' && *number >= '0')
		{
			n = n*10.0+*number-'0';
			power_decimal--;
			number++;
		}
	}
	else 
	{
		while (*number <= '9' && *number >= '0')
		{
			n = n*10.0+*number-'0';
			number++;
		}

	}
	if (*number == 'E' || *number == 'e')
	{
		number++;
		if (*number == '+')
		      number++;
		if (*number == '-')
		{
			sign_power = -1;
			number++;
		}
		while (*number <= '9' && *number >= '0')
		{
			power_index = power_index*10.0+*number-'0';
			number++;
		}
	}

	n = n*sign*pow(10.0, power_decimal+sign_power*power_index);
	item->valuedouble = n;
	item->valueint = (int)n;

	return number;
}

static const char* json_ParseValue(cjson* item, const char* text)
{
	text = json_Skip(text);

	if (!strncmp(text, "null", 4))
	{
		item->type = JSON_NULL;
		return text+4;
	}
	if (!strncmp(text, "true", 4))
	{
		item->type = JSON_TRUE;
		return text+4;
	}
	if (!strncmp(text, "false", 5))
	{
		item->type = JSON_FALSE;
		return text+5;
	}
	if (*text == '\"') 
	      return json_ParseString(item, json_Skip(text));
	if (*text == '{') 
	      return json_ParseObject(item, json_Skip(text));
	if (*text == '[') 
	      return json_ParseArray(item, json_Skip(text));
	if (*text == '-' || (*text <= '9' && *text >= '0')) 
	      return json_ParseNumber(item, json_Skip(text));
	
	return NULL;
}

cjson* json_CreateNull()
{
	cjson* item = json_New();
	item->type = JSON_NULL;
	return item;
}
cjson* json_CreateTrue()
{
	cjson* item = json_New();
	item->type = JSON_TRUE;
	return item;
}
cjson* json_CreateFalse()
{
	cjson* item = json_New();
	item->type = JSON_FALSE;
	return item;
}
cjson* json_CreateNumber(double num)
{
	cjson* item = json_New();
	item->type = JSON_NUMBER;
	item->valuedouble = num;
	item->valueint = (int)num;
	return item;
}
cjson* json_CreateString(char* text)
{
	cjson* item = json_New();
	item->type = JSON_STRING;
	item->valuestring = (char*)malloc(strlen(text)+1);
	memcpy(item->valuestring, text, strlen(text)+1);
	return item;
}

cjson* json_CreateObject(char* string)
{
	cjson* item = json_New();
	item->type = JSON_OBJECT;
	item->string = (char*)malloc(strlen(string)+1);
	memcpy(item->string, string, strlen(string)+1);
	return item;
}
cjson* json_CreateArray(int num)
{
	cjson* item = json_New();
	cjson* child = item->child;
	int i;
	item->type = JSON_ARRAY;
	
	child = json_New();
	for ( i = 0; i < num-1; i++)
	{
		child->next = json_New();
		child->next->prev = child;
		child = child->next;
	}
	child->next = NULL;

	return item;
}
cjson* json_CreateIntArray(const int* number, int count)
{
	cjson* head;
	cjson* child;
	head = json_CreateArray(count);
	for ( child = head->child; child != NULL; child = child->next)
		child->valueint = *number;
	return head;
}

cjson* json_CreateDoubleArray(const double* number, int count)
{
	cjson* head;
	cjson* child;
	head = json_CreateArray(count);
	for ( child = head->child; child != NULL; child = child->next)
		child->valuedouble = *number;
	return head;
}

cjson* json_CreateFloatArray(const float* number, int count)
{
	cjson* head;
	cjson* child;
	head = json_CreateArray(count);
	for ( child = head->child; child != NULL; child = child->next)
		child->valuedouble = *number;
	return head;
}

void json_AddItemToObject(cjson* object, char* string)
{
	cjson* child = object->child;
	if (child)
		child = child->next;
	else return;
	while (child != NULL)
	      child = child->next;
	child = json_CreateObject(string);
}
void json_AddItemToArray(cjson* array, cjson* item)
{
	cjson* child = array->child;
	if (child)
		child = child->next;
	else return;
	while (child != NULL)
	      child = child->next;
	memcpy(child, item, sizeof(cjson));
	child->next = NULL;
}
cjson* json_GetItemFromArray(cjson* array, int where)
{
	cjson* child = array->child;
	int i = 0;
	while (child != NULL && i < where)
	      child = child->next;
	return child;
	
}
void json_ReplaceItemFromArray(cjson* array, int where, cjson* item)
{
	cjson* child = array->child;
	int i = 0;
	while (child != NULL && i < where)
	      child = child->next;
	memcpy(item, child, sizeof(cjson));
}

void json_DeleteItemFromArray(cjson* array, int where)
{
	cjson* child = array->child;
	int i = 0;
	while (child != NULL && i < where-1)
	      child = child->next;
	child->next = child->next->next;
	free(child->next);
}

static char* print_value(cjson* item, int depth, int fmt);
static char* print_null();
static char* print_true();
static char* print_false();
static char* print_array(cjson* item, int depth, int fmt);
static char* print_object(cjson* item, int depth, int fmt);
static char* print_string(char *string);
static char* print_number(cjson* item);

char* json_Print(cjson* root)
{
	return print_value(root, 0, 1);
}
char* json_PrintUnformat(cjson* root)
{
	return print_value(root, 0, 0);
}

static char* print_value(cjson* item, int depth, int fmt)
{
	char* out;

	switch (item->type)
	{
		case JSON_NULL:
			out = print_null();
			break;
		case JSON_TRUE:
			out = print_true();
			break;
		case JSON_FALSE:
			out = print_false();
			break;
		case JSON_STRING:
			out = print_string(item->valuestring);
			break;
		case JSON_NUMBER:
			out = print_number(item);
			break;
		case JSON_ARRAY:
			out = print_array(item, depth, fmt);
			break;
		case JSON_OBJECT:
			out = print_object(item, depth, fmt);
			break;
	}

	return out;
}
static char* print_null()
{
	char* str;
	str = (char*)malloc(5);
	memcpy(str, "null", 5);

	return str;
}
static char* print_true()
{
	char* str;
	str = (char*)malloc(5);
	memcpy(str, "true", 5);

	return str;
}
static char* print_false()
{
	char* str;
	str = (char*)malloc(6);
	memcpy(str, "false", 6);

	return str;
}
static char* print_string(char *string)
{
	char *str, *ptr;
	//string or valuestring
	ptr = str = (char*)malloc(strlen(string)+3);
	*str++ = '\"';
	strncpy(str, string, strlen(string));
	str += strlen(string);
	*str++ = '\"';
	*str = '\0';
	return ptr;
}

static char* print_number(cjson* item)
{
	char* str;
	double d = item->valuedouble;

	//int
	if (fabs(((double)item->valueint)-d) <= DBL_EPSILON && d <= INT_MAX && d >= INT_MIN )
	{
		str = (char*)malloc(64);/*64 char can be pepresented in 21 chars.*/
		if (str) 
		      sprintf(str, "%d", item->valueint);
	}
	//double
	else
	{
		str = (char*)malloc(64);
		if (str)
		{
			if (fabs(floor(d)-d) <= DBL_EPSILON && fabs(d) < 1.0e60)
			      sprintf(str, "%.0f", d);
			else if (fabs(d) < 1.0e-6 || fabs(d) > 1.0e9)
			      sprintf(str, "%e", d);
			else 
			      sprintf(str, "%f", d);
		}
	}

	return str;
}

static char* print_array(cjson* item, int depth, int fmt)
{
	char *out, *ptr;
	char** content;
	cjson* child;
	int num_item = 0, len = 0, i, j;

	child = item->child;
	while (child)
	{ 
		child = child->next;
		num_item++;
	}

	content = (char**)malloc(num_item*sizeof(char*));

	child = item->child;
	for ( i = 0; child != NULL; i++)
	{
		content[i] = print_value(child, depth+1, fmt);
		if (!content[i]) return NULL;
		child = child->next;
	}

	for ( i = 0; i < num_item; i++)
		len += strlen(content[i]) + 10;//contain ' ' ',' '\n'
	ptr= out = (char*)malloc(len+3);//contain "{}\0"

	*ptr++ = '[';
	if (fmt)
	{	
		*ptr++ = '\n';
		for ( i = 0; i < depth+1; i++)
		{
			*ptr++ = '\t';
		}
	}
	for ( i = 0; i < num_item; i++)
	{
		memcpy(ptr, content[i], strlen(content[i]));
		ptr = ptr+strlen(content[i]);
		if (i+1 < num_item)
		      *ptr++ = ',';
		if (fmt)
		{
			*ptr++ = '\n';
			if (i+1 >= num_item)
			      depth = depth-1;
			for ( j = 0; j < depth+1; j++)
			{
				*ptr++ = '\t';
			}
		}
	}
	*ptr++ = ']';
	*ptr++ = '\0';

	for ( i = 0; i < num_item; i++)
		if (content[i]) free(content[i]);
	if (content) free(content);
	
	return out;
}
static char* print_object(cjson* item, int depth, int fmt)
{
	char **names, **content;
	char *out, *ptr;
	int num_item = 0, len = 0, i = 0, j = 0;
	cjson* child = item->child;

	while (child)
	{
		num_item++;
		child = child->next;
	}
	
	content = (char**)malloc(num_item*sizeof(char*));
	names = (char**)malloc(num_item*sizeof(char*));

	child = item->child;
	while (child)
	{
		names[i] = print_string(child->string);
		if (!names[i]) return NULL;
		content[i] = print_value(child, depth+1, fmt);
		if (!content[i]) return NULL;
		i++;
		child = child->next;
	}

	for ( i = 0; i < num_item; i++)
		len += strlen(names[i]) + strlen(content[i]) + 4*(depth+1) + 10 ;//contain space, ': ' and '\n'

	//format and unformat judge by "if (fmt)"

	ptr = out = (char*)malloc(len+3+num_item);//contain {} and '\0' ','
	*ptr++ = '{';
	if (fmt)
	{
		*ptr++ = '\n';
		for ( j = 0; j < depth+1; j++)
		{
			*ptr++ = '\t';
		}
	}
	for ( i = 0; i < num_item; i++)
	{
		memcpy(ptr, names[i], strlen(names[i]));
		ptr = ptr+strlen(names[i]);
		*ptr++ = ':';
		if (fmt)
			*ptr++ = ' ';
		memcpy(ptr, content[i], strlen(content[i]));
		ptr = ptr+strlen(content[i]);
		if (i+1 < num_item)
		      *ptr++ = ',';
		if (fmt)
		{
			*ptr++ = '\n';
			if (i+1 >= num_item)
			      depth = depth-1;
			for ( j = 0; j < depth+1; j++)
			{
				*ptr++ = '\t';
			}
		}
	}
	*ptr++ = '}';
	*ptr = '\0';
	for ( i = 0; i < num_item; i++)
	{
		if (names[i]) free(names[i]);
		if (content[i]) free(content[i]);
	}
	if (names) free(names);
	if (content) free(content);

	return out;
}

