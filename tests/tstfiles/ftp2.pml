/* 
 * File Transfer Protocol 
 * 
 * Promela (Protocol Meta Language) 
 * 
 * J-Ph Cottin 12/96 (inspired by Gerard J. Holzmann) 
 */ 
 
 
/* 
 * Global Definitions 
 */ 
 
#define LOSS           0    /* message loss   */ 
#define DUPS           0    /* duplicate msgs */    
#define QSZ            2    /* queue size */ 
 
mtype = { 
  red, white, blue, 
  abort, accept, ack, sync_ack, close, connect, 
  create, data, eof, open, reject, sync, transfer,  
  FATAL, NON_FATAL, COMPLETE 
 } 
 
 
/*chan use_to_pres[2] = [QSZ] of {byte}; 
chan pres_to_use[2] = [QSZ] of {byte}; 
chan pres_to_ses[2] = [QSZ] of {byte}; 
 
chan ses_to_pres[2] = [QSZ] of {byte,byte}; 
chan ses_to_flow[2] = [QSZ] of {byte,byte}; 
chan flow_to_ses[2] = [QSZ] of {byte,byte}; 
chan dll_to_flow[2] = [QSZ] of {byte,byte}; 
chan flow_to_dll[2] = [QSZ] of {byte,byte}; 
 
chan ses_to_fsrv[2] = [QSZ] of {byte}; 
chan fsrv_to_ses[2] = [QSZ] of {byte}; */
 
chan use_to_pres[2] = [QSZ] of {mtype}; 
chan pres_to_use[2] = [QSZ] of {mtype}; 
chan pres_to_ses[2] = [QSZ] of {mtype}; 
 
chan ses_to_pres[2] = [QSZ] of {mtype,byte}; 
chan ses_to_flow[2] = [QSZ] of {mtype,byte}; 
chan flow_to_ses[2] = [QSZ] of {mtype,byte}; 
chan dll_to_flow[2] = [QSZ] of {mtype,byte}; 
chan flow_to_dll[2] = [QSZ] of {mtype,byte}; 
 
chan ses_to_fsrv[2] = [QSZ] of {mtype}; 
chan fsrv_to_ses[2] = [QSZ] of {mtype}; 
  
/* 
 * User Layer Validation Model 
 */ 
 
proctype userprc ( bit n) 
{ 
  use_to_pres[n]!transfer; 
  if 
    :: pres_to_use[n]?accept -> goto Done 
    :: pres_to_use[n]?reject -> goto Done 
    :: use_to_pres[n]!abort  -> goto Aborted 
  fi; 
Aborted: 
  if 
    :: pres_to_use[n]?accept -> goto Done 
    :: pres_to_use[n]?reject -> goto Done 
  fi; 
Done: 
  skip 
    } 
 
 
/* 
 * Presentation Layer Validation Model 
 */ 
 
proctype present(bit n) 
{ 
  byte status, uabort; 
 
endIDLE: 
  do 
    :: use_to_pres[n]?transfer -> 
                   uabort = 0; 
                   break 
    ::  use_to_pres[n]?abort -> 
                   skip 
  od; 
 
TRANSFER: 
  pres_to_ses[n]!transfer; 
  do 
    :: use_to_pres[n]?abort -> 
         if 
        :: (!uabort) -> 
                   uabort = 1; 
                   pres_to_ses[n]!abort 
        :: (uabort) -> 
                   assert(1+1!=2) 
         fi 
    :: ses_to_pres[n]?accept,0 -> 
         goto DONE
    :: ses_to_pres[n]?reject(status) ->
         if 
           :: (status == FATAL || uabort) -> 
                     goto FAIL 
           :: (status == NON_FATAL && !uabort) -> 
                     goto TRANSFER 
         fi 
  od ;
 
DONE: 
  pres_to_use[n]!accept; 
  goto endIDLE; 
FAIL: 
  pres_to_use[n]!reject; 
  goto endIDLE 
    } 
 
/* 
 * Session Layer Validation Model 
 */    
 
 
proctype session (bit n)  
{ 
    bit toggle; 
    byte type, status; 
 
endIDLE: 
 do 
   :: pres_to_ses[n]?type -> 
       if 
         :: (type == transfer) ->  
                   goto DATA_OUT 
      :: (type != transfer)      /* ignore */ 
       fi 
   :: flow_to_ses[n]?type, 0 -> 
       if 
         :: (type == connect) ->  
                   goto DATA_IN 
      :: (type != connect)      /* ignore */ 
       fi 
 od; 
 
DATA_IN:          /* 1. prepare local file fsrver  */ 
  ses_to_fsrv[n]!create; 
  do 
    :: fsrv_to_ses[n]?reject -> 
             ses_to_flow[n]!reject,0; 
             goto endIDLE 
    :: fsrv_to_ses[n]?accept -> 
             ses_to_flow[n]!accept,0; 
             break 
  od; 
             /* 2. Receive the data, upto eof */ 
  do 
    :: flow_to_ses[n]?data,0 -> 
             ses_to_fsrv[n]!data 
    :: flow_to_ses[n]?eof,0 -> 
             ses_to_fsrv[n]!eof; 
             break 
    :: pres_to_ses[n]?transfer -> 
             ses_to_pres[n]!reject(NON_FATAL) 
    :: flow_to_ses[n]?close,0 ->        /* remote user aborted */ 
             ses_to_fsrv[n]!close; 
             break 
    :: timeout ->                       /* got disconnect */ 
             ses_to_fsrv[n]!close; 
             goto endIDLE 
  od; 
                          /* 3. Close the connection */ 
  ses_to_flow[n]!close,0; 
  goto endIDLE; 
 
DATA_OUT:                  /* 1. prepare local file fsver */ 
  ses_to_fsrv[n]!open; 
  if 
   :: fsrv_to_ses[n]?reject -> 
         ses_to_pres[n]!reject(FATAL); 
          goto endIDLE 
   :: fsrv_to_ses[n]?accept -> 
        skip 
 fi; 
                                      /* 2. Initialize flow control */ 
 ses_to_flow[n]!sync,toggle; 
 do 
   :: atomic { 
           flow_to_ses[n]?sync_ack,type -> 
                 if 
                   :: (type!= toggle) 
                   :: (type == toggle) -> break 
                fi 
          } 
 :: timeout -> 
            ses_to_fsrv[n]!close; 
            ses_to_pres[n]!reject(FATAL); 
            goto endIDLE 
od; 
 toggle = 1 - toggle; 
                                    /* 3. Prepare remote file fsrver */ 
 ses_to_flow[n]!connect,0; 
 if  
   :: flow_to_ses[n]?reject,0 -> 
         ses_to_fsrv[n]!close; 
         ses_to_pres[n]!reject(FATAL); 
       goto endIDLE 
   :: flow_to_ses[n]?connect,0 -> 
         ses_to_fsrv[n]!close; 
         ses_to_pres[n]!reject(NON_FATAL); 
       goto endIDLE 
   :: flow_to_ses[n]?accept,0 -> 
        skip 
    :: timeout -> 
         ses_to_fsrv[n]!close; 
         ses_to_pres[n]!reject(FATAL); 
       goto endIDLE  
fi; 
                       /* 4. Transmit the data, upto eof*/ 
do 
  :: fsrv_to_ses[n]?data -> 
            ses_to_flow[n]!data,0 
  :: fsrv_to_ses[n]?eof -> 
            ses_to_flow[n]!eof,0; 
            status = COMPLETE; 
            break 
  :: pres_to_ses[n]?abort ->          /* local user aborted  */
               ses_to_fsrv[n]!close;
               ses_to_flow[n]!close,0;
               status = FATAL;
               break
od; 
                 /* 5. Close the connexion */ 
do 
   :: pres_to_ses[n]?abort            /* ignore */ 
   :: flow_to_ses[n]?close,0 -> 
              if 
                 :: (status == COMPLETE) ->  
                          ses_to_pres[n]!accept,0       
                 :: (status != COMPLETE) -> 
                         ses_to_pres[n]!reject(status) 
             fi; 
           break 
 :: timeout -> 
       ses_to_pres[n]!reject(FATAL); 
       break 
 od; 
 goto endIDLE 
 
} 
 
/* 
 * File Server Validation Model 
 */ 
 
proctype fserver (bit n) 
{ 
 end: 
    do 
        :: ses_to_fsrv[n]?create ->     /* incoming */ 
             if 
                :: fsrv_to_ses[n]!reject  
               :: fsrv_to_ses[n]!accept -> 
            do 
                 :: ses_to_fsrv[n]?data 
                 :: ses_to_fsrv[n]?eof -> break 
                 :: ses_to_fsrv[n]?close -> break 
            od 
      fi    
     :: ses_to_fsrv[n]?open ->     /* outgoing */ 
             if 
               :: fsrv_to_ses[n]!reject  
               :: fsrv_to_ses[n]!accept -> 
            do 
                 :: fsrv_to_ses[n]!data 
                 :: fsrv_to_ses[n]!eof -> break 
                 :: ses_to_fsrv[n]?close -> break 
            od 
      fi 
 od 
} 
 
/* 
 * Flow Control Laer Validation Model 
 */ 
 
#define true    1 
#define false   0 
 
#define M       4   /* range sequence numbers */ 
#define W        2  /* window size : M/2   */ 
 
proctype fc(bit n) 
{ 
     bool busy[M];        /* outstanding messages     */            
     byte q;                   /* seq# oldest unacked msg */  
     byte m;                   /* seq# last msg received  */ 
     byte s;                   /* seq# next msg to send   */ 
     byte window;              /* nr of outstanding msgs  */ 
     byte type;                /* msg type                */ 
     bit received[M];    /* receiver housekeeping */ 
     bit x;                     /* scratch variable */ 
     byte p;                 /* seq# of last msg acked */ 
    byte I_buf[M], O_buf[M];   /* message buffers */ 
 
 /* sender part */ 
 end: 
      do 
          :: atomic { 
                (window < W && len (ses_to_flow [n]) > 0 
                        && len (flow_to_dll[n]) < QSZ ) -> 
                           ses_to_flow[n]?type,x; 
                          window = window +1; 
                          busy[s]= true; 
                         O_buf[s] = type; 
                         flow_to_dll[n]!type, s; 
                         if 
                            :: (type != sync) -> 
                                     s = (s+1) %M 
                            :: (type == sync) -> 
                                     window = 0; 
                                     s = M; 
                                    do 
                                       :: (s > 0) -> 
                                             s= s - 1; 
                                             busy[s] = false 
                                       :: (s == 0) -> break 
                                    od 
                         fi 
                     } 
       :: atomic { 
                       (window > 0 && busy[q] == false) -> 
                           window = window -1; 
                          q = (q+1) %M 
             } 
#if  DUPS 
       :: atomic { 
             (len(flow_to_dll[n]) < QSZ 
                && window >0 && busy[q] == true) -> 
                  flow_to_dll[n]! O_buf[q], q 
        } 
#endif 
       :: atomic { 
             (timeout && len(flow_to_dll[n]) < QSZ 
                && window >0 && busy[q] == true) -> 
                  flow_to_dll[n]! O_buf[q], q 
        } 
/* receiver part */ 
#if  LOSS 
       :: dll_to_flow[n]?type,m   /* lose any message */ 
#endif 
       :: dll_to_flow[n]?type,m -> 
              if 
                 :: atomic{ 
                        (type == ack) -> 
                           busy[m] = false 
                } 
                :: atomic{ 
                        (type == sync) -> 
                           m = 0;
                           do
                             :: (m < M) ->
                                  received[m] = 0;
                                  m = m + 1
			     :: (m == M) ->
                                  break
                           od  
                }; 
                  flow_to_dll[n]!sync_ack,0
	   :: (type == sync_ack) ->
                  flow_to_ses[n]!sync_ack,0
	   :: (type != ack && type != sync && type != sync_ack) ->
                  if
                    :: atomic {
                       (received[m] == true) ->
                            x = ((0 < p-m  && p-m<= W)
				 || (0 < p-m-M && p-m-M <=W))};
                       if 
                         :: (x) -> flow_to_dll[n]!ack,m
			 :: (!x) /* else skip */   
                       fi
		   :: atomic {
                         (received[m] == false) ->
                              I_buf[m] = type;
                              received[m] = true;
                              received[(m-W+M)%M] = false
				}
                 fi
           fi

     :: (received[p] == true && len(flow_to_ses[n])<QSZ
                             && len(flow_to_dll[n])<QSZ ) ->
                  flow_to_ses[n]!I_buf[p],0;
                  flow_to_dll[n]!ack,p;
                  p = (p+1) % M
     od
 }

/*
 * Datalink Layer Validation Model
 */

proctype data_link ()
{
byte type, seq;

end:
        do
	  :: flow_to_dll[0]?type,seq ->
                 if
                   :: dll_to_flow[1]!type,seq
		   :: skip /* lose message */
                 fi
          :: flow_to_dll[1]?type,seq ->
                 if
                   :: dll_to_flow[0]!type,seq
		   :: skip /* lose message */
                 fi
         od
}

/* 
 * Main Program 
 */ 
 
/*init 
  { 
    atomic { 
      run userprc(0); run userprc(1); 
      run present(0); run present(1); 
      run session(0); run session(1); 
      run fserver(0); run fserver(1); 
      run fc(0);      run fc(1); 
      run data_link() 
     } 
  } 
 */

init 
  { 
    atomic { 
      run userprc(0); 
      run present(0); 
      run session(0); 
      run fserver(0); 
      run fc(0);      
      run data_link();
      run fc(1); 
      run fserver(1); 
      run session(1); 
      run present(1); 
      run userprc(1); 
    } 
  }  
