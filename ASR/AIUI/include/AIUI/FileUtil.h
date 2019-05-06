/*
 * FileUtil.h
 *
 *  Created on: 2017��2��21��
 *      Author: hj
 */

#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include <iostream>
#include <string>
#include <fstream>

#include <aiui/AIUIType.h>

using namespace std;
using namespace aiui;

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif


/**
 * �ļ������ࡣ
 *
 * @author <a href="http://www.xfyun.cn">Ѷ�ɿ���ƽ̨</a>
 */
class FileUtil
{
public:
	static const string SURFFIX_PCM;

	static const string SURFFIX_TXT;

	static const string SURFFIX_CFG;

	static const string SURFFIX_LOG;

	static bool exist(const string &path);

	static bool isDirectory(const string &path);

	/**
	 * ��Ŀ¼·��������ʱ����Ŀ¼��
	 *
	 * @param path ·������"/"��β
	 */
	static bool makeDirIfNoExist(const string &path);

	static bool delFile(const string &filePath);

	static string readFileAsString(const string &filePath);

	static string getCurrentTime();

	/**
	 * �·���Buffer����ȡ�ļ���Buffer��Bufferʹ����֮����Ҫʹ�����ֶ��ͷš�
	 *
	 * @return Buffer��ַ���ļ���ȡ���󷵻�NULL
	 */
	static Buffer* readFileAsBuffer(const string &filePath);

	class DataFileHelper
	{
	private:
		pthread_mutex_t mMutex;

		const string mFileDir;

		fstream mIn;

		fstream mOut;

	public:
		DataFileHelper(const string& fileDir);

		~DataFileHelper();

		bool openReadFile(const string& filePath, bool inCurrentDir = true);

		int read(char* buffer, int bufferLen);

		string readLine();

		void rewindReadFile();

		void closeReadFile();

		void createWriteFile(const string& suffix, bool append);

		void createWriteFile(const string& filename, const string& suffix, bool append, bool inCurrentDir = true);

		bool write(const char* data, int dataLen, int offset, int writeLen);

		void closeWriteFile();

		void closeFile();
	};

};



#endif /* FILEUTIL_H_ */
