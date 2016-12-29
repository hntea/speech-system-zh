/*
 * xfparams.h
 *
 *  Created on: 2016年12月29日
 *      Author: hntea
 */

#ifndef SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_
#define SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_

using namespace std;

typedef struct basic{
	string  loginParams;
	string  userWord;
}XfBasic;

typedef struct online{
	string  params;
}XfOnlineASR;

typedef struct local{
	string engine_type;
	int sample_rate;
	string result_type;
	string result_encoding;
	string asr_model;
	string grm_build_path;
	string bnf_file;
}XfLocalASR;


#endif /* SRC_ASR_SERVER_INCLUDE_ASR_SERVER_XFPARAMS_H_ */
