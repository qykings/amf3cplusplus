
#pragma once

//----------------------------------------------
//Name：AMF3 Protocol
//About：AMF3协议（部分），暂时仅支持win平台
//Author：6Spring 
//Time: 2010/04/07
//EMail: 6spring@gmail.com
//----------------------------------------------
#ifndef __AMF3_H__
#define __AMF3_H__

#include <Windows.h>

#include <string>
#include <map>
#include <vector>


namespace AMF3
{

//避免解析错误出过大object，最大支持的动态子元素个数
#define DYNAMIC_OBJ_MAX_PROPERTY	512
	
//引用标志位
#define REFERENCE_BIT	0x01

//空串
#define EMPTY_STRING	0x01

//IO 读写函数
typedef int (*READ_FUNC)(void* file,size_t size,unsigned char* buf);
typedef void (*WRITE_FUNC)(void* file,const unsigned char* buf,size_t size);

//数据类型定义
enum DataType
{
	DT_UNDEFINED = 0x00,	//unsupport
	DT_NULL = 0x01,
	DT_FALSE = 0x02,
	DT_TRUE = 0x03,
	DT_INTEGER = 0x04,
	DT_DOUBLE = 0x05,
	DT_STRING = 0x06,
	DT_XMLDOC = 0x07,		//unsupport
	DT_DATE = 0x08,
	DT_ARRAY = 0x09,
	DT_OBJECT = 0x0A,
	DT_XML = 0x0B,			//unsupport
	DT_BYTEARRAY = 0x0C		
};


class amf_object;

///////////////////////////////////////////////////////////////////////////////
//obj handle 用于object的引用计数自动加减
//
///////////////////////////////////////////////////////////////////////////////
class amf_object_handle
{
public:
	amf_object_handle();
	amf_object_handle(const amf_object_handle &rhs);
	amf_object_handle(amf_object* obj);
	~amf_object_handle(void);

public:
	bool isnull();
	void release();

public:
	amf_object_handle& operator = (const amf_object_handle &rhs);
	amf_object_handle& operator = (amf_object* obj);
	amf_object* operator -> () const;

	bool operator == (const amf_object_handle &rhs) const;

private:
	amf_object* pobj;
};

typedef std::vector<amf_object_handle> type_amf_ref_tab;

///////////////////////////////////////////////////////////////////////////////
//object 元素数据对象
//
///////////////////////////////////////////////////////////////////////////////
class amf_object
{
	friend class amf_object_handle;

public:
	static amf_object_handle Alloc();

protected:
	amf_object();
	~amf_object(void);

public:
	void clear_value();

public:
	double get_time_seed();
	void set_time_seed(double t);

public:
	void set_as_unsigned_number(unsigned int num);
	void set_as_number(int num);

public:
	void add_ref();
	void release();

public:
	void add_child(amf_object_handle obj);
	bool has_child(const char* name);
	amf_object_handle get_child(const char* name);

private:
	long ref;

public:
	std::string classname;
	std::string name;

	DataType type;

	bool boolValue;
	int intValue;
	double doubleValue;
	std::string strValue;
	SYSTEMTIME dateValue;

	int bytearrayLen;
	unsigned char* bytearrayValue;

	std::vector<amf_object_handle> childrens;
};
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
typedef struct _context
{
	void* m_IFileHandle;
	void* m_OFileHandle;

	READ_FUNC m_ReadFunc;
	WRITE_FUNC m_WriteFunc;

	unsigned char* s_ReadBuf;
	int s_ReadBufSize;

	type_amf_ref_tab m_szReadObjRefTab;
	type_amf_ref_tab m_szReadStrRefTab;

}context;
///////////////////////////////////////////////////////////////////////////////

void init_context(context* ctx,READ_FUNC pRFunc,WRITE_FUNC pWFunc);
void clear_context(context* ctx);

amf_object_handle decode(context* ctx,void* file_handle);
void encode(context* ctx,void* file_handle,amf_object_handle obj);


}//end namespace AMF3



#endif