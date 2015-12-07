
#include "ReadWave.h"



JNIEXPORT void JNICALL Java_com_wave_server_ComtradeWrap_freeXJComtrade
	(JNIEnv *, jobject){
		freeQuickComtrade();
}

JNIEXPORT jint JNICALL Java_com_wave_server_ComtradeWrap_getComtradeWeb
	(JNIEnv *env, jclass jclss, jstring cfgPath,  jobject objStream, jint){

		jclass streamtCls = env->GetObjectClass(objStream);   
		jmethodID streamtCons = env->GetMethodID(streamtCls,"<init>","()V"); 
		jmethodID streamtWriteObject = env->GetMethodID(streamtCls,"writeObject","(Ljava/lang/Object;)Z");

		return 0;
}

JNIEXPORT jstring JNICALL Java_com_wave_server_ComtradeWrap_getComtrade 
	(JNIEnv *env, jclass jclss, jstring cfgPath, jobject cfg, jobject channelA, jobject channelD)

{
	return Java_com_wave_server_ComtradeWrap_getComtradeB (   env,   jclss,   cfgPath,   cfg,   channelA,   channelD, false);
}

JNIEXPORT jstring JNICALL Java_com_wave_server_ComtradeWrap_getComtradeB
	(JNIEnv *env, jclass jclss, jstring cfgPath, jobject cfg, jobject channelA, jobject channelD, jboolean isFree){

		Comtrade cm;
		if(isFree){
			cm.isFreeQuickComtrade=true;
		}
		
		char* strPath = jstringToWindows(env,cfgPath); 
		char sMsgErr[256];
		int nRl=cm.LoadFile(strPath,sMsgErr);

		if(nRl!=0){
			cout<<"readwave.dll:"<<"load file failed"<<endl;
			return NULL;
		}

		int nrCfg=cm.GetComtradeCfg(env,jclss,cfg,sMsgErr);
		if(nrCfg!=0){
			cout<<"readwave.dll:"<<"get cfg failed"<<endl;
			return NULL;
		}

		int nRA=cm.GetComtradeA(env,jclss,channelA,sMsgErr);
		if(nRA!=0){
			cout<<"readwave.dll:"<<"load A channels failed"<<endl;
			return NULL;
		}

		int nRD=cm.GetComtradeD(env,jclss,channelD,sMsgErr);
		if(nRD!=0){
			cout<<"readwave.dll:"<<"load D channels failed"<<endl;
			return NULL;
		} 
		
		return cm.GetHdrInfo(env);
}


