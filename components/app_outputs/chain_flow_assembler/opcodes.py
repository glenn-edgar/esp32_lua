
import datetime

#
# These are the opcodes of the python object chain flow
#
#
#
#

class Opcodes():
  
  def __init__(self):
        
        self.opcodes = {}
        self.opcodes["Terminate"]                        =    "terminate_fn"
        self.opcodes[ "Halt"]                               =    "halt_fn"
        self.opcodes["One_Step" ]                         =    "one_step_fn" 
        self.opcodes["Code_Step"]                         =    "code_step_fn"	
        self.opcodes["Reset" ]                              =    "reset_fn"     
        self.opcodes["SendEvent"]                         =    "send_event_fn" 
        self.opcodes["WaitTod" ]                           =    "wait_tod_fn"  
        self.opcodes["WaitEvent"]                         =    "wait_event_fn"
        self.opcodes["WaitEventCount"]                 =    "wait_event_count_fn"
        self.opcodes["WaitTime" ]                         =    "wait_time_fn"       
        self.opcodes["Wait"]                                =    "wait_condition_fn"  
        self.opcodes["Verify"]                             =    "verify_condition_fn" 
        self.opcodes["Verify_Not_Timeout"]           =   "verify_not_timeout_fn"
        self.opcodes["Verify_Not_Event"]              =   "verify_not_event_fn"
        self.opcodes["Verify_Not_Tod"]                 =    "verify_not_tod_fn"
        self.opcodes["Nop"]                                 =    "nop_fn"  
        self.opcodes["Enable_Chain"]                    =    "enable_chain_fn"  
        self.opcodes["Disable_Chain"]                  =    "disable_chain_fn" 
        self.opcodes["Change_State"]                    =    "change_state_fn" 
        self.opcodes["Reset_System"]                    =    "system_reset_fn"
        self.opcodes["Suspend_Chain"]                  =    "suspend_chain_fn"
        self.opcodes["Resume_Chain"]                   =    "resume_chain_fn"
        
  def has_key( self, opcode_name):
      return self.opcodes.has_key( opcode_name )

  def get_opcode( self, opcode_name):
     return self.opcodes[ opcode_name ]
  
  def add_opcode( self, name,code ):
       self.opcodes[name] = code

   

