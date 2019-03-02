/*
  ext_m5avatar.cpp

  Defining extension methods of Arduino

  Copyright (c) 2018, katsuhiko kageyama All rights reserved.

*/
#include "mrubyc_config.h"

#ifdef USE_M5AVATAR

#include "mrubyc.h"
#include "ext.h"
#include <M5Stack.h>
#include <Avatar.h>
#include <tasks/LipSync.h>
#include <AquesTalkTTS.h>

using namespace m5avatar;

static Avatar *avatar=NULL;

static void class_m5avatar_initialize(mrb_vm *vm, mrb_value *v, int argc ){
	/*
	char* arg1 = NULL;
	if(argc>1 && GET_TT_ARG(1)==MRB_TT_STRING){
		arg1 = GET_STRING_ARG(1);
	}
	if(argc>1 && GET_TT_ARG(1)!=MRB_TT_STRING){
		DEBUG_PRINTLN("class_m5avatar_initialize:ERROR");
		SET_FALSE_RETURN();
		return;
	}
	  */
	if(avatar==NULL){
		TTS.create(NULL);
		avatar = new Avatar();
		avatar->init();
		avatar->addTask(lipSync, "lipSync");
	}
	SET_TRUE_RETURN();
}
static void class_m5avatar_speech(mrb_vm *vm, mrb_value *v, int argc ){
	char* arg1 = NULL;
	if(GET_TT_ARG(1) == MRB_TT_STRING){
		arg1 = (char*)GET_STRING_ARG(1);
	}else{
		DEBUG_PRINTLN("class_m5avatar_speech:ERROR");
		SET_FALSE_RETURN();
		return;
	}
	if(strlen(arg1)>1){
		TTS.play(arg1, 80);
	}else{
	}
	avatar->setSpeechText(arg1);
	SET_TRUE_RETURN();
}

void define_m5avatar_class(void){
	mrb_class *class_m5avatar;
	class_m5avatar = mrbc_define_class(0, "M5Avatar", mrbc_class_object);
	mrbc_define_method(0, class_m5avatar, "initialize", class_m5avatar_initialize);
	mrbc_define_method(0, class_m5avatar, "speech", class_m5avatar_speech);

}

#endif
