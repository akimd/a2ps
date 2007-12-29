## --------------------------------------------------------- ##
## Find what is the program to use to send to the printer    ##
## 				demaille@inf.enst.fr	     ##
## --------------------------------------------------------- ##

# serial 3

#
#	Find the program to send to the printer
# Put the prog name in LPR, its option for destination in LPR_QUEUE_OPTION
#
AC_DEFUN([ad_PROG_LPR],
  [AC_CHECK_PROGS(LPR, lp lpr, no)
   AC_MSG_CHECKING([for printer queue selection])
   case "x$LPR" in
      xlp)  LPR_QUEUE_OPTION=-d ;;
      xlpr) LPR_QUEUE_OPTION=-P ;;
      xno)  LPR="cat"
            LPR_QUEUE_OPTION=">"
	    ;;
      *)    # This user has specified per $LPR and $LPR_QUEUE_OPTION
	    ;;
   esac
   AC_MSG_RESULT([$LPR $LPR_QUEUE_OPTION PRINTER])
   AC_SUBST(LPR_QUEUE_OPTION)])
