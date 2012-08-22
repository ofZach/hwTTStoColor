/* Generated automatically from make_voice_list */

#include "flite.h"

cst_voice *register_cmu_us_kal(const char *voxdir);
cst_voice *register_cmu_time_awb(const char *voxdir);
cst_voice *register_cmu_us_kal16(const char *voxdir);
cst_voice *register_cmu_us_awb(const char *voxdir);
cst_voice *register_cmu_us_rms(const char *voxdir);
cst_voice *register_cmu_us_slt(const char *voxdir);

cst_val *flite_set_voice_list(void)
{
   flite_voice_list = cons_val(voice_val(register_cmu_us_kal(NULL)),flite_voice_list);
   flite_voice_list = cons_val(voice_val(register_cmu_time_awb(NULL)),flite_voice_list);
   flite_voice_list = cons_val(voice_val(register_cmu_us_kal16(NULL)),flite_voice_list);
   flite_voice_list = cons_val(voice_val(register_cmu_us_awb(NULL)),flite_voice_list);
   flite_voice_list = cons_val(voice_val(register_cmu_us_rms(NULL)),flite_voice_list);
   flite_voice_list = cons_val(voice_val(register_cmu_us_slt(NULL)),flite_voice_list);
   flite_voice_list = val_reverse(flite_voice_list);
   return flite_voice_list;
}

