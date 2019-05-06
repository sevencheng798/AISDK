/*
 * AIUIType.h
 *
 *  Created on: 2017��2��17��
 *      Author: AIUI����ƽ̨��https://aiui.xfyun.cn/��
 */

#ifndef AIUITYPE_H_
#define AIUITYPE_H_

#include "AIUICommon.h"

#include <stdint.h>
#include <sys/types.h>

namespace aiui {

/*
 �ڴ�����࣬���ڹ���һ���ڴ棬�ɵ��������ȵ�����ʹ�á�
 */
class Buffer
{
public:
    enum {
        eKeepStorage = 0x00000001
    };

    /*
     �ڶ��Ϸ���һ��Buffer���󣬷�����ָ�롣
     @param size Buffer��С
     */
    AIUIEXPORT static Buffer* alloc(size_t size);

    /*
     �����ü���Ϊ0ʱ�������ѷ����Buffer�ڴ档
     @param released Bufferָ��
     @return 0�ɹ���-1ʧ��
     */
    AIUIEXPORT static ssize_t dealloc(const Buffer* released);

    /*
     ��ȡֻ�ɶ���Buffer���ݵ�ַ��
     @return ���ݵ�ַ
     */
    AIUIEXPORT inline const void* data() const;

    /*
     ��ȡ�ɶ�д��Buffer�����ݵ�ַ��
     @return ���ݵ�ַ
     */
    AIUIEXPORT inline void* data();

    /*
     ��ȡBuffer�Ĵ�С���ֽ�������
     @return Buffer��С
     */
    AIUIEXPORT inline size_t size() const;

    /*
     �����ݵ�ַ�õ�Bufferָ�루�ɶ�д����
     @param data ���ݵ�ַ
     @param ��Ӧ��Bufferָ��
     */
    AIUIEXPORT static inline Buffer* bufferFromData(void* data);

    /*
	 �����ݵ�ַ�õ�Bufferָ�루ֻ�ɶ�����
	 @param ���ݵ�ַ
	 @param ��Ӧ��Bufferָ��
	 */
    AIUIEXPORT static inline const Buffer* bufferFromData(const void* data);

    /*
     ��ȡBuffer��С��
     @param data()�������ص����ݵ�ַ
     @return Buffer��С
     */
    AIUIEXPORT static inline size_t sizeFromData(const void* data);

    /*
     ��ȡ�ɶ�д��Bufferָ�롣
     @return �ɶ�д��Bufferָ��
     */
    AIUIEXPORT Buffer* edit() const;

    /*
     ��������Buffer��С��������Bufferָ�룬��Buffer��Сʱ�ᶪʧ����ԭʼ���ݡ�
     @param size ��Buffer��С
     @param Bufferָ��
     */
    AIUIEXPORT Buffer* editResize(size_t newSize) const;

    /*
     ��Buffer���ü���Ϊ1ʱ������Bufferָ�룬���򷵻�NULL��
     @return Bufferָ��
     */
    AIUIEXPORT Buffer* attemptEdit() const;

    /*
     ��������Buffer��С��ԭʼ���ݻᶪʧ��
     @param size Buffer��С
     @return ��Bufferָ��
     */
    AIUIEXPORT Buffer* reset(size_t size) const;

    /*
     ����Buffer�����ü�����
     */
    AIUIEXPORT void acquire() const;

    /*
     ��Buffer�����ü���Ϊ1ʱ���ͷ�Buffer��ռ�ڴ档
     */
    AIUIEXPORT int32_t release(uint32_t flags = 0) const;

    /*
      ��ȡBuffer�����ü����Ƿ�Ϊ1��
     */
    AIUIEXPORT inline bool onlyOwner() const;

    /*
     ��ȡBuffer�Ŀ��������ڶ������·����ڴ档
     @return ��������ָ��
     */
	AIUIEXPORT Buffer* copy();


private:
	inline Buffer() : mRefs(0), mSize(0) { }

	inline ~Buffer() { }

	Buffer(const Buffer&);

	Buffer& operator = (const Buffer&);

	mutable int32_t mRefs;

	size_t mSize;

	uint32_t mReserved[2];
};


/*
  ���������֧࣬��int��long��string��Buffer*�������������䡣
 */
class IDataBundle
{
public:
	AIUIEXPORT virtual ~IDataBundle();

	/*
	 ���������������
	 @return ����ָ��
	 */
	AIUIEXPORT static IDataBundle* create();

	/*
	  ���ٶ���
	 */
	virtual void destroy() = 0;

	/*
	 �Ƴ�key��Ӧ����Ŀ��
	 @param key ��Ŀ��keyֵ
	 */
	virtual bool remove(const char* key) = 0;

	/* 
	 ���·���������ӣ���ȡ��int��string��Buffer*�������ݡ�
	*/
	virtual bool putInt(const char* key, int val, bool replace = false) = 0;

	virtual int getInt(const char* key, int defaultVal) = 0;

	virtual bool putLong(const char* key, long val, bool replace = false) = 0;

	virtual long getLong(const char* key, long defaultVal) = 0;

	virtual bool putString(const char* key, const char* val, bool replace = false) = 0;

	/**
	  ע��getString���ص�ַ��Ӧ���ڴ���IDataBundle�������ɣ�����Ҫ�����ⲿdelete����free��
	 **/
	virtual const char* getString(const char* key, const char* defaultVal) = 0;

	virtual bool putBinary(const char* key, const char* data, int dataLen, bool replace = false) = 0;

	virtual const char* getBinary(const char* key, int* dataLen) = 0;
};



// ---------------------------------------------------------------------------

const void* Buffer::data() const {
	return this + 1;
}

void* Buffer::data() {
	return this + 1;
}

size_t Buffer::size() const {
	return mSize;
}

Buffer* Buffer::bufferFromData(void* data) {
	return data ? static_cast<Buffer *>(data)-1 : 0;
}

const Buffer* Buffer::bufferFromData(const void* data) {
	return data ? static_cast<const Buffer *>(data)-1 : 0;
}

size_t Buffer::sizeFromData(const void* data) {
	return data ? bufferFromData(data)->mSize : 0;
}

bool Buffer::onlyOwner() const {
	return (mRefs == 1);
}

}

#ifdef AIUI_LIB_COMPILING
#include "aiui_internal/AIUIType_internal.h"
#endif

#endif /* AIUITYPE_H_ */
