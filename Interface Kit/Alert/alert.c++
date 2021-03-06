#include "mlvalues.h"
#include "alloc.h"
#include "memory.h"
#include "signals.h"
#include "threads.h"

#include <Alert.h>
#include <stdio.h>
#include "interfaceDefs.h"

#include "glue.h"

extern "C" {
	extern sem_id ocaml_sem;
	alert_type decode_alert_type(value alert_type);
	button_spacing decode_button_spacing(value button_spacing);
	value b_alert_alert_bytecode(value *argv, int argn);
	value b_alert_alert_nativecode(value self, value title, value text, value button0Label, value button1Label, value button2Label, value widthStyle, value alert_type);
	value b_alert_alert_spacing_bytecode(value *argv, int argn);
	value b_alert_alert_spacing_nativecode(value self, value title, value text, value button0Label, value button1Label, value button2Label, value widthStyle, value spacing, value alert_type);
	value b_alert_go(value alert);
}

class OAlert : public BAlert, public Glue 
	{
		public :
				OAlert(value self, char *title, char *text, char *button0Label, char *button1Label, char *button2Label, button_width widthStyle, alert_type alert_type):
					BAlert(title, text, button0Label, button1Label, button2Label, widthStyle, alert_type)
					,Glue(self)
				{
			    }
				
				OAlert(value self, char *title, char *text, char *button0Label, char *button1Label, char *button2Label, button_width widthStyle, button_spacing button_spacing, alert_type alert_type):
					BAlert(title, text, button0Label, button1Label, button2Label, widthStyle, button_spacing, alert_type)
				,	Glue(self)
				{
			    }
			    
#ifdef __HAIKU__
				void SetLayout(BLayout *) {
			    }
#endif
};

//***********************
alert_type decode_alert_type(value alert_type){
	CAMLparam1(alert_type);

	switch(Int_val(alert_type)){
		case 0 : return B_EMPTY_ALERT;
		case 1 : return B_INFO_ALERT;
		case 2 : return B_IDEA_ALERT;
		case 3 : return B_WARNING_ALERT;
		case 4 : return B_STOP_ALERT;
	};
}

//***********************
button_spacing decode_button_spacing(value button_spacing){
	CAMLparam1(button_spacing);

	switch(Int_val(button_spacing)){
		case 0 : CAMLreturnT(enum button_spacing, B_EVEN_SPACING);
		case 1 : CAMLreturnT(enum button_spacing, B_OFFSET_SPACING);
	};
}

//*********************
value b_alert_alert_nativecode(value self,
			       value title, 
			       value text, 
			       value button0Label, 
			       value button1Label, 
			       value button2Label, 
			       value widthStyle, 
			       value alert_type){

	CAMLparam5(self, title, text, button0Label, button1Label);
	CAMLxparam3(button2Label, widthStyle, alert_type);
	CAMLlocal1(p_alert);
	
	char *b1;
	char *b2;
//caml_leave_blocking_section();
	Is_block(button1Label)?
		b1 = String_val(Field(button1Label, 0))
	:	b1 = '\0';
	
	Is_block(button2Label)?
		b2 = String_val(Field(button2Label, 0))
	:	b2 = '\0';
	p_alert = alloc_small(1,Abstract_tag);
	caml_register_global_root(&p_alert);

	caml_release_runtime_system();
		OAlert *ba = new OAlert(self,
					String_val(title), 
					String_val(text), 
					String_val(button0Label), 
					b1, 
					b2, 
					decode_button_width(widthStyle), 
					decode_alert_type(alert_type)
				       );
	caml_acquire_runtime_system();
	Field(p_alert,0) = (value) ba;
	printf("C 0x%lx : %lx\n", ba, sizeof(OAlert));
	
	if (button1Label == Val_int(0)) 
		b1=NULL;
	else 
		b1=String_val(button1Label);
	
	if (button2Label == Val_int(0)) 
		b2=NULL;
	else 
		b2=String_val(button2Label);
	
//caml_enter_blocking_section();
	CAMLreturn(p_alert);
}

//*********************
value b_alert_alert_bytecode(value *argv, int argn){
	return b_alert_alert_nativecode(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
}

//*********************
value b_alert_alert_spacing_nativecode(value self,
									   value title, 
							   		   value text, 
									   value button0Label, 
									   value button1Label, 
				    		           value button2Label, 
									   value widthStyle, 
									   value spacing,
									   value alert_type){

	CAMLparam5(self, title, text, button0Label, button1Label);
	CAMLxparam4(button2Label, widthStyle, spacing, alert_type);
	CAMLlocal1(p_alert);
	
	char *b1;
	char *b2;
	
	if (button1Label == Val_int(0)) 
		b1=NULL;
	else 
		b1=String_val(button1Label);
	
	if (button2Label == Val_int(0)) 
		b2=NULL;
	else 
		b2=String_val(button2Label);
	
	p_alert = alloc_small(1,Abstract_tag);
	caml_register_global_root(&p_alert);

	caml_release_runtime_system();
	OAlert *oa = new OAlert(self,
				String_val(title), 
				String_val(text), 
				String_val(button0Label),
				b1,
				b2,
				decode_button_width(widthStyle),
				decode_button_spacing(spacing),
				decode_alert_type(alert_type)
			    );
	caml_acquire_runtime_system();
	Field(p_alert,1) = (value)oa;
	printf("C 0x%lx : %lx\n", oa, sizeof(OAlert));

	CAMLreturn(p_alert);
}

//*********************
value b_alert_alert_spacing_bytecode(value *argv, int argn){
	return b_alert_alert_spacing_nativecode(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
}


//************************
value b_alert_go(value alert) {
	CAMLparam1(alert);
	CAMLlocal1(caml_res);
	status_t res;
	
	OAlert *oalert = (OAlert *)Field(alert,0);	
//	caml_leave_blocking_section();
	caml_release_runtime_system();
		res=oalert->BAlert::Go();
	caml_acquire_runtime_system();
//	caml_enter_blocking_section();
	caml_res = copy_int32(res);

	CAMLreturn(caml_res);
}
