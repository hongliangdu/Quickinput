/*
 Copyright (c) 2009 Dave Gamble

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

/*******************************************************************************
 * Project:  neb
 * @file     CJsonObject.hpp
 * @brief    Json
 * @author   bwarliao
 * @date:    2014-7-16
 * @note
 * Modify history:
 ******************************************************************************/

#ifndef CJSONOBJECT_HPP_
#define CJSONOBJECT_HPP_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <malloc.h>
#include <math.h>
#include <string>
#include <list>

#if __cplusplus < 201101L
#include <map>
#else
#include <unordered_map>
#endif

/* cJSON Types: */
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Int 3
#define cJSON_Double 4
#define cJSON_String 5
#define cJSON_Array 6
#define cJSON_Object 7

#define cJSON_IsReference 256

typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

/* The cJSON structure: */
typedef struct cJSON
{
    struct cJSON *next, *prev; /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    int type; /* The type of the item, as above. */

    char *valuestring; /* The item's string, if type==cJSON_String */
    int64 valueint; /* The item's number, if type==cJSON_Number */
    double valuedouble; /* The item's number, if type==cJSON_Number */
    int sign;   /* sign of valueint, 1(unsigned), -1(signed) */

    char *string; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} cJSON;

typedef struct cJSON_Hooks
{
    void *(*malloc_fn)(size_t sz);
    void (*free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc, realloc and free functions to cJSON */
void cJSON_InitHooks(cJSON_Hooks* hooks);

/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
cJSON *cJSON_Parse(const char *value, const char **ep);
/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
char *cJSON_Print(cJSON *item);
/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
char *cJSON_PrintUnformatted(cJSON *item);
/* Delete a cJSON entity and all subentities. */
void cJSON_Delete(cJSON *c);

/* Returns the number of items in an array (or object). */
int cJSON_GetArraySize(cJSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
cJSON *cJSON_GetArrayItem(cJSON *array, int item);
/* Get item "string" from object. Case insensitive. */
cJSON *cJSON_GetObjectItem(cJSON *object, const char *string);

/* remove gloal variable for thread safe. --by Bwar on 2020-11-15 */
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
/* extern const char *cJSON_GetErrorPtr(); */

/* These calls create a cJSON item of the appropriate type. */
cJSON *cJSON_CreateNull();
cJSON *cJSON_CreateTrue();
cJSON *cJSON_CreateFalse();
cJSON *cJSON_CreateBool(int b);
cJSON *cJSON_CreateDouble(double num, int sign);
cJSON *cJSON_CreateInt(uint64 num, int sign);
cJSON *cJSON_CreateString(const char *string);
cJSON *cJSON_CreateArray();
cJSON *cJSON_CreateObject();

/* These utilities create an Array of count items. */
cJSON *cJSON_CreateIntArray(int *numbers, int sign, int count);
cJSON *cJSON_CreateFloatArray(float *numbers, int count);
cJSON *cJSON_CreateDoubleArray(double *numbers, int count);
cJSON *cJSON_CreateStringArray(const char **strings, int count);

/* Append item to the specified array/object. */
void cJSON_AddItemToArray(cJSON *array, cJSON *item);
void cJSON_AddItemToArrayHead(cJSON *array, cJSON *item);    /* add by Bwar on 2015-01-28 */
void cJSON_AddItemToObject(cJSON *object, const char *string,
                cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
void cJSON_AddItemReferenceToObject(cJSON *object, const char *string,
                cJSON *item);

/* Remove/Detatch items from Arrays/Objects. */
cJSON *cJSON_DetachItemFromArray(cJSON *array, int which);
void cJSON_DeleteItemFromArray(cJSON *array, int which);
cJSON *cJSON_DetachItemFromObject(cJSON *object, const char *string);
void cJSON_DeleteItemFromObject(cJSON *object, const char *string);

/* Update array items. */
void cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
void cJSON_ReplaceItemInObject(cJSON *object, const char *string, cJSON *newitem);

#define cJSON_AddNullToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateNull())
#define cJSON_AddTrueToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

namespace neb
{

class CJsonObject
{
public:     // method of ordinary json object or json array
    CJsonObject();
    CJsonObject(const std::string& strJson);
    CJsonObject(const CJsonObject* pJsonObject);
    CJsonObject(const CJsonObject& oJsonObject);
#if __cplusplus >= 201101L
    CJsonObject(CJsonObject&& oJsonObject);
#endif
    virtual ~CJsonObject();

    CJsonObject& operator=(const CJsonObject& oJsonObject);
#if __cplusplus >= 201101L
    CJsonObject& operator=(CJsonObject&& oJsonObject);
#endif
    bool operator==(const CJsonObject& oJsonObject) const;
    bool Parse(const std::string& strJson);
    void Clear();
    bool IsEmpty() const;
    bool IsArray() const;
    std::string ToString() const;
    std::string ToFormattedString() const;
    const std::string& GetErrMsg() const
    {
        return(m_strErrMsg);
    }

public:     // method of ordinary json object
    bool AddEmptySubObject(const std::string& strKey);
    bool AddEmptySubArray(const std::string& strKey);
    bool GetKey(std::string& strKey);
    void ResetTraversing();
    CJsonObject& operator[](const std::string& strKey);
    std::string operator()(const std::string& strKey) const;
    bool KeyExist(const std::string& strKey) const;
    bool Get(const std::string& strKey, CJsonObject& oJsonObject) const;
    bool Get(const std::string& strKey, std::string& strValue) const;
    bool Get(const std::string& strKey, int32& iValue) const;
    bool Get(const std::string& strKey, uint32& uiValue) const;
    bool Get(const std::string& strKey, int64& llValue) const;
    bool Get(const std::string& strKey, uint64& ullValue) const;
    bool Get(const std::string& strKey, bool& bValue) const;
    bool Get(const std::string& strKey, float& fValue) const;
    bool Get(const std::string& strKey, double& dValue) const;
    int GetValueType(const std::string& strKey) const;
    bool IsNull(const std::string& strKey) const;
    bool Add(const std::string& strKey, const CJsonObject& oJsonObject);
#if __cplusplus < 201101L
    bool AddWithMove(const std::string& strKey, CJsonObject& oJsonObject);
#else
    bool Add(const std::string& strKey, CJsonObject&& oJsonObject);
#endif
    bool Add(const std::string& strKey, const std::string& strValue);
    bool Add(const std::string& strKey, int32 iValue);
    bool Add(const std::string& strKey, uint32 uiValue);
    bool Add(const std::string& strKey, int64 llValue);
    bool Add(const std::string& strKey, uint64 ullValue);
    bool Add(const std::string& strKey, bool bValue, bool bValueAgain);
    bool Add(const std::string& strKey, float fValue);
    bool Add(const std::string& strKey, double dValue);
    bool AddNull(const std::string& strKey);    // add null like this:   "key":null
    bool Delete(const std::string& strKey);
    bool Replace(const std::string& strKey, const CJsonObject& oJsonObject);
#if __cplusplus < 201101L
    bool ReplaceWithMove(const std::string& strKey, CJsonObject& oJsonObject);
#else
    bool Replace(const std::string& strKey, CJsonObject&& oJsonObject);
#endif
    bool Replace(const std::string& strKey, const std::string& strValue);
    bool Replace(const std::string& strKey, int32 iValue);
    bool Replace(const std::string& strKey, uint32 uiValue);
    bool Replace(const std::string& strKey, int64 llValue);
    bool Replace(const std::string& strKey, uint64 ullValue);
    bool Replace(const std::string& strKey, bool bValue, bool bValueAgain);
    bool Replace(const std::string& strKey, float fValue);
    bool Replace(const std::string& strKey, double dValue);
    bool ReplaceWithNull(const std::string& strKey);    // replace value with null
#if __cplusplus < 201101L
    bool ReplaceAdd(const std::string& strKey, const CJsonObject& oJsonObject);
    bool ReplaceAdd(const std::string& strKey, const std::string& strValue);
    template <typename T>
    bool ReplaceAdd(const std::string& strKey, T value) 
    {
        if (KeyExist(strKey))
        {
            return(Replace(strKey, value));
        }
        return(Add(strKey, value));
    }
#else
    template <typename T>
    bool ReplaceAdd(const std::string& strKey, T&& value)
    {
        if (KeyExist(strKey))
        {
            return(Replace(strKey, std::forward<T>(value)));
        }
        return(Add(strKey, std::forward<T>(value)));
    }
#endif

public:     // method of json array
    int GetArraySize() const;
    CJsonObject& operator[](unsigned int uiWhich);
    std::string operator()(unsigned int uiWhich) const;
    bool Get(int iWhich, CJsonObject& oJsonObject) const;
    bool Get(int iWhich, std::string& strValue) const;
    bool Get(int iWhich, int32& iValue) const;
    bool Get(int iWhich, uint32& uiValue) const;
    bool Get(int iWhich, int64& llValue) const;
    bool Get(int iWhich, uint64& ullValue) const;
    bool Get(int iWhich, bool& bValue) const;
    bool Get(int iWhich, float& fValue) const;
    bool Get(int iWhich, double& dValue) const;
    int GetValueType(int iWhich) const;
    bool IsNull(int iWhich) const;
    bool Add(const CJsonObject& oJsonObject);
#if __cplusplus < 201101L
    bool AddWithMove(CJsonObject& oJsonObject);
#else
    bool Add(CJsonObject&& oJsonObject);
#endif
    bool Add(const std::string& strValue);
    bool Add(int32 iValue);
    bool Add(uint32 uiValue);
    bool Add(int64 llValue);
    bool Add(uint64 ullValue);
    bool Add(int iAnywhere, bool bValue);
    bool Add(float fValue);
    bool Add(double dValue);
    bool AddNull();   // add a null value
    bool AddAsFirst(const CJsonObject& oJsonObject);
#if __cplusplus < 201101L
    bool AddAsFirstWithMove(CJsonObject& oJsonObject);
#else
    bool AddAsFirst(CJsonObject&& oJsonObject);
#endif
    bool AddAsFirst(const std::string& strValue);
    bool AddAsFirst(int32 iValue);
    bool AddAsFirst(uint32 uiValue);
    bool AddAsFirst(int64 llValue);
    bool AddAsFirst(uint64 ullValue);
    bool AddAsFirst(int iAnywhere, bool bValue);
    bool AddAsFirst(float fValue);
    bool AddAsFirst(double dValue);
    bool AddNullAsFirst();     // add a null value
    bool Delete(int iWhich);
    bool Replace(int iWhich, const CJsonObject& oJsonObject);
#if __cplusplus < 201101L
    bool ReplaceWithMove(int iWhich, CJsonObject& oJsonObject);
#else
    bool Replace(int iWhich, CJsonObject&& oJsonObject);
#endif
    bool Replace(int iWhich, const std::string& strValue);
    bool Replace(int iWhich, int32 iValue);
    bool Replace(int iWhich, uint32 uiValue);
    bool Replace(int iWhich, int64 llValue);
    bool Replace(int iWhich, uint64 ullValue);
    bool Replace(int iWhich, bool bValue, bool bValueAgain);
    bool Replace(int iWhich, float fValue);
    bool Replace(int iWhich, double dValue);
    bool ReplaceWithNull(int iWhich);      // replace with a null value

private:
    CJsonObject(cJSON* pJsonData);

private:
    cJSON* m_pJsonData;
    cJSON* m_pExternJsonDataRef;
    cJSON* m_pKeyTravers;
    const char* mc_pError;
    std::string m_strErrMsg;
#if __cplusplus < 201101L
    std::map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
    std::map<unsigned int, CJsonObject*>::iterator m_array_iter;
    std::map<std::string, CJsonObject*> m_mapJsonObjectRef;
    std::map<std::string, CJsonObject*>::iterator m_object_iter;
#else
    std::unordered_map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
    std::unordered_map<std::string, CJsonObject*>::iterator m_object_iter;
    std::unordered_map<std::string, CJsonObject*> m_mapJsonObjectRef;
    std::unordered_map<unsigned int, CJsonObject*>::iterator m_array_iter;
#endif
};

}

#endif /* CJSONHELPER_HPP_ */
