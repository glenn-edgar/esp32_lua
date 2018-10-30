from helper_functions import  Helper_Functions
from assembler          import  CF_Assembler

cf = CF_Assembler()
hf = Helper_Functions( cf )


cf.define_chain("initialization",True)

hf.one_step("add_watch_dog")
hf.one_step("app_input_register_configure_read")
hf.one_step("app_input_register_read")
hf.wait( "wait_for_mqtt_connect"  )
hf.enable_chain( "sampling_task")
hf.terminate()  #initialization is done now disable the chain
cf.end_chain()






#These chains are for actions every second 
cf.define_chain("feed_watch_dog", True )
hf.wait_event("CF_SECOND_TICK")
hf.one_step("pat_watch_dog")
hf.reset()
cf.end_chain()


cf.define_chain("sampling_task", False )
hf.wait_event("CF_TIME_TICK_EVENT")
hf.one_step("app_input_sample_inputs")
hf.reset()
cf.end_chain()






cf.generate_c_header()


