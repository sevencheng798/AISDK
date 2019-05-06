/*
 * AIUIConstant.h
 *
 *  Created on: 2017��2��17��
 *      Author: AIUI����ƽ̨��https://aiui.xfyun.cn/��
 */

#ifndef AIUICONSTANT_H_
#define AIUICONSTANT_H_

#include "AIUICommon.h"

namespace aiui {

class AIUIEXPORT AIUIConstant
{
public:
	/*******************�¼�����**********************/
	/**
	 * ����¼���infoΪ�������JSON�ַ�����dataΪ�������map��ͨ������params
	 * ���Ի�ȡ������ͺͽ����keyֵ��Ȼ����keyֵ��data��ȡ�����
	 **/
	static const int EVENT_RESULT = 1;

	/**
	 * �����¼���arg1Ϊ�����룬infoΪ����������Ϣ��
	 **/
	static const int EVENT_ERROR = 2;

	/**
	 * ״̬�¼���arg1Ϊ����״̬��ȡֵ��STATE_IDLE}��STATE_READY��STATE_WORKING��
	 **/
	static const int EVENT_STATE = 3;

	/**
	 * �����¼���infoΪ���ѽ��JSON�ַ�����ʾ����<br/>
	 * <pre>{
	 *   "power": 12342435436,   // ��������ֵ
	 *   "beam":3,               // ʰ�������ţ����ѳɹ������н��ڸò���������ʰ��
	 *   "angle":180,            // ���ѽǶ�
	 *   "channel":5,            // ��������������˷��ţ���ʾ����������Ƶ�������
	 *   "CMScore":132           // ������Ӧ�Ļ��ѵ÷�
	 * }</pre>
	 **/
	static const int EVENT_WAKEUP = 4;

	/**
	 * �����¼��������ֽ�����ʱ�������ⲿ����CMD_RESET_WAKEUP��Ϣ���û���ʱ�׳����¼���
	 * arg1ȡֵ��TYPE_AUTO���Զ����ߣ���TYPE_COMPEL���ⲿǿ�����ߣ�����CMD_RESET_WAKEUP����
	 **/
	static const int EVENT_SLEEP = 5;

	/**
	 * VAD�¼���arg1��ʾVAD��Ϣ���ͣ�ȡֵ��VAD_BOS��VAD_VOL��VAD_EOS��
	 * ��arg1ΪVAD_VOLʱ��arg2��ʾ������ȡֵ��Χ��[0-30]��
	 */
	static const int EVENT_VAD = 6;

	/**
	 * �󶨳ɹ��¼�����ServiceKit�����󶨳ɹ�ʱ�׳���Linux�汾�޴��¼���
	 */
	static const int EVENT_BIND_SUCESS = 7;

	/**
	 * CMD��Ӧ�ķ����¼���arg1��ʾ��Ӧ��CMD�����CMD_BUILD_GRAMMAR�ȣ���arg2Ϊ�����루0��ʾ�ɹ�����
	 * infoΪ���صĽ����
	 */
	static const int EVENT_CMD_RETURN = 8;

	/**
	 * ��Ƶ�׳��¼���16k��Ƶ������data�У�ͨ����"audio"��ȡ��
	 */
	static const int EVENT_AUDIO = 9;

	/**
	 * ׼�������¼����׳����¼�֪ͨ�ⲿ�������ߣ���10��֮������Ч�������׳�EVENT_SLEEP��
	 */
	static const int EVENT_PRE_SLEEP = 10;

	/**
	 * ��ʼ¼���¼���֪ͨ�ⲿ¼����ʼ���û����Կ�ʼ˵����
	 */
	static const int EVENT_START_RECORD = 11;

	/**
	 * ֹͣ¼���¼���
	 */
	static const int EVENT_STOP_RECORD = 12;

	/**
	 * �����˽����������¼����������Ӻ���ܽ���������ͬ���ȸ��ֲ�����
	 */
	static const int EVENT_CONNECTED_TO_SERVER = 13;

	/**
	 * �����˶Ͽ������¼���
	 */
	static const int EVENT_SERVER_DISCONNECTED = 14;

	/**
	 * TTS�����׳��¼���
	 */
	static const int EVENT_TTS = 15;

	/**
	 * CAE��Ȩ�׳��������ġ�
	 */
	static const int EVENT_CAE_PLAIN_TEXT = 1000;

	/**
	 * ������Ϣ�¼���
	 */
	static const int EVENT_PUSH_MESSAGE = 1001;

	/*******************����״̬**********************/
	/**
	 * ����״̬��AIUI����δ������
	 **/
	static const int STATE_IDLE = 1;

	/**
	 * ����״̬���Ѿ�����¼�����ȴ����ѡ�
	 **/
	static const int STATE_READY = 2;

	/**
	 * ����״̬���Ѿ����ѣ����Կ�ʼ�˻�������
	 **/
	static const int STATE_WORKING = 3;

	/*******************�����**********************/
	/**
	 * ��ȡAIUI����״̬�����͸������AIUI���ӦEVENT_STATE�¼���
	 **/
	static const int CMD_GET_STATE = 1;

	/**
	 * д�����ݡ�paramsΪ����������Ϣ���������͵ȣ���JSON�ַ�����dataЯ�����������ݡ�
	 **/
	static const int CMD_WRITE = 2;

	/**
	 * ֹͣд�롣paramsΪ����������Ϣ���������͵ȣ���JSON�ַ�����
	 **/
	static const int CMD_STOP_WRITE = 3;

	/**
	 * ���������൱��ֹͣ����������������ʱ�����¼���������ʹ��������Ч��
	 **/
	static const int CMD_RESET = 4;

	/**
	 * �������������ڷ���ֹͣʱ������������ʱ�����¼���������ʹ��������Ч��
	 **/
	static const int CMD_START = 5;

	/**
	 * ֹͣ���������ڷ�������ʱֹͣ����ֹͣ���ٽ������롣
	 **/
	static const int CMD_STOP = 6;

	/**
	 * ������������ⲿ�������ѡ�arg1�ֶ�Ϊ��˷����в�����ţ�Ĭ��Ϊ0�Ų���������
	 * �����ò�������ʰ�������ѳɹ��󣬻��׳�EVENT_WAKEUP�¼���
	 **/
	static const int CMD_WAKEUP = 7;

	/**
	 * ���û��ѡ���AIUI����Ϊ������״̬����STATE_WORKING��ΪSTATE_READY��
	 **/
	static const int CMD_RESET_WAKEUP = 8;

	/**
	 * ��������ʰ��������arg1Ϊ������ţ�������������͵Ĳ�ͬ����ͬ�����ú����ò�������ʰ����
	 **/
	static const int CMD_SET_BEAM = 9;

	/**
	 * ���ò�����paramsΪaiui.cfg�ļ����ݣ���һ��JSON�ַ�����
	 **/
	static const int CMD_SET_PARAMS = 10;

	/**
	 * �ϴ��û��ʱ�paramsΪJSON�ַ�����ʾ����
	 * <pre>{
	 *   "name":"userword",   // �ʱ�����
	 *   "content":"XXXX"     // �ʱ�����
	 * }</pre>
	 * ����XXXXҲΪһ��JSON�ַ�����ʾ����
	 * <pre>{
	 *   "userword":[
	 *    {
	 *       "name":"�ҵĳ��ô�",
	 *       "words":["�ѳ�ʵҵ","����ͥԷ","����·","������"]
	 *    },
	 *    {
	 *       "name":"�ҵĺ���",
	 *       "words":["��ܰ��","¹����","�ż���","�ܼ���","Ҷ����","������"]
	 *    }]
	 * }</pre>
	 **/
	static const int CMD_UPLOAD_LEXICON = 11;

	/**
	 * �ϴ���־��paramsΪJSON��ʽ����־��
	 **/
	static const int CMD_SEND_LOG = 12;

	/**
	 * ͬ��������
	 **/
	static const int CMD_SYNC = 13;

	/**
	 * ��ʼ�������ݡ�paramsΪ��ֵ�Բ���������ָ���������ͣ�ʾ����
	 * data_type=raw_audio����ʱֻ֧��ԭʼ��Ƶһ�����͵����ݡ�
	 **/
	static const int CMD_START_SAVE = 14;

	/**
	 * ֹͣ�������ݡ�paramsΪ��ֵ�Բ���������ָ���������ͣ�ʾ����
	 * data_type="raw_audio"����ʱֻ֧��ԭʼ��Ƶһ�����͵����ݡ�
	 **/
	static const int CMD_STOP_SAVE = 15;

	/**
	 * ����ʶ���﷨��paramsΪbnf�﷨���ݡ�
	 **/
	static const int CMD_BUILD_GRAMMAR = 16;

	/**
	 * ���±��شʵ䣬�����ڵ��ù�CMD_BUILD_GRAMMAR֮����á�
	 * paramsΪJSON�ַ���������ָ���ʱ����ƺ����ݣ�ʾ����<pre>
	 * {
	 *   "name":"<contact>",  // �ʱ�����
	 *   "content":"����\n����\n" //�ʱ�����
	 * }<pre>
	 **/
	static const int CMD_UPDATE_LOCAL_LEXICON = 17;

	/**
	 * ��ʼ�׳�ʶ����Ƶ������֮���׳�EVENT_AUDIO�¼���
	 * arg1Ϊʰ������������ã�����ǰδ�������ʹ�øò���ʰ�������Ѿ����ڻ���״̬��arg1�������á�
	 **/
	static const int CMD_START_THROW_AUDIO = 18;

	/**
	 * ֹͣ�׳�ʶ����Ƶ������֮��ֹͣ�׳�EVENT_AUDIO�¼���
	 **/
	static const int CMD_STOP_THROW_AUDIO = 19;

	/**
	 * ȷ���ƶ˷��ؽ������Ч�ԣ������Ӻ����ߣ��ڵõ����֮��5��ʱ���ڷ�����Ч��
	 */
	static const int CMD_RESULT_VALIDATION_ACK = 20;

	/**
	 * ����ƶ�����Ի���ʷ��
	 */
	static const int CMD_CLEAN_DIALOG_HISTORY = 21;

	/**
	 * ��ʼ¼�ƣ���Ҫ��params��ͨ��data_type�ֶ�ָ��¼�Ƶ��������͡�
	 */
	static const int CMD_START_RECORD = 22;

	/**
	 * ֹͣ¼�ƣ���Ҫ��params��ͨ��data_type�ֶ�ָ��ֹͣ¼�Ƶ��������͡�
	 */
	static const int CMD_STOP_RECORD = 23;

	/**
	 * ��ѯ����ͬ��״̬����Ҫ��params��ͨ��sid�ֶ�ָ��CMD_SYNC���ص�sid��
	 */
	static const int CMD_QUERY_SYNC_STATUS = 24;

	/**
	 * ��ѯ������paramsΪ��ǰAIUI������Ч������
	 */
	static const int CMD_QUERY_PARAMS = 25;

	/**
	 * ��̬����CAE������
	 */
	static const int CMD_SET_CAE_PARAMS = 26;

	/**
	 * TTS�ϳ����arg1��ʾ����Ĳ�����ȡֵ��START, PAUSE, RESUME, CANCEL��
	 */
	static const int CMD_TTS = 27;

	/**
	 * CAE��Ȩ��д���豸��Ϣ��
	 */
	static const int CMD_CAE_WRITE_DEVINFO = 1000;

	/**
	 * CAE��Ȩ��д����ܺ�����ġ�
	 */
	static const int CMD_CAE_WRITE_ENCRYPT = 1001;

	/*******************����keyֵ**********************/
	/**
	 * Ӧ��Ψһ��ʶ������<a href="http://www.xfyun.cn/">������ƽ̨</a>�������8λӦ��Id��
	 */
	static const char * const KEY_APPID;

	/**
	 * �û�Ψһ��ʶ��
	 */
	static const char * const KEY_UID;

	/**
	 * ���ӵķ�������ַ��
	 */
	static const char * const KEY_SERVER_URL;

	/**
	 * AIUI�����ϴ���ַ��
	 */
	static const char * const KEY_AIUI_UP_URL;

	/**
	 * AIUI���ͽ���ַ�����·�����ķ�������ַ��
	 */
	static const char * const KEY_AIUI_PUSHNODE_URL;

	/**
	 * ��ȡchid�ķ�������ַ��
	 */
	static const char * const KEY_AIUI_CHID_URL;

	/**
	 * ������ʱ����λ��ms���������Ѻ�һ��ʱ������Ч��������ߡ�
	 * ȡֵ��[10000,180000)��
	 */
	static const char * const KEY_INTERACT_TIMEOUT;

	/**
	 * ���糬ʱʱ�䣨��λ��ms����
	 * {@hide}
	 */
	static const char * const KEY_NETWORK_TIMEOUT;

	/**
	 * �����ʱʱ�䣨��λ��ms��������⵽��Ƶ��˵��һ��ʱ�����޽����ʱ��Ĭ��ֵ��5000��
	 */
	static const char * const KEY_RESULT_TIMEOUT;

	/**
	 * VAD���أ�ȡֵ��0���رգ���1���򿪣���
	 */
	static const char * const KEY_VAD_ENABLE;

	/**
	 * VAD�������á�ȡֵ��meta��ģ��VAD����fix_front������VAD����Ĭ��ֵ��meta��
	 */
	static const char * const KEY_VAD_TYPE;

	/**
	 * VADǰ�˵㳬ʱ����λ��ms����ȡֵ��Χ��[1000, 10000]��Ĭ��ֵ��5000��
	 */
	static const char * const KEY_VAD_BOS;

	/**
	 * VAD��˵㳬ʱ����λ��ms����ȡֵ��Χ��[0, 10000]��Ĭ��ֵ��1000.
	 */
	static const char * const KEY_VAD_EOS;

	/**
	 * �ƶ�VAD��˵㳬ʱ����λ��ms����ȡֵ��Χ��[0, 10000]��Ĭ��ֵ��400.
	 */
	static const char * const KEY_CLOUD_VAD_EOS;

	/**
	 * �������á��ݲ�֧�֡�
	 */
	static const char * const KEY_LANGUAGE;

	/**
	 * �������á��ݲ�֧�֡�
	 */
	static const char * const KEY_ACCENT;

	/**
	 * ʶ����������á��ݲ�֧�֡�
	 */
	static const char * const KEY_ASR_PTT;

	/**
	 * �������͡�
	 */
	static const char * const KEY_ENGINE_TYPE;

	/**
	 * ��Դ���ͣ�ȡֵ��path��sdcard�µ���Դ����assets��assets�µ���Դ��
	 */
	static const char * const KEY_RES_TYPE;

	/**
	 * ��Դ��ַ��
	 */
	static const char * const KEY_RES_PATH;

	/**
	 * IVW threshold value
	 */
	static const char * const KEY_IVW_THRESHOLD;

	/**
	 * IVW sst value
	 */
	static const char * const KEY_IVW_SST;

	/**
	 * ��ǲ�����
	 */
	static const char * const KEY_TAG;

	/**
	 * ҵ�񳡾���
	 */
	static const char * const KEY_SCENE;

	/**
	 * �������ͣ�ȡֵ��text��raw_audio��audio��image��video��
	 */
	static const char * const KEY_DATA_TYPE;

	/**
	 * ������Դ��ȡֵ��user���ⲿд�룩��sdk��SDK�ڲ�����
	 */
	static const char * const KEY_DATA_SOURCE;

	/**
	 * ��Ƶ�����ʣ���λ��Hz����ȡֵ��8000��16000��
	 */
	static const char * const KEY_SAMPLE_RATE;

	/**
	 * ����ʶ���û����Ի����ݣ���������ָ�������ʶ��Ч����
	 */
	static const char * const KEY_IAT_USER_DATA;

	/**
	 * ��������û����Ի����ݣ�����������ҵ����ʹ�á�
	 */
	static const char * const KEY_NLP_USER_DATA;

	/**
	 * ����ģʽ��ÿһ��ҵ���й���ģʽ���á�<p>
	 * ����ҵ��Ĺ���ģʽ��rec_only����ʹ�õ���˷�����¼������intent����ͼģʽ������Ƶ���д���������ͼ�����������Ĭ��ֵ��intent��</p>
	 */
	static const char * const KEY_WORK_MODE;

	/**
	 * ����ģʽ��ȡֵ��ivw��һ�㻽�ѣ���cae�����л��ѣ���off���رգ���
	 */
	static const char * const KEY_WAKEUP_MODE;

	/**
	 * ������ͼ���������ͣ�ȡֵ��local�����ߴ����������﷨ʶ�𣩡�cloud���ƶ˴��������壩��mixed����Ϸ�ʽ����
	 */
	static const char * const KEY_INTENT_ENGINE_TYPE;

	/*
	 * ����ģʽ��
	 */
	static const char * const KEY_INTERACT_MODE;

	/**
	 * ���ޡ�
	 */
	static const char * const KEY_THRESHOLD;

	/**
	 * ���ơ�
	 */
	static const char * const KEY_NAME;

	/**
	 * �����ֶΡ�
	 */
	static const char * const KEY_CONTENT;

	/**
	 * ����·����
	 */
	static const char * const KEY_BUILD_PATH;

	/**
	 * �﷨id��
	 */
	static const char * const KEY_GRAMMAR_ID;

	/**
	 * �ʱ�id��
	 */
	static const char * const KEY_LEXICON_ID;

	/**
	 * CAE������
	 */
	static const char * const KEY_LIB_CAE;

	/**
	 * CAE�������á�
	 */
	static const char * const KEY_CAE_PARAMS;
	/*******************����ȡֵ**********************/
	/**
	 * sdcard��Դ���͡�
	 */
	static const char * const RES_TYPE_PATH;

	/**
	 * assets��Դ���͡�
	 */
	static const char * const RES_TYPE_ASSETS;

	/**
	 * �������档
	 */
	static const char * const ENGINE_TYPE_CLOUD;

	/**
	 * �������档
	 */
	static const char * const ENGINE_TYPE_LOCAL;

	/**
	 * ͬʱʹ�����ߡ��������档
	 */
	static const char * const ENGINE_TYPE_MIXED;

	/**
	 * ¼��ģʽ��
	 */
	static const char * const WORK_MODE_REC_ONLY;

	/**
	 * ��ͼģʽ��
	 */
	static const char * const WORK_MODE_INTENT;

	/**
	 * һ�ν������������⼴��һ�λ��ѣ�һ�ν�������
	 */
	static const char * const INTERACT_MODE_ONESHOT;

	/**
	 * ����������������������һ�λ��ѣ���ν�������
	 */
	static const char * const INTERACT_MODE_CONTINUOUS;

	static const char * const AUTO;

	static const char * const USER;

	static const char * const KEY_CLEAN_DIALOG_HISTORY;

	/*******************alsa����**********************/
	/**
	 * �����豸�ţ���ʵ�������д��
	 */
	static const char * const KEY_SOUND_CARD;

	/**
	 * ���������ʣ���λ��Hz����
	 */
	static const char * const KEY_CARD_SAMPLE_RATE;

	/*******************log����**********************/
	/**
	 * debug��־���أ�ȡֵ��0���رգ���1���򿪣���
	 */
	static const char * const KEY_DEBUG_LOG;

	/**
	 * �Ƿ񱣴�data��־��ȡֵ��0���񣩣�1���ǣ���
	 */
	static const char * const KEY_SAVE_DATALOG;

	/**
	 * data��־�ı���Ŀ¼��������"/"��β��
	 */
	static const char * const KEY_DATALOG_PATH;

	/**
	 * ԭʼ��Ƶ����Ŀ¼��������"/"��β��
	 */
	static const char * const KEY_RAW_AUDIO_PATH;

	/*******************�����붨��**********************/
	/**
	 * �����ɹ���
	 */
	static const int SUCCESS								= 0;
	
	static const int NLP_SUCCESS_NO_DATA					= 3;
	/**
	 * ����ʧ�ܡ�
	 */
	static const int FAIL									= -1;

	/*******************��������**********************/
	/**
	 * VAD��Ϣ���ͣ�ǰ�˵㡣
	 */
	static const int VAD_BOS = 0;

	/**
	 * VAD��Ϣ���ͣ�������
	 */
	static const int VAD_VOL = 1;

	/**
	 * VAD��Ϣ���ͣ���˵㡣
	 */
	static const int VAD_EOS = 2;

	/**
	 * VAD��Ϣ���ͣ�ǰ�˵㳬ʱ��
	 */
	static const int VAD_BOS_TIMEOUT = 3;

	/**
	 * ��SDK�ڲ��Զ��׳����¼���
	 */
	static const int TYPE_AUTO = 0;

	/**
	 * ���ⲿ���������׳����¼���
	 */
	static const int TYPE_COMPEL = 1;

	/**
	 * ͬ��״̬���ݡ�
	 */
	static const int SYNC_DATA_STATUS = 0;

	/**
	 * ͬ�����Ի����ݡ�
	 */
	static const int SYNC_DATA_INDIVIDUAL = 1;

	/**
	 * �������˺Ź������ݡ�
	 */
	static const int SYNC_DATA_ACCOUNT = 2;

	/**
	 * ͬ���ŵ��ȸ��Ի����ݡ�
	 */
	static const int SYNC_DATA_SCHEMA = 3;

	/**
	 * ��ѯ����ͬ��״̬��
	 */
	static const int SYNC_DATA_QUERY = 4;

	/**
	 * ��������˵���ݡ�
	 */
	static const int SYNC_DATA_SPEAKABLE = 5;

	static const char* const EMPTY_STRING;

	static const int TTS_SPEAK_BEGIN = 1;

	static const int TTS_SPEAK_PAUSED = 2;

	static const int TTS_SPEAK_RESUMED = 3;

	static const int TTS_SPEAK_PROGRESS = 4;

	static const int TTS_SPEAK_COMPLETED = 5;

	static const int START = 1;

	static const int PAUSE = 2;

	static const int RESUME = 3;

	static const int CANCEL = 4;
};

}

#endif /* AIUICONSTANT_H_ */
