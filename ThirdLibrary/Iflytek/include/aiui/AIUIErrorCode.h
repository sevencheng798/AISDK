/*
 * AIUIErrorCode.h
 *
 *  Created on: 2017��2��17��
 *      Author: AIUI����ƽ̨��https://aiui.xfyun.cn/��
 */

#ifndef AIUIERRORCODE_H_
#define AIUIERRORCODE_H_

#include "AIUICommon.h"

namespace aiui {

class AIUIEXPORT AIUIErrorCode
{
public:
	/*
	 * 10100 - 19999������
	 * 1��ͷ�Ĵ�������MSC�ϱ�������������MSC/MSS/����
	 * ������ش��������ԭ����Խ���msc.log�ͽṹ����־.
	 */
	static const int SUCCESS									= 0;
	static const int FAIL										= -1;

	static const int MSP_SUCCESS								= 0;
	static const int MSP_ERROR_FAIL							= -1;
	static const int MSP_ERROR_EXCEPTION						= -2;

	/* General errors 10100(0x2774) */
	static const int MSP_ERROR_GENERAL							= 10100; 	/* 0x2774 */
	static const int MSP_ERROR_OUT_OF_MEMORY					= 10101; 	/* 0x2775 */
	static const int MSP_ERROR_FILE_NOT_FOUND					= 10102; 	/* 0x2776 */
	static const int MSP_ERROR_NOT_SUPPORT						= 10103; 	/* 0x2777 */
	static const int MSP_ERROR_NOT_IMPLEMENT					= 10104; 	/* 0x2778 */
	static const int MSP_ERROR_ACCESS							= 10105; 	/* 0x2779 */
	static const int MSP_ERROR_INVALID_PARA					= 10106; 	/* 0x277A */  /* ȱ�ٲ��� */
	static const int MSP_ERROR_INVALID_PARA_VALUE				= 10107; 	/* 0x277B */  /* ��Ч����ֵ */
	static const int MSP_ERROR_INVALID_HANDLE					= 10108; 	/* 0x277C */
	static const int MSP_ERROR_INVALID_DATA					= 10109; 	/* 0x277D */
	static const int MSP_ERROR_NO_LICENSE						= 10110; 	/* 0x277E */  /* ������Ȩ���� */
	static const int MSP_ERROR_NOT_INIT						= 10111; 	/* 0x277F */  /* ����δ��ʼ��,������������� */
	static const int MSP_ERROR_NULL_HANDLE						= 10112; 	/* 0x2780 */
	static const int MSP_ERROR_OVERFLOW						= 10113; 	/* 0x2781 */  /* ���û���ģ����������(10��); */
																	  /* ֻ�����ڲ���ʱ��һ���û����в���ע�� */
	static const int MSP_ERROR_TIME_OUT						= 10114; 	/* 0x2782 */  /* ��ʱ */
	static const int MSP_ERROR_OPEN_FILE						= 10115; 	/* 0x2783 */
	static const int MSP_ERROR_NOT_FOUND						= 10116; 	/* 0x2784 */  /* ���ݿ���ģ�Ͳ����� */
	static const int MSP_ERROR_NO_ENOUGH_BUFFER				= 10117; 	/* 0x2785 */
	static const int MSP_ERROR_NO_DATA							= 10118; 	/* 0x2786 */  /* �ӿͻ��˶���Ƶ�������λ�ȡ���ʱ������ */
	static const int MSP_ERROR_NO_MORE_DATA					= 10119; 	/* 0x2787 */
	static const int MSP_ERROR_NO_RESPONSE_DATA				= 10120; 	/* 0x2788 */
	static const int MSP_ERROR_ALREADY_EXIST					= 10121; 	/* 0x2789 */  /* ���ݿ���ģ���Ѵ��� */
	static const int MSP_ERROR_LOAD_MODULE						= 10122; 	/* 0x278A */
	static const int MSP_ERROR_BUSY							= 10123; 	/* 0x278B */
	static const int MSP_ERROR_INVALID_CONFIG					= 10124; 	/* 0x278C */
	static const int MSP_ERROR_VERSION_CHECK                 	= 10125; 	/* 0x278D */
	static const int MSP_ERROR_CANCELED						= 10126; 	/* 0x278E */
	static const int MSP_ERROR_INVALID_MEDIA_TYPE				= 10127; 	/* 0x278F */
	static const int MSP_ERROR_CONFIG_INITIALIZE				= 10128; 	/* 0x2790 */
	static const int MSP_ERROR_CREATE_HANDLE					= 10129; 	/* 0x2791 */
	static const int MSP_ERROR_CODING_LIB_NOT_LOAD				= 10130; 	/* 0x2792 */
	static const int MSP_ERROR_USER_CANCELLED					= 10131; 	/* 0x2793 */
	static const int MSP_ERROR_INVALID_OPERATION				= 10132; 	/* 0x2794 */
	static const int MSP_ERROR_MESSAGE_NOT_COMPLETE			= 10133;	/* 0x2795 */   /* flash */
	static const int MSP_ERROR_NO_EID							= 10134;	/* 0x2795 */
	static const int MSP_ERROE_OVER_REQ                      	= 10135;    /* 0x2797 */   /* client Redundancy request */
	static const int MSP_ERROR_USER_ACTIVE_ABORT             	= 10136;    /* 0x2798 */   /* user abort */
	static const int MSP_ERROR_BUSY_GRMBUILDING              	= 10137;    /* 0x2799 */
	static const int MSP_ERROR_BUSY_LEXUPDATING              	= 10138;    /* 0x279A */
	static const int MSP_ERROR_SESSION_RESET	                = 10139;    /* 0x279B */   /* msc������ֹ�Ự��׼���ش� */
	static const int MSP_ERROR_BOS_TIMEOUT                   	= 10140;    /* 0x279C */   /* VADǰ�˵㳬ʱ */
	static const int MSP_ERROR_STREAM_FILTER					= 10141;	/* 0X279D */   /* AIUI��ǰStream������ */
	static const int MSP_ERROR_STREAM_CLEAR				    = 10142;    /* 0X279E */   /* AIUI��ǰStream������ */
	static const int MSP_ERROR_NLP_TIMEOUT 					= 10146;    /* ��������ʱ */
	static const int MSP_ERROR_TPP_TIMEOUT						= 10147;	/* ������ó�ʱ */

	/* Error codes of network 10200(0x27D8)*/
	static const int MSP_ERROR_NET_GENERAL						= 10200; 	/* 0x27D8 */
	static const int MSP_ERROR_NET_OPENSOCK         			= 10201; 	/* 0x27D9 */   /* Open socket */
	static const int MSP_ERROR_NET_CONNECTSOCK      			= 10202; 	/* 0x27DA */   /* Connect socket */
	static const int MSP_ERROR_NET_ACCEPTSOCK       			= 10203; 	/* 0x27DB */   /* Accept socket */
	static const int MSP_ERROR_NET_SENDSOCK         			= 10204; 	/* 0x27DC */   /* Send socket data */
	static const int MSP_ERROR_NET_RECVSOCK         			= 10205; 	/* 0x27DD */   /* Recv socket data */
	static const int MSP_ERROR_NET_INVALIDSOCK      			= 10206; 	/* 0x27DE */   /* Invalid socket handle */
	static const int MSP_ERROR_NET_BADADDRESS       			= 10207; 	/* 0x27EF */   /* Bad network address */
	static const int MSP_ERROR_NET_BINDSEQUENCE     			= 10208; 	/* 0x27E0 */   /* Bind after listen/connect */
	static const int MSP_ERROR_NET_NOTOPENSOCK      			= 10209; 	/* 0x27E1 */   /* Socket is not opened */
	static const int MSP_ERROR_NET_NOTBIND         			= 10210; 	/* 0x27E2 */   /* Socket is not bind to an address */
	static const int MSP_ERROR_NET_NOTLISTEN        			= 10211; 	/* 0x27E3 */   /* Socket is not listening */
	static const int MSP_ERROR_NET_CONNECTCLOSE     			= 10212; 	/* 0x27E4 */   /* The other side of connection is closed */
	static const int MSP_ERROR_NET_NOTDGRAMSOCK     			= 10213; 	/* 0x27E5 */   /* The socket is not datagram type */
	static const int MSP_ERROR_NET_DNS     					= 10214; 	/* 0x27E6 */   /* domain name is invalid or dns server does not function well */
	static const int MSP_ERROR_NET_INIT     					= 10215; 	/* 0x27E7 */   /* ssl ctx create failed */

	/*nfl error*/
	static const int MSP_ERROR_NFL_INNER_ERROR               	= 10216;   /* NFL inner error */
	static const int MSP_ERROR_MSS_TIME_OUT                  	= 10217;/* MSS TIMEOUT */
	static const int MSP_ERROT_CLIENT_TIME_OUT               	= 10218;    /* CLIENT TIMEOUT */
	static const int MSP_ERROR_CLIENT_CLOSE                  	= 10219;    /* CLIENT CLOSED CONNECTION */

	static const int MSP_ERROR_CLIENT_AREA_CHANGE				= 10220;
	static const int MSP_ERROR_NET_SSL_HANDSHAKE				= 10221;
	static const int MSP_ERROR_NET_INVALID_ROOT_CERT			= 10222;
	static const int MSP_ERROR_NET_INVALID_CLIENT_CERT			= 10223;
	static const int MSP_ERROR_NET_INVALID_SERVER_CERT			= 10224;
	static const int MSP_ERROR_NET_INVALID_KEY					= 10225;


	/* Error codes of mssp message 10300(0x283C) */
	static const int MSP_ERROR_MSG_GENERAL						= 10300; 	/* 0x283C */
	static const int MSP_ERROR_MSG_PARSE_ERROR					= 10301; 	/* 0x283D */
	static const int MSP_ERROR_MSG_BUILD_ERROR					= 10302; 	/* 0x283E */
	static const int MSP_ERROR_MSG_PARAM_ERROR					= 10303; 	/* 0x283F */
	static const int MSP_ERROR_MSG_CONTENT_EMPTY				= 10304; 	/* 0x2840 */
	static const int MSP_ERROR_MSG_INVALID_CONTENT_TYPE		= 10305; 	/* 0x2841 */
	static const int MSP_ERROR_MSG_INVALID_CONTENT_LENGTH		= 10306; 	/* 0x2842 */
	static const int MSP_ERROR_MSG_INVALID_CONTENT_ENCODE		= 10307; 	/* 0x2843 */
	static const int MSP_ERROR_MSG_INVALID_KEY					= 10308; 	/* 0x2844 */
	static const int MSP_ERROR_MSG_KEY_EMPTY					= 10309; 	/* 0x2845 */
	static const int MSP_ERROR_MSG_SESSION_ID_EMPTY			= 10310; 	/* 0x2846 */
	static const int MSP_ERROR_MSG_LOGIN_ID_EMPTY				= 10311; 	/* 0x2847 */
	static const int MSP_ERROR_MSG_SYNC_ID_EMPTY				= 10312; 	/* 0x2848 */
	static const int MSP_ERROR_MSG_APP_ID_EMPTY				= 10313; 	/* 0x2849 */
	static const int MSP_ERROR_MSG_EXTERN_ID_EMPTY				= 10314; 	/* 0x284A */
	static const int MSP_ERROR_MSG_INVALID_CMD					= 10315; 	/* 0x284B */
	static const int MSP_ERROR_MSG_INVALID_SUBJECT				= 10316; 	/* 0x284C */
	static const int MSP_ERROR_MSG_INVALID_VERSION				= 10317; 	/* 0x284D */
	static const int MSP_ERROR_MSG_NO_CMD						= 10318; 	/* 0x284E */
	static const int MSP_ERROR_MSG_NO_SUBJECT					= 10319; 	/* 0x284F */
	static const int MSP_ERROR_MSG_NO_VERSION					= 10320; 	/* 0x2850 */
	static const int MSP_ERROR_MSG_MSSP_EMPTY					= 10321; 	/* 0x2851 */
	static const int MSP_ERROR_MSG_NEW_RESPONSE				= 10322; 	/* 0x2852 */
	static const int MSP_ERROR_MSG_NEW_CONTENT					= 10323; 	/* 0x2853 */
	static const int MSP_ERROR_MSG_INVALID_SESSION_ID			= 10324; 	/* 0x2854 */
	static const int MSP_ERROR_MSG_INVALID_CONTENT				= 10325; 	/* 0x2855 */

	/* Error codes of DataBase 10400(0x28A0)*/
	static const int MSP_ERROR_DB_GENERAL					= 10400; 	/* 0x28A0 */
	static const int MSP_ERROR_DB_EXCEPTION					= 10401; 	/* 0x28A1 */
	static const int MSP_ERROR_DB_NO_RESULT					= 10402; 	/* 0x28A2 */
	static const int MSP_ERROR_DB_INVALID_USER				= 10403; 	/* 0x28A3 */
	static const int MSP_ERROR_DB_INVALID_PWD				= 10404; 	/* 0x28A4 */
	static const int MSP_ERROR_DB_CONNECT					= 10405; 	/* 0x28A5 */
	static const int MSP_ERROR_DB_INVALID_SQL				= 10406; 	/* 0x28A6 */
	static const int MSP_ERROR_DB_INVALID_APPID				= 10407;	/* 0x28A7 */
	static const int MSP_ERROR_DB_NO_UID						= 10408;


	/* Error codes of Resource 10500(0x2904)*/
	static const int MSP_ERROR_RES_GENERAL					= 10500; 	/* 0x2904 */
	static const int MSP_ERROR_RES_LOAD          			= 10501; 	/* 0x2905 */   /* Load resource */
	static const int MSP_ERROR_RES_FREE          			= 10502; 	/* 0x2906 */   /* Free resource */
	static const int MSP_ERROR_RES_MISSING       			= 10503; 	/* 0x2907 */   /* Resource File Missing */
	static const int MSP_ERROR_RES_INVALID_NAME  			= 10504; 	/* 0x2908 */   /* Invalid resource file name */
	static const int MSP_ERROR_RES_INVALID_ID    			= 10505; 	/* 0x2909 */   /* Invalid resource ID */
	static const int MSP_ERROR_RES_INVALID_IMG   			= 10506; 	/* 0x290A */   /* Invalid resource image pointer */
	static const int MSP_ERROR_RES_WRITE         			= 10507; 	/* 0x290B */   /* Write read-only resource */
	static const int MSP_ERROR_RES_LEAK          			= 10508; 	/* 0x290C */   /* Resource leak out */
	static const int MSP_ERROR_RES_HEAD          			= 10509; 	/* 0x290D */   /* Resource head currupt */
	static const int MSP_ERROR_RES_DATA          			= 10510; 	/* 0x290E */   /* Resource data currupt */
	static const int MSP_ERROR_RES_SKIP          			= 10511; 	/* 0x290F */   /* Resource file skipped */

	/* Error codes of TTS 10600(0x2968)*/
	static const int MSP_ERROR_TTS_GENERAL					= 10600; 	/* 0x2968 */
	static const int MSP_ERROR_TTS_TEXTEND          			= 10601; 	/* 0x2969 */  /* Meet text end */
	static const int MSP_ERROR_TTS_TEXT_EMPTY				= 10602; 	/* 0x296A */  /* no synth text */
	static const int MSP_ERROR_TTS_LTTS_ERROR				= 10603; 	/* 0x296B */

	/* Error codes of Recognizer 10700(0x29CC) */
	static const int MSP_ERROR_REC_GENERAL					= 10700; 	/* 0x29CC */
	static const int MSP_ERROR_REC_INACTIVE					= 10701; 	/* 0x29CD */
	static const int MSP_ERROR_REC_GRAMMAR_ERROR				= 10702; 	/* 0x29CE */
	static const int MSP_ERROR_REC_NO_ACTIVE_GRAMMARS		= 10703; 	/* 0x29CF */
	static const int MSP_ERROR_REC_DUPLICATE_GRAMMAR			= 10704; 	/* 0x29D0 */
	static const int MSP_ERROR_REC_INVALID_MEDIA_TYPE		= 10705; 	/* 0x29D1 */
	static const int MSP_ERROR_REC_INVALID_LANGUAGE			= 10706; 	/* 0x29D2 */
	static const int MSP_ERROR_REC_URI_NOT_FOUND				= 10707; 	/* 0x29D3 */
	static const int MSP_ERROR_REC_URI_TIMEOUT				= 10708; 	/* 0x29D4 */
	static const int MSP_ERROR_REC_URI_FETCH_ERROR			= 10709; 	/* 0x29D5 */
	static const int MSP_ERROR_REC_PROC_MOD					= 10710;	/* 0x29D6 */

	/* Error codes of Speech Detector 10800(0x2A30) */
	static const int MSP_ERROR_EP_GENERAL					= 10800; 	/* 0x2A30 */
	static const int MSP_ERROR_EP_NO_SESSION_NAME            = 10801; 	/* 0x2A31 */
	static const int MSP_ERROR_EP_INACTIVE                   = 10802; 	/* 0x2A32 */
	static const int MSP_ERROR_EP_INITIALIZED                = 10803; 	/* 0x2A33 */

	/* Error codes of TUV */
	static const int MSP_ERROR_TUV_GENERAL					= 10900; 	/* 0x2A94 */
	static const int MSP_ERROR_TUV_GETHIDPARAM        		= 10901; 	/* 0x2A95 */   /* Get Busin Param huanid*/
	static const int MSP_ERROR_TUV_TOKEN      				= 10902; 	/* 0x2A96 */   /* Get Token */
	static const int MSP_ERROR_TUV_CFGFILE					= 10903; 	/* 0x2A97 */   /* Open cfg file */
	static const int MSP_ERROR_TUV_RECV_CONTENT              = 10904; 	/* 0x2A98 */   /* received content is error */
	static const int MSP_ERROR_TUV_VERFAIL      			    = 10905; 	/* 0x2A99 */   /* Verify failure */

	/* Error codes of IMTV */
	static const int MSP_ERROR_LOGIN_SUCCESS					= 11000; 	/* 0x2AF8 */   /* �ɹ� */
	static const int MSP_ERROR_LOGIN_NO_LICENSE        	    = 11001; 	/* 0x2AF9 */   /* ���ô����������û���Ҫ���� */
	static const int MSP_ERROR_LOGIN_SESSIONID_INVALID		= 11002; 	/* 0x2AFA */   /* SessionIdʧЧ����Ҫ���µ�¼ͨ��֤ */
	static const int MSP_ERROR_LOGIN_SESSIONID_ERROR			= 11003; 	/* 0x2AFB */   /* SessionIdΪ�գ����߷Ƿ� */
	static const int MSP_ERROR_LOGIN_UNLOGIN		  			= 11004; 	/* 0x2AFC */   /* δ��¼ͨ��֤ */
	static const int MSP_ERROR_LOGIN_INVALID_USER	  		= 11005; 	/* 0x2AFD */   /* �û�ID��Ч */
	static const int MSP_ERROR_LOGIN_INVALID_PWD		  		= 11006; 	/* 0x2AFE */   /* �û�������Ч */
	static const int MSP_ERROR_LOGIN_SYSTEM_ERROR            = 11099; 	/* 0x2B5B */   /* ϵͳ���� */

	/* Error codes of HCR */
	static const int MSP_ERROR_HCR_GENERAL					= 11100;
	static const int MSP_ERROR_HCR_RESOURCE_NOT_EXIST		= 11101;
	static const int MSP_ERROR_HCR_CREATE					= 11102;
	static const int MSP_ERROR_HCR_DESTROY					= 11103;
	static const int MSP_ERROR_HCR_START						= 11104;
	static const int MSP_ERROR_HCR_APPEND_STROKES			= 11105;
	static const int MSP_ERROR_HCR_INIT                      = 11106;
	static const int MSP_ERROR_HCR_POINT_DECODE              = 11107;
	static const int MSP_ERROR_HCR_DISPATCH                  = 11108;
	static const int MSP_ERROR_HCR_GETRESULT                 = 11109;
	static const int MSP_ERROR_HCR_RESOURCE		            = 11110;

	/* Error Codes using in local engine */
	static const int MSP_ERROR_AUTH_NO_LICENSE				= 11200;	/* 0x2BC0 */   /* ����Ȩ */
	static const int MSP_ERROR_AUTH_NO_ENOUGH_LICENSE		= 11201;	/* 0x2BC1 */   /* ��Ȩ���� */
	static const int MSP_ERROR_AUTH_INVALID_LICENSE		    = 11202;	/* 0x2BC2 */   /* ��Ч����Ȩ */
	static const int MSP_ERROR_AUTH_LICENSE_EXPIRED			= 11203;	/* 0x2BC3 */   /* ��Ȩ���� */
	static const int MSP_ERROR_AUTH_NEED_MORE_DATA           = 11204;    /* 0x2BC4 */   /* ���豸��Ϣ */
	static const int MSP_ERROR_AUTH_LICENSE_TO_BE_EXPIRED	= 11205;	/* 0x2BC5 */   /* ��Ȩ�������ڣ������Դ����� */
	static const int MSP_ERROR_AUTH_INVALID_MACHINE_ID       = 11206;    /* 0x2BC6 */   /* ��Ч�Ļ����� */
	//��������ֹʹ��
	static const int MSP_ERROR_AUTH_LOCAL_ASR_FORBIDDEN 	    = 11207;    /* 0x2BC7 */   /* ��ֹʹ�ñ���ʶ������ */
	static const int MSP_ERROR_AUTH_LOCAL_TTS_FORBIDDEN      = 11208;    /* 0x2BC8 */   /* ��ֹʹ�ñ��غϳ����� */
	static const int MSP_ERROR_AUTH_LOCAL_IVW_FORBIDDEN 		= 11209; 	/* 0x2BC8 */   /* ��ֹʹ�ñ��ػ������� */

	static const int MSP_ERROR_AUTH_APPID_NOT_MATCH 			= 11210;    /* ��Դappid��Ӧ��appid��ƥ�� */
	static const int	MSP_ERROR_AUTH_UID_NOT_MATCH 			= 11211;    /* ��Դuid�͵�¼�û�uid��ƥ�� */
	static const int MSP_ERROR_AUTH_TRIAL_EXPIRED			= 11212;    /* ������Դ���� */
	static const int MSP_ERROR_AUTH_ERROR_END 				= 11250; 	/* 0x2BC8 */   /* End ��Ȩ������*/

	static const int MSP_ERROR_AIUI_NO_ENOUGH_LICENSE 		= 11216;	/* AIUI��Ȩ���� */

	/*Error Codes of Authorization*/
	static const int MSP_ERROR_AUTH_DVC_NO_LICENSE				= 11300;
	static const int MSP_ERROR_AUTH_DVC_NO_ENOUGH_LICENSE		= 11301;
	static const int MSP_ERROR_AUTH_DVC_INVALID_LICENSE		    = 11302;
	static const int MSP_ERROR_AUTH_DVC_LICENSE_EXPIRED			= 11303;
	static const int MSP_ERROR_AUTH_DVC_NEED_MORE_DATA           = 11304;
	static const int MSP_ERROR_AUTH_DVC_LICENSE_TO_BE_EXPIRED	= 11305;
	static const int MSP_ERROR_AUTH_DVC_EXCEED_LICENSE			= 11306;


	/*Error codes of ISE */
	static const int MSP_ERROR_ASE_EXCEP_SILENCE  			= 11401;
	static const int MSP_ERROR_ASE_EXCEP_SNRATIO  			= 11402;
	static const int MSP_ERROR_ASE_EXCEP_PAPERDATA  			= 11403;
	static const int MSP_ERROR_ASE_EXCEP_PAPERCONTENTS 		= 11404;
	static const int MSP_ERROR_ASE_EXCEP_NOTMONO    			= 11405;
	static const int MSP_ERROR_ASE_EXCEP_OTHERS  			= 11406;
	static const int MSP_ERROR_ASE_EXCEP_PAPERFMT 			= 11407;
	static const int MSP_ERROR_ASE_EXCEP_ULISTWORD  			= 11408;

	/*Error Code Of ivw optimization */
	static const int MSP_ERROR_IVW_MODEL_TRAINING			= 11501;  /* ģ����Դ���������� */
	static const int MSP_ERROR_IVW_MODEL_NO_FOUND			= 11502;  /* ����ģ����Դ */
	static const int MSP_ERROR_IVW_BUSY						= 11503;  /* ����æ */


	//-------------------------����-------------------------//
	/** �ں��쳣*/
	static const int MSP_ERROR_IVP_GENERAL					= 11600;
	/** rgn�������֧�ִ���9*/
	static const int MSP_ERROR_IVP_EXTRA_RGN_SOPPORT			= 11601;
	/** ��Ƶ���η���̫�󣬳���ϵͳ��Χ�������ط�*/
	static const int MSP_ERROR_IVP_TRUNCATED		    		= 11602;
	/** ̫������*/
	static const int MSP_ERROR_IVP_MUCH_NOISE				= 11603;
	/** ����̫С*/
	static const int MSP_ERROR_IVP_TOO_LOW          			= 11604;
	/** û��⵽��Ƶ*/
	static const int MSP_ERROR_IVP_ZERO_AUDIO				= 11605;
	/** ��Ƶ̫��*/
	static const int MSP_ERROR_IVP_UTTER_TOO_SHORT			= 11606;
	/** ��Ƶ����������ı���һ��*/
	static const int MSP_ERROR_IVP_TEXT_NOT_MATCH			= 11607;
	/** ��Ƶ���ﲻ������˵��Ҫ��*/
	static const int MSP_ERROR_IVP_NO_ENOUGH_AUDIO 		    = 11608;
	/** ����ģ��������hbase���Ҳ��� **/
	static const int MSP_ERROR_IVP_MODEL_NOT_FOUND_IN_HBASE	= 11610;
	/** �ƶ�����ģ�����ڸ���,5s���� **/
	static const int MSP_MODEL_NEED_UPDATE					= 10031;
	//-------------------------����-------------------------//

	//-------------------------����-------------------------//
	/** ������ */
	static const int MSP_ERROR_IFR_NOT_FACE_IMAGE            		= 11700;
	/** ���� */
	static const int MSP_ERROR_FACE_IMAGE_FULL_LEFT       		    = 11701;
	/** ���� */
	static const int MSP_ERROR_FACE_IMAGE_FULL_RIGHT      			= 11702;
	/** ˳ʱ����ת */
	static const int	MSP_ERROR_IMAGE_CLOCKWISE_WHIRL     			= 11703;
	/** ��ʱ����ת */
	static const int	MSP_ERROR_IMAGE_COUNTET_CLOCKWISE_WHIRL         = 11704;
	/** �ߴ����*/
	static const int	MSP_ERROR_VALID_IMAGE_SIZE                      = 11705;
	/** �����쳣 */
	static const int	MSP_ERROR_ILLUMINATION                          = 11706;
	/** �������ڵ� */
	static const int	MSP_ERROR_FACE_OCCULTATION                      = 11707;
	/** �Ƿ�ģ������ */
	static const int	MSP_ERROR_FACE_INVALID_MODEL                    = 11708;
	/** ����ģ��������hbase���Ҳ��� **/
	static const int MSP_ERROR_FACE_MODEL_NOT_FOUND_IN_HBASE			= 11712;
	//-------------------------����-------------------------//

	//-------------------------�ں���֤-------------------------//
	/** �����������ͷǷ� */
	static const int MSP_ERROR_FUSION_INVALID_INPUT_TYPE				= 11709;
	/** ��������ݲ����� */
	static const int MSP_ERROR_FUSION_NO_ENOUGH_DATA					= 11710;
	/** ��������ݹ��� */
	static const int MSP_ERROR_FUSION_ENOUGH_DATA					= 11711;
	//-------------------------�ں���֤-------------------------//

	//-------------------------������-------------------------//
	/** �鲻���ڣ�δ���� */
	static const int MSP_ERROR_NO_GROUP								= 10143;
	/** ���� */
	static const int MSP_ERROR_GROUP_EMPTY							= 10141;
	/** ����û�и��û� */
	static const int MSP_ERROR_NO_USER								= 10142;
	/** �û��������������޻����鴴�������������� */
	static const int MSP_ERROR_OVERFLOW_IN_GROUP						= 10144;
	//-------------------------������-------------------------//

	/* Error codes of http 12000(0x2EE0) */
	static const int MSP_ERROR_HTTP_BASE						= 12000;	/* 0x2EE0 */

	/*Error codes of ISV */
	static const int MSP_ERROR_ISV_NO_USER                   = 13000;	/* 32C8 */    /* the user doesn't exist */

	/* Error codes of Lua scripts */
	static const int MSP_ERROR_LUA_BASE						= 14000;    /* 0x36B0 */
	static const int MSP_ERROR_LUA_YIELD						= 14001;	/* 0x36B1 */
	static const int MSP_ERROR_LUA_ERRRUN					= 14002;	/* 0x36B2 */
	static const int MSP_ERROR_LUA_ERRSYNTAX					= 14003;	/* 0x36B3 */
	static const int MSP_ERROR_LUA_ERRMEM					= 14004;	/* 0x36B4 */
	static const int MSP_ERROR_LUA_ERRERR					= 14005;	/* 0x36B5 */
	static const int MSP_ERROR_LUA_INVALID_PARAM				= 14006;	/* 0x36B6 */

	/* Error codes of MMP */
	static const int MSP_ERROR_MMP_BASE						= 15000;    /* 0x3A98 */
	static const int MSP_ERROR_MMP_MYSQL_INITFAIL			= 15001;	/* 0x3A99 */
	static const int MSP_ERROR_MMP_REDIS_INITFAIL			= 15002;	/* 0x3A9A */
	static const int MSP_ERROR_MMP_NETDSS_INITFAIL			= 15003;	/* 0x3A9B */
	static const int MSP_ERROR_MMP_TAIR_INITFAIL				= 15004;	/* 0x3A9C */
	static const int MSP_ERROR_MMP_MAIL_SESSION_FAIL			= 15006;	/* 0x3A9E */	/* �ʼ���½������ʱ���Ự����*/
	static const int MSP_ERROR_MMP_MAIL_LOGON_FAIL			= 15007;	/* 0x3A9F */	/* �ʼ���½������ʱ���ܾ���½��*/
	static const int MSP_ERROR_MMP_MAIL_USER_ILLEGAL			= 15008;	/* 0x3AA0 */	/* �ʼ���½������ʱ���û����Ƿ���*/
	static const int MSP_ERROR_MMP_MAIL_PWD_ERR				= 15009;	/* 0x3AA1 */	/* �ʼ���½������ʱ���������*/
	static const int MSP_ERROR_MMP_MAIL_SOCKET_ERR			= 15010;	/* 0x3AA2 */	/* �ʼ����͹������׽��ִ���*/
	static const int MSP_ERROR_MMP_MAIL_INIT_FAIL			= 15011;	/* 0x3AA3 */	/* �ʼ���ʼ������*/
	static const int MSP_ERROR_MMP_STORE_MNR_NO_INIT			= 15012;	/* 0x3AA4 */	/* store_managerδ��ʼ�������ʼ��ʧ��*/
	static const int MSP_ERROR_MMP_STORE_MNR_POOL_FULL		= 15013;	/* 0x3AA5 */	/* store_manager�����ӳ�����*/
	static const int MSP_ERROR_MMP_STRATGY_PARAM_ILLEGAL		= 15014;	/* 0x3AA6 */	/* �������Ա��ʽ�Ƿ�*/
	static const int MSP_ERROR_MMP_STRATGY_PARAM_TOOLOOG		= 15015;	/* 0x3AA7 */	/* �������Ա��ʽ̫��*/
	static const int MSP_ERROR_MMP_PARAM_NULL				= 15016;	/* 0x3AA8 */	/* ��������Ϊ��*/
	static const int MSP_ERROR_MMP_ERR_MORE_TOTAL			= 15017;	/* 0x3AA9 */	/* pms�������ݿ��д�����ܱ�����ݣ�������� > �ܴ�����*/
	static const int MSP_ERROR_MMP_PROC_THRESHOLD			= 15018;	/* 0x3AAA */	/* ���̼�ط�ֵ���ô���*/
	static const int MSP_ERROR_MMP_SERVER_THRESHOLD			= 15019;	/* 0x3AAB */	/* ��������ط�ֵ���ô���*/
	static const int MSP_ERROR_MMP_PYTHON_NO_EXIST			= 15020;    /* 0x3AAC */	/* python�ű��ļ������� */
	static const int MSP_ERROR_MMP_PYTHON_IMPORT_FAILED		= 15021;	/* 0x3AAD */	/* python�ű�������� */
	static const int MSP_ERROR_MMP_PYTHON_BAD_FUNC			= 15022;	/* 0x3AAE */	/* python�ű�������ʽ���� */
	static const int MSP_ERROR_MMP_DB_DATA_ILLEGAL			= 15023;	/* 0x3AAF */	/* �������ݿ��е����ݸ�ʽ���� */
	static const int MSP_ERROR_MMP_REDIS_NOT_CONN			= 15024;	/* 0x3AB0 */	/* redisû�����ӵ������ */
	static const int MSP_ERROR_MMP_PMA_NOT_FOUND_STRATEGY	= 15025;	/* 0x3AB1 */	/* û���ҵ��������� */
	static const int MSP_ERROR_MMP_TAIR_CONNECT				= 15026;	/* 0x3AB2 */	/* ����tair��Ⱥʧ�� */
	static const int MSP_ERROR_MMP_PMC_SERVINFO_INVALID		= 15027;	/* Ox3AB3 */	/* ��pmc�ķ�������Ϣ�Ѿ���Ч */
	static const int MSP_ERROR_MMP_ALARM_GROUP_NULL			= 15028;	/* Ox3AB4 */	/* �����������Ķ��ű��������ʼ��������Ϊ�� */
	static const int MSP_ERROR_MMP_ALARM_CONTXT_NULL			= 15029;	/* Ox3AB5 */	/* �����������ı�������Ϊ�� */

	/* Error codes of MSC(lmod loader) */
	static const int MSP_ERROR_LMOD_BASE						= 16000;	/* 0x3E80 */
	static const int MSP_ERROR_LMOD_NOT_FOUND				= 16001;	/* 0x3E81 */	/* û�ҵ�lmod�ļ� */
	static const int MSP_ERROR_LMOD_UNEXPECTED_BIN			= 16002;	/* 0x3E82 */	/* ��Ч��lmod */
	static const int MSP_ERROR_LMOD_LOADCODE					= 16003;	/* 0x3E83 */	/* ����lmodָ��ʧ�� */
	static const int MSP_ERROR_LMOD_PRECALL					= 16004;	/* 0x3E84 */	/* ��ʼ��lmodʧ�� */
	static const int MSP_ERROR_LMOD_RUNTIME_EXCEPTION		= 16005;	/* 0x3E85 */	/* lmod����ʱ�쳣 */
	static const int MSP_ERROR_LMOD_ALREADY_LOADED			= 16006;	/* 0x3E86 */	/* lmod�ظ����� */

	// Error code of Third Business
	static const int MSP_ERROR_BIZ_BASE						= 17000;	/* 0x4268 */	/* lmod�ظ����� */

	//Error of Nginx errlog file increase exception
	static const int MSP_ERROR_NGX_LOG_MORE_TOTEL_SIZE       = 18000;

	// Error Code Of Speech plus
#undef ERROR_NO_NETWORK
	static const int ERROR_NO_NETWORK 				               = 20001;     /* ����Ч����������*/
	static const int ERROR_NETWORK_TIMEOUT 			               = 20002;     /* �������ӳ�ʱ*/
	static const int ERROR_NET_EXCEPTION				               = 20003; /* �����쳣*/ 
	static const int ERROR_INVALID_RESULT			                   = 20004; /* ����Ч�Ľ��*/
#undef ERROR_NO_MATCH
	static const int ERROR_NO_MATCH 					               = 20005; /* ��ƥ���� */
	static const int ERROR_AUDIO_RECORD 				               = 20006; /* ¼��ʧ�� */
	static const int ERROR_NO_SPEECH 				                   = 20007; /* δ��⵽����*/

	static const int ERROR_SPEECH_TIMEOUT 			               = 20008; /* ��Ƶ���볬ʱ*/
	static const int ERROR_EMPTY_UTTERANCE 			               = 20009; /* ��Ч���ı����� */
	static const int ERROR_FILE_ACCESS 				               = 20010; /* �ļ���дʧ�� */
	static const int ERROR_PLAY_MEDIA 				               = 20011; /* ��Ƶ����ʧ�� */

	static const int ERROR_INVALID_PARAM 			                   = 20012; /* ��Ч�Ĳ���*/
	static const int ERROR_TEXT_OVERFLOW				               = 20013; /* �ı���� */
#undef ERROR_INVALID_DATA
	static const int ERROR_INVALID_DATA 				               = 20014; /* ��Ч���� */
	static const int ERROR_LOGIN						               = 20015; /* �û�δ��½*/
	static const int ERROR_PERMISSION_DENIED 		                   = 20016; /* ��Ч��Ȩ */
	static const int ERROR_INTERRUPT 				                   = 20017; /* ���쳣��� */

	static const int ERROR_VERSION_LOWER                             = 20018; /* �汾���� */
	static const int ERROR_SYSTEM_PREINSTALL                         = 20020; /*ϵͳԤ�ð汾 */
	static const int ERROR_UNSATISFIED_LINK							= 20021; /*δʵ�ֵ�Native��������*/
	static const int ERROR_UNKNOWN                                   = 20999; /* δ֪���� */

	/** û�а�װ������� */
	static const int ERROR_COMPONENT_NOT_INSTALLED                   = 21001;
	/** ���治֧�� */
	static const int ERROR_ENGINE_NOT_SUPPORTED                      = 21002;
	/** ��ʼ��ʧ�� */
	static const int ERROR_ENGINE_INIT_FAIL                          = 21003;
	/** ����ʧ�� */
	static const int ERROR_ENGINE_CALL_FAIL                          = 21004;
	/** ���深æ */
	static const int ERROR_ENGINE_BUSY	                            = 21005;
	/** ��������δ��ʼ�� */
	static const int ERROR_LOCAL_NO_INIT                             = 22001;
	/** ������������Դ */
	static const int ERROR_LOCAL_RESOURCE                            = 22002;
	/** ���������ڲ����� */
	static const int ERROR_LOCAL_ENGINE                              = 22003;
	/** ���ػ������汻�쳣��� */
	static const int ERROR_IVW_INTERRUPT 			                = 22004;

	/** �������binder�Ѿ����� */
	static const int ERROR_SERVICE_BINDER_DIED = 21020;

	/** IO�쳣 */
	static const int ERROR_IO_EXCEPTION = 21021;

	/** δ���ڹ���״̬ */
	static const int ERROR_NOT_WORKING = 21022;

	/*Error Code Of Local iflytek Engines*/

	/*Error Code Of AiTalk*/

	/*Error Code Of AiTalk Operation*/

	static const int ERROR_ASR_CLIENT                                = 23000; /*�ͻ���Ӧ�ó������*///?????????
	static const int ERROR_ASR_INVALID_PARA                          = 23001; /*��Ч�Ĳ���*/
	static const int ERROR_ASR_INVALID_PARA_VALUE                    = 23002; /*��Ч�Ĳ���ֵ*/
	static const int ERROR_ASR_OUT_OF_MEMORY                         = 23003; /*�ڴ�ľ�*/
	static const int ERROR_ASR_CREATE_HANDLE_FAILED                  = 23004; /*�������ʧ��*/
	static const int ERROR_ASR_ENGINE_INIT_FAILED                    = 23005; /*�����ʼ��ʧ��*/
	static const int ERROR_ASR_ENGINE_STARTED                        = 23006; /*�����Ѿ�����*/
	static const int ERROR_ASR_ENGINE_UNINIT                         = 23007; /*����δ��ʼ��*/
	static const int ERROR_ASR_SPEECH_TIMEOUT                        = 23008; /*ʶ��ʱ��VADû������û�м�⵽��˵㣩*/
	static const int ERROR_ASR_NO_RECOGNIZED_RESULT                  = 23009; /*��ʶ����*/
	static const int ERROR_ASR_INVALID_HANDLE                        = 23010;  /*��Ч�ľ��*/
	static const int ERROR_ASR_FILE_ACCESS                           = 23011;  /*���ļ�ʧ��*/

	/*Error Code Of AiTalk Engine*/
	static const int ERROR_AITALK_FALSE                              = 23100; // ivErr_FALSE               = 1

	/* For license check */
	static const int ERROR_AITALK_PERMISSION_DENIED                  = 23101; // ivErr_InvSN               = 2

	/* General */
	static const int ERROR_AITALK_INVALID_PARA                       = 23102; // ivErr_InvArg              = 3
	static const int ERROR_AITALK_BUFFER_OVERFLOW                    = 23103; // ivErr_BufferFull          = 4  /*��Ƶ���ݻ���������*/
	static const int ERROR_AITALK_FAILED                             = 23104; // ivErr_Failed              = 5
	static const int ERROR_AITALK_NOT_SUPPORTED                      = 23105; // ivErr_NotSupport          = 6  /*���治֧��*/
	static const int ERROR_AITALK_OUT_OF_MEMORY                      = 23106; // ivErr_OutOfMemory         = 7
	static const int ERROR_AITALK_INVALID_RESOURCE                   = 23107; // ivErr_InvResource         = 8  /*��Դ��Ч*/
	static const int ERROR_AITALK_NOT_FOUND                          = 23108; // ivErr_NotFound            = 9  /*���ļ�ʧ��*/
	static const int ERROR_AITALK_INVALID_GRAMMAR                    = 23109; // ivErr_InvGrmr             = 10 /*ʶ���﷨����*/

	/* For object status */
	static const int ERROR_AITALK_INVALID_CALL                       = 23110; // ivErr_InvCall             = 11 /*��Ч����*/

	/* For ASR Input */
	static const int ERROR_AITALK_SYNTAX_ERROR                       = 23111; // ivErr_InvCall             = 12

	/* For Message Call Back */
	static const int ERROR_AITALK_RESET                              = 23112; // ivErr_Reset               = 13
	static const int ERROR_AITALK_ENDED                              = 23113; // ivErr_Ended               = 14
	static const int ERROR_AITALK_IDLE                               = 23114; // ivErr_Idle                = 15
	static const int ERROR_AITALK_CANNOT_SAVE_FILE                   = 23115; // ivErr_CanNotSaveFile      = 16

	/* For Lexicon name */
	static const int ERROR_AITALK_INVALID_GRAMMAR_NAME               = 23116; // ivErr_InvName             = 17 /*�ķ���ʵ����ƷǷ�*/

	static const int ERROR_AITALK_BUFFER_EMPTY                       = 23117; // ivErr_BufferEmpty         = 18

	static const int ERROR_AITALK_GET_RESULT                         = 23118; // ivErr_GetResult           = 19

	static const int ERROR_AITALK_REACT_OUT_TIME                     = 23119; // ivErr_ReactOutTime        = 20 /*��Ӧ��ʱ*/
	static const int ERROR_AITALK_SPEECH_OUT_TIME                    = 23120; // ivErr_SpeechOutTime       = 21 /*������ʱ*/

	static const int ERROR_AITALK_AUDIO_CUT                          = 23121; // ivErr_CUT                 = 22 /*¼����������*/
	static const int ERROR_AITALK_AUDIO_LOWER                        = 23122; // ivErr_LOWER               = 23 /*¼����������*/

	static const int ERROR_AITALK_INSUFFICIENT_PERMISSIONS           = 23123; // ivErr_Limitted            = 24 /*��Ȩ����*/
	static const int ERROR_AITALK_RESULT_ERROR                       = 23124; // ivErr_ResultError         = 25 /*������Wfst�������Ȼ��cmd���*/
	static const int ERROR_AITALK_SHORT_PAUSE                        = 23125; // ivErr_ShortPause          = 26
	static const int ERROR_AITALK_BUSY                               = 23126; // ivErr_Busy                = 27
	static const int ERROR_AITALK_GRM_NOT_UPDATE                     = 23127; // ivErr_GrmNotUpdate        = 28 /*�﷨δ����*/
	static const int ERROR_AITALK_STARTED                            = 23128; // ivErr_Started             = 29
	static const int ERROR_AITALK_STOPPED                            = 23129; // ivErr_Stopped             = 30
	static const int ERROR_AITALK_ALREADY_STARTED                    = 23130; // ivErr_AlreadyStarted      = 31
	static const int ERROR_AITALK_ALREADY_STOPPED                    = 23131; // ivErr_AlreadyStopped      = 32
	static const int ERROR_AITALK_TOO_MANY_COMMAND                   = 23132; // ivErr_TooManyCmd          = 33
	static const int ERROR_AITALK_WAIT                               = 23133; // ivErr_Wait                = 34 /*�����������һЩ���������߳���Ҫ�ȴ�*/
	static const int ERROR_AITALK_MAE_RIGHT                          = 23134; // ivErr_MAERight            = 35
	static const int ERROR_AITALK_MAE_WRONG                          = 23135; // ivErr_MAEWrong            = 36

	static const int ERROR_AITALK_GRM_ERR                            = 23300;  // �﷨����



	/*Error Code Of AiSound*/

	/*Error Code Of AiSound Operation*/
	static const int ERROR_TTS_INVALID_PARA                          = 24000; /* ������� */
	static const int ERROR_TTS_INVALID_PARA_VALUE                    = 24001; /* ��Ч�Ĳ���ֵ*/
	static const int ERROR_TTS_OUT_OF_MEMORY	                       = 24002; /* �ڴ治��*/
	static const int ERROR_TTS_INVALID_HANDLE                        = 24003; /* ��Ч�ľ��*/
	static const int ERROR_TTS_CREATE_HANDLE_FAILED			       = 24004; /* �������ʧ��*/
	static const int ERROR_TTS_INVALID_RESOURCE	                   = 24005;	/* ��Ч��Դ */
	static const int ERROR_TTS_INVALID_VOICE_NAME	                   = 24006;	/* ��Ч������*/
	static const int ERROR_TTS_ENGINE_UNINIT			               = 24007; /* ����δ��ʼ�� */
	static const int ERROR_TTS_ENGINE_INIT_FAILED	                   = 24008;	/* �����ʼ��ʧ�� */
	static const int ERROR_TTS_ENGINE_BUSY			               = 24009; /* ����æ */


	/*Error Code Of AiSound Engine*/
	static const int ERROR_AISOUND_BASE					           = 24100;
	static const int ERROR_AISOUND_UNIMPEMENTED				       = 24100;  /* unimplemented function */
	static const int ERROR_AISOUND_UNSUPPORTED				       = 24101;  /* unsupported on this platform */
	static const int ERROR_AISOUND_INVALID_HANDLE			           = 24102;  /* invalid handle */
	static const int ERROR_AISOUND_INVALID_PARA			           = 24103;  /* invalid parameter(s) */
	static const int ERROR_AISOUND_INSUFFICIENT_HEAP			       = 24104;  /* insufficient heap size  */
	static const int ERROR_AISOUND_STATE_REFUSE				       = 24105;  /* refuse to do in current state  */
	static const int ERROR_AISOUND_INVALID_PARA_ID			       = 24106;  /* invalid parameter ID */
	static const int ERROR_AISOUND_INVALID_PARA_VALUE		           = 24107;  /* invalid parameter value */
	static const int ERROR_AISOUND_RESOURCE					       = 24108;  /* Resource is error */
	static const int ERROR_AISOUND_RESOURCE_READ				       = 24109;  /* read resource error */
	static const int ERROR_AISOUND_LBENDIAN					       = 24110;  /* the Endian of SDK  is error */
	static const int ERROR_AISOUND_HEADFILE					       = 24111;  /* the HeadFile is different of the SDK */
	static const int ERROR_AISOUND_BUFFER_OVERFLOW		           = 24112;  /* get data size exceed the data buffer */
	static const int ERROR_AISOUND_INVALID_ISAMPA			           = 24113;  /* !Invalid iSampa format or input iSampa text contain invalid alphabet*/
	static const int ERROR_AISOUND_INVALID_CSSML     	               = 24114;   /* !Invalid cssml format */


	/*Error Code Of ivw*/

	/*Error Code Of ivw Operation*/
	static const int ERROR_IVW_ENGINE_UNINI             = 25000; /* ����δ��ʼ�� */
	static const int ERROR_IVW_RESVER_NOMATCH           = 25001; /* ��Դ�汾��ƥ�� */
	static const int SPEECH_ERROR_IVW_INVALID_RESTYPE   = 25002; /* ���Ϸ�����Դ���� */

	/*Error Code Of ivw Engine*/
	static const int ERROR_IVW_INVALID_CALL             = 25101;  // IvwErr_InvCal       = 1
	static const int ERROR_IVW_INVALID_ARG              = 25102;  // IvwErr_InvArg	    = 2
	static const int ERROR_IVW_TELL_SIZE                = 25103;  // IvwErr_TellSize     = 3
	static const int ERROR_IVW_OUT_OF_MEMORY            = 25104;  // IvwErr_OutOfMemory  = 4
	static const int ERROR_IVW_OUT_BUFFER_FULL          = 25105;  // IvwErr_BufferFull	= 5
	static const int ERROR_IVW_OUT_BUFFER_EMPTY         = 25106;  // IvwErr_BufferEmpty	= 6
	static const int ERROR_IVW_INVALID_RESOURCE         = 25107;  // IvwErr_InvRes		= 7
	static const int ERROR_IVW_REPETITIOPN_ENTER        = 25108;  // IvwErr_ReEnter		= 8
	static const int ERROR_IVW_NOT_SUPPORT              = 25109;  // IvwErr_NotSupport	= 9
	static const int ERROR_IVW_NOT_FOUND                = 25110;  // IvwErr_NotFound		= 10
	static const int ERROR_IVW_INVALID_SN               = 25111;  // IvwErr_InvSN		= 11
	static const int ERROR_IVW_LIMITTED                 = 25112;  // IvwErr_Limitted		= 12
	static const int ERROR_IVW_TIME_OUT                 = 25113;  // IvwErr_TimeOut		= 13

	static const int ERROR_IVW_ENROLL1_SUCESS           = 25114;  // IvwErr_Enroll1_Success = 14
	static const int ERROR_IVW_ENROLL1_FAILED           = 25115;  // IvwErr_Enroll1_Failed  = 15
	static const int ERROR_IVW_ENROLL2_SUCESS           = 25116;  // IvwErr_Enroll2_Success = 16
	static const int ERROR_IVW_ENROLL2_FAILED           = 25117;  // IvwErr_Enroll2_Failed  = 17
	static const int ERROR_IVW_ENROLL3_SUCESS           = 25118;  // IvwErr_Enroll3_Success = 18
	static const int ERROR_IVW_ENROLL3_FAILED           = 25119;  // IvwErr_Enroll3_Failed  = 19
	static const int ERROR_IVW_SPEECH_TOO_SHORT         = 25120;  // IvwErr_SpeechTooShort  = 20
	static const int ERROR_IVW_SPEECH_STOP              = 25121;   // IvwErr_SpeechStop      = 21
};

}


#endif /* AIUIERRORCODE_H_ */
