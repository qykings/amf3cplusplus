// amf3demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "AMF3.h"

typedef struct _tagIoData
{
	unsigned char* buf;
	int buflen;
	int pos;
}tagIoData;

#define CHAR2BYTE(x) ((x)<=0x39?(x-0x30):((x)<0x61?(x)-0x37:(x)-0x57))
#define HEX2BYTE(h,l) (CHAR2BYTE(h)<<4|CHAR2BYTE(l))

bool IsHexChar(char chr)
{
	if(chr>=0x30 && chr<=0x39) return true;
	if(chr>=0x41 && chr<=0x46) return true;
	if(chr>=0x61 && chr<=0x66) return true;
	return false;
}

unsigned int DecodeHex(const char* pData,BYTE* pOut,int inlen)
{
	unsigned int decodesize = 0;

	if(inlen==0) 
		inlen = (int)::strlen(pData);

	char *pBinPtr = const_cast<char*>(pData);
	char *end = pBinPtr+inlen;
	BYTE *pWrite = pOut;

	for(pBinPtr; pBinPtr < end; pBinPtr+=2)
	{
		if(!IsHexChar(*pBinPtr) || !IsHexChar(*(pBinPtr+1))) break;
		++decodesize;
		*(pWrite++) = HEX2BYTE(*pBinPtr,*(pBinPtr+1));
	}

	return decodesize;
}

tagIoData* DecodeHex(const char* pData)
{
	static unsigned char* s_decode_buf =(unsigned char*)malloc(65536);

	tagIoData* io = new tagIoData();
	io->buflen = DecodeHex(pData,s_decode_buf,65536);
	io->buf = (unsigned char*)malloc(io->buflen);
	::memcpy(io->buf,s_decode_buf,io->buflen);
	io->pos = 0;
	return io;
}

int read_data(void* file,size_t size,unsigned char* buf)
{
	tagIoData* data = (tagIoData*)file;

	if((data->buflen-data->pos)<size) 
		return 0;

	::memcpy(buf,data->buf+data->pos,size);
	data->pos += size;
	return size;
}

void write_data(void* file,const unsigned char* buf,size_t size)
{
	tagIoData* data = (tagIoData*)file;
	if((data->buflen-data->pos)>=size) 
	{
		::memcpy(data->buf+data->pos,buf,size);
		data->pos += size;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//{a:"123",b:[1,"2",0.3],c:"ºº×Ö"}
	const char* test = "0a0b010363060de6b189e5ad970361060731323303620907010401060332053fd333333333333301";
	
	AMF3::context ctx;
	AMF3::init_context(&ctx,read_data,write_data);

	tagIoData* io = DecodeHex(test);
	AMF3::amf_object_handle obj = AMF3::decode(&ctx,io);

	tagIoData* out = new tagIoData;
	out->buflen = 4096;
	out->buf = (unsigned char*)malloc(out->buflen);
	out->pos = 0;
	AMF3::encode(&ctx,out,obj);

	return 0;
}

