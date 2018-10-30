


class Helper_Functions:

   def __init__( self, assembler):
        self.asm  = assembler

   def  one_step( self, function, param1=0,parm2=0,parm3=0 , name=""):       
        self.asm.define_link("One_Step" , function, param1,parm2,parm3, name=name)
        self.asm.foriegn_helper_functions[function] = function 
   
   def  code_step( self, function, param1=0,parm2=0,parm3= 0 ,name=""):	
       self.asm.define_link("Code_Step" , function, param1,parm2,parm3,name=name)
       self.asm.foriegn_helper_functions[function] = function 
       
   def  halt(self,name =""):
         self.asm.define_link_a("Halt", name=name)
	 
   def terminate( self ,name =""):
         self.asm.define_link_a("Terminate", name=name)
	 
   def  reset( self ,name ="" ):            
        self.asm.define_link_a("Reset", name=name )

   def  send_event( self ,event,data =0,name =""):              
        self.asm.define_link_a("SendEvent",event,data, name=name)

   def  wait_tod( self,dow,hr,minute,sec ,name ="" ):         
        self.asm.define_link_a("WaitTod",hr,minute,sec, name=name )

   def  wait_event( self, event, time_count = 0 ,name ="" ):      #wait for  events within a time window if not then 
                                                                  #reset -- time_count =0 forever	   
        self.asm.define_link_a("WaitEvent",event,time_count, name=name )
	
   def  wait_event_count( self, event, event_count ,name ="" ):      #wait for n events      
        self.asm.define_link_a("WaitEventCount",event,event_count, name=name)

   def  wait_time( self,time_ticks ,name ="" ):         
        self.asm.define_link_a("WaitTime" ,time_ticks, name=name )

   def  wait( self,wait_function ,parm_1=0,parm_2=0,time_out=0,name ="" ):
        self.asm.define_link("Wait", wait_function, parm_1,parm_2,time_out,name=name)
        self.asm.foriegn_helper_functions[wait_function] = wait_function

   def  verify_not_tod( self,dow,hr,minute,sec ,name ="" ):         
        self.asm.define_link_a("Verify_Not_Tod",hr,minute,sec , name=name)

   def  verify_not_event( self, event ,name ="" ):           
        self.asm.define_link_a("Verify_Not_Event",event, name=name)

   def  verify_not_time( self,time_ticks  ,name =""):         
        self.asm.define_link_a("Verify_Not_Timeout" ,time_ticks, name=name )


   def  verify( self,verify_function ,parm_1=0,parm_2=0,parm_3=0,name ="" ):          
        self.asm.define_link("Verify",verify_function, parm_1,parm_2,parm_3,name=name) 
        self.asm.foriegn_helper_functions[verify_function] = verify_function


   def  nop( self ,parm_1=0,name ="" ):          
        self.asm.define_link_a("Nop",parm_1, name=name)

   def  enable_chain( self ,chain_1 = -1, chain_2 = -1, chain_3 = -1  ,name =""):                   
        self.asm.define_link_a("Enable_Chain",chain_1,chain_2,chain_3, name=name)

   def  disable_chain( self, chain_1 = -1, chain_2 = -1, chain_3 = -1 ,name =""):        
        self.asm.define_link_a("Disable_Chain",chain_1,chain_2,chain_3, name=name)


   def  change_state( self , link_number ,name ="" ):     
        self.asm.define_link_a("Change_State",link_number, name=name) 

   def  reset_system( self ,name ="" ):    
        self.asm.define_link_a("Reset_System, name=name")

   def  suspend_chain( self, chain_1 = -1, chain_2 = -1, chain_3 = -1 ,name ="" ):     
        self.asm.define_link_a("Suspend_Chain", chain_1, chain_2, chain_3, name=name)

   def  resume_chain( self ,chain_1 = -1, chain_2 = -1, chain_3 = -1  ,name =""):   
        self.asm.define_link_a("Resume_Chain",chain_1,chain_2,chain_3, name=name)
      



