/*
 * 	AIUI.h
 *
 *  Created on: 2017��2��17��
 *      Author: AIUI����ƽ̨��https://aiui.xfyun.cn/��
 */

#ifndef AIUI_API_H_
#define AIUI_API_H_

#include "AIUIConstant.h"
#include "AIUIErrorCode.h"
#include "AIUIType.h"
#include "AIUICommon.h"

namespace aiui {

/**
 * ��ȡSDK�汾��Ϣ��
 */
AIUIEXPORT const char* getVersion();


/**
 * AIUI�¼��ࡣҵ������SDK�ڲ�״̬�仯�������Ϣ��ͨ���¼��׳���
 *
 * ע��SDKͨ���ص��׳��¼���������ͷ��¼��е��ڴ������Ҫ�ڱ�ʹ�õ���Щ�ڴ���󣬿�������Ҫ�ڻص�����������
 */
class IAIUIEvent
{
public:
	AIUIEXPORT virtual ~IAIUIEvent();

	/**
	 * ��ȡ�¼����͡�ȡֵ�μ�AIUIConstant��EVENT_��ͷ�ĳ������塣
	 */
	virtual int getEventType() const = 0;

	/**
	 * ��ȡ��չ����1��
	 */
	virtual int getArg1() const = 0;

	/**
	 * ��ȡ��չ����2��
	 */
	virtual int getArg2() const = 0;

	/**
	 * ��ȡ������Ϣ�����ص�ָ�벻���ⲿdelete����free��
	 */
	virtual const char* getInfo() const = 0;

	/**
	 * ��ȡ�������ݡ����ص��ڴ治���ⲿdelete����free��
	 */
	virtual IDataBundle* getData() const = 0;
};


/**
 * AIUI��Ϣ�ࡣAIUI���е����붼��ͨ����Ϣ���͵�SDK�ڲ���
 */
class IAIUIMessage
{
public:
	AIUIEXPORT virtual ~IAIUIMessage();

	/**
	 * ������Ϣ����
	 *
     * @param msgType ��Ϣ���ͣ��μ�AIUIConstant��CMD_��ͷ�ĳ�������
	 * @param arg1 ��չ����1
	 * @param arg2 ��չ����2
	 * @param params ҵ�������������ڲ���������
     * @param data �������ݣ�Message��SDK�ڲ��������Զ��ͷ�, ����Ҫ�ⲿ�ͷ�
     *
	 * @return IAIUIMessage ����ָ��
	 */
	AIUIEXPORT static IAIUIMessage* create(
		int msgType,
		int arg1 = 0,
		int arg2 = 0,
		const char* params = "",
		Buffer* data = 0);

	/**
	 * ��ȡ��Ϣ���͡�����ȡֵ�μ�AIUIConstant��CMD_��ͷ�ĳ������塣
	 */
	virtual int getMsgType() const = 0;

	/**
	 * ��ȡ��չ����1��
	 */
	virtual int getArg1() const = 0;

	/**
	 * ��ȡ��չ����2��
	 */
	virtual int getArg2() const = 0;

	/**
	 * ��ȡҵ�������
	 */
	virtual const char* getParams() const = 0;

	/**
	 * ��ȡ�������ݡ�
	 */
	virtual Buffer* getData() const = 0;

	/**
	 * �ͷŸ������ݡ�
	 */
	virtual void releaseData() = 0;

	/**
	 * ������Ϣ������
	 *
	 * ע���÷��������ͷ���Ϣ���е�Buffer* dataָ�롣
	 */
	virtual void destroy() = 0;
};


/**
 * AIUI�¼������ӿڡ�SDK���������ͨ���ص�onEvent�����׳���
 */
#ifdef AIUI_WINDOWS
class AIUIListener
#else
class AIUIEXPORT AIUIListener
#endif
{
public:
	AIUIEXPORT virtual ~AIUIListener();

	/**
	 * �¼��ص���SDK���������ͨ��event�׳���
	 *
	 * @param event AIUI�¼�
	 */
	AIUIEXPORT virtual void onEvent(const IAIUIEvent& event) const = 0;
};

typedef AIUIListener IAIUIListener;

/**
 * AIUI�����࣬��������Ӧ��ͨ�����������AIUI������
 */
class IAIUIAgent
{
public:
	AIUIEXPORT virtual ~IAIUIAgent();

	/**
	 * ����Agent��������SDK��ʼ���С�
	 *
	 * ע���÷������Ƿ��طǿն��󣬷ǿղ����������������޴�������
	 *
	 * @param params ��������
	 * @param listener �¼�������
	 * @return AIUIAgent ��������ָ��
	 */
	AIUIEXPORT static IAIUIAgent* createAgent(const char* params, const IAIUIListener* listener);

	/**
	 * ������Ϣ��AIUI����Ϣ�п��԰���������������ݣ������ʽ�μ�IAIUIMessage��
	 *
	 * @param message AIUI��Ϣ�����ָ���˷ǿյ�Buffer *data����SDK�ڲ��������Զ�release()�ⲿ��data������Ҫ�ⲿȥ�ͷ�
	 */
	virtual void sendMessage(const IAIUIMessage* message) = 0;

	/**
	 * ����Agent����SDKֹͣ���С�
	 */
	virtual void destroy() = 0;
};


/**
 * ��־����
 */
enum LogLevel
{
	_info,
	_debug,
	_warn,
	_error,
	_none
};


/**
 * �汾���͡�
 */
enum VersionType
{
	INTELLIGENT_HDW,	// ����Ӳ���汾
	MOBILE_PHONE,		// �ƶ��������汾
	DESKTOP_PC			// ����PC�汾
};


/**
 * AIUI�����࣬���ڿ�����־�����á�
 */
class AIUISetting
{
public:
	/**
	 * ����AIUI����Ŀ¼��SDK���ڸ�·���±�����־�����ݵ��ļ���
	 *
	 * @param dir ·����windows����"\\"��β������ƽ̨��"/"��β��������Ϊ��
	 * @return �Ƿ����óɹ�
	 */
	AIUIEXPORT static bool setAIUIDir(const char* szDir);

	/**
	 * ����MSC����Ŀ¼��SDK��Ӹ�·���¶�ȡ�������ã������ƶ˽�����־��
	 *
	 * ע����Ҫ��createAgent֮ǰ���ã�������Ч��
	 *
	 * @param dir ·����windows����"\\"��β������ƽ̨��"/"��β��������Ϊ��
	 * @return �Ƿ����óɹ�
	 */
	AIUIEXPORT static bool setMscDir(const char* szDir);

	/**
	 * ����msc.cfg�����ݵ�SDK�С�
	 *
	 * ע����Ҫ��createAgent֮ǰ���ã�������Ч��
	 *
	 * @param cfg msc.cfg�е�ʵ������
	 * @return �Ƿ����óɹ�
	 */
	AIUIEXPORT static bool setMscCfg(const char* szCfg);

	/**
	 * ��ʼ����־��¼����������־����Ŀ¼��
	 *
	 * ע����Ҫ��createAgent֮ǰ���ã�������Ч��
	 *
	 * @param logDir ��־Ŀ¼��windows����"\\"��β������ƽ̨��"/"��β����Ϊ������AIUI����Ŀ¼�´���logĿ¼
	 * @return �Ƿ�ɹ�
	 */
	AIUIEXPORT static bool initLogger(const char* szLogDir = "");

	/**
	 * �����ϲ������־��ӡ����Ĭ�ϼ���Ϊinfo��
	 *
	 * ע��
	 * 		1.Android��ͨ��logcat���������־��
	 * 		2.iOS��ͨ������̨���������־��
	 * 		3.Linux/Windows�����޵�����־�����
	 *
	 * @param level ��־�ȼ�������ȡֵ�ο�LogLevel����
	 */
	AIUIEXPORT static void setLogLevel(LogLevel level);

	/**
	 * ���õײ����罻����־��ӡ����
	 *
	 * ע�����罻����־Ĭ�ϴ�ӡ��aiui.log�ļ���
	 *
	 * @param level ��־�ȼ�������ȡֵ�ο�LogLevel����
	 */
	AIUIEXPORT static void setNetLogLevel(LogLevel level);

	/**
	 * �����Ƿ񱣴�������־�����������Ƶ���ƶ˷��صĽ����
	 *
	 * @param save �Ƿ񱣴�
	 */
	AIUIEXPORT static void setSaveDataLog(bool save);

	/**
	 * ����������־����Ŀ¼����������Ĭ������AIUI����Ŀ¼�µ�dataĿ¼��
	 *
	 * @param dir Ŀ¼��windows����"\\"��β������ƽ̨��"/"��β��
	 * @return �Ƿ����óɹ�
	 */
	AIUIEXPORT static bool setDataLogDir(const char* szDir);

	/**
	 * ����ԭʼ��Ƶ����Ŀ¼����������Ĭ������AIUI����Ŀ¼�µ�audio/raw/Ŀ¼��
	 *
	 * @param dir Ŀ¼��windows����"\\"��β������ƽ̨��"/"��β��
	 */
	AIUIEXPORT static bool setRawAudioDir(const char* dir);

	/**
	 * �Ƿ�Ϊ�ƶ��������汾��
	 *
	 * @return true����false
	 */
	AIUIEXPORT static bool isMobileVersion();

	/**
	 * ��ȡ�汾���͡�
	 *
	 * @return �ο�VersionType����
	 */
	AIUIEXPORT static VersionType getVersionType();
};

AIUIEXPORT const char* getAIUIDir();

}

#ifdef AIUI_LIB_COMPILING
#include "aiui_internal/AIUI_internal.h"
#endif

#endif /* AIUI_API_H_ */
