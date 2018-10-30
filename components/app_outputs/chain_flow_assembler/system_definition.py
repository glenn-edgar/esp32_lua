from helper_functions import  Helper_Functions
from assembler          import  CF_Assembler

cf = CF_Assembler()
hf = Helper_Functions( cf )


cf.define_chain("initialization",True)
hf.one_step("analog_output_store_cf_handle_ref")
hf.one_step("analog_output_subscribe_configuration")
hf.one_step("analog_ouptut_subscribe_output")
hf.one_step("analog_output_subscribe_pulse_output")
hf.one_step("add_watch_dog")
hf.wait( "wait_for_mqtt_connect"  )
hf.enable_chain( "pulse_task")
hf.terminate()  #initialization is done now disable the chain
cf.end_chain()






#These chains are for actions every second 
cf.define_chain("feed_watch_dog", True )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("pat_watch_dog")
hf.reset()
cf.end_chain()

cf.define_chain("pulse_task",False)
hf.wait_event("CF_PULSE_EVENT")
hf.one_step("analog_output_set_initial_value")
hf.wait("analog_output_step_timer")
hf.one_step("analog_output_set_final_value")
hf.reset()
cf.end_chain()

cf.generate_c_header()


