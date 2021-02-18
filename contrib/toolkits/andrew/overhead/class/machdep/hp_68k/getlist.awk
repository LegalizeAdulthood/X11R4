# awk script to list all modules and entry points from a library which are
# needed (via transitive closure) for a given list of basic entry points

# the input to this awk script should be generated by:  nm -go /lib/libc.a | tr : " "

# data structures used here:
#
#  definer[entry-point-name] = module-name
#  epcount = # entry-point-name's seen so far
#  ep[k] = k'th entry-point-name
#  refcount[entry-point-name] = number of references by wanted modules
#  referrer[entry-point-name.k] = k'th module-name with an undefined reference to this entry-point-name
#  want[module-name] = "Y" if this module wanted

# search output of nm and set up definers and referrers

/ [ATDBC] / {
    if (definer[$5] == "")
	definer[ep[epcount++] = $5] = $2;
}

/ U / {
    referrer[$4 "." refcount[$4]++] = $2;
}

# postprocessing

END {


# Specify which entry points we definitely want.  Edit this list to add entry points.

  want[definer["_class_RoutineStruct"]] = "Y";
  want[definer["_class_Error"]] = "Y";
  want[definer["_ProgramName"]] = "Y";
#  want[definer["_class_Header"]] = "Y";
  want[definer["_class_NewObject"]] = "Y";
  want[definer["_class_Load"]] = "Y";
  want[definer["_class_IsLoaded"]] = "Y";
  want[definer["_class_Lookup"]] = "Y";
  want[definer["_class_IsType"]] = "Y";
  want[definer["_class_IsTypeByName"]] = "Y";
  want[definer["_class_EnterInfo"]] = "Y";
  want[definer["_class_SetClassPath"]] = "Y";
  want[definer["_class_PrependClassPath"]] = "Y";
  want[definer["_class_GetEText"]] = "Y";
  want[definer["_environ"]] = "Y";
  want[definer["__exit"]] = "Y";
  want[definer["_abort"]] = "Y";
  want[definer["_alloca"]] = "Y";
  want[definer["_blt"]] = "Y";
#  want[definer["_bcopy"]] = "Y";
  want[definer["_brk"]] = "Y";
#  want[definer["_bzero"]] = "Y";
  want[definer["_calloc"]] = "Y";
  want[definer["_cfree"]] = "Y";
  want[definer["_errno"]] = "Y";
  want[definer["_close"]] = "Y";
  want[definer["_errno"]] = "Y";
  want[definer["_creat"]] = "Y";
  want[definer["__ctype_"]] = "Y";
  want[definer["__doprnt"]] = "Y";
  want[definer["_ecvt"]] = "Y";
  want[definer["_fcvt"]] = "Y";
  want[definer["_sys_errlist"]] = "Y";
  want[definer["_sys_nerr"]] = "Y";
  want[definer["_exit"]] = "Y";
  want[definer["_fcntl"]] = "Y";
  want[definer["__filbuf"]] = "Y";
  want[definer["__iob"]] = "Y";
  want[definer["__cleanup"]] = "Y";
  want[definer["__flsbuf"]] = "Y";
  want[definer["_fclose"]] = "Y";
  want[definer["_fflush"]] = "Y";
  want[definer["_fopen"]] = "Y";
  want[definer["_fprintf"]] = "Y";
  want[definer["_fread"]] = "Y";
  want[definer["_fstat"]] = "Y";
  want[definer["_fwrite"]] = "Y";
  want[definer["_gcvt"]] = "Y";
  want[definer["_getdtablesize"]] = "Y";
  want[definer["_getpagesize"]] = "Y";
  want[definer["_ioctl"]] = "Y";
  want[definer["_isatty"]] = "Y";
  want[definer["_lseek"]] = "Y";
  want[definer["_malloc"]] = "Y";
  want[definer["_memcmp"]] = "Y";
  want[definer["_memcpy"]] = "Y";
  want[definer["_memset"]] = "Y";
  want[definer["_modf"]] = "Y";
  want[definer["_realloc"]] = "Y";
  want[definer["_free"]] = "Y";
  want[definer["_open"]] = "Y";
  want[definer["_perror"]] = "Y";
  want[definer["_printf"]] = "Y";
  want[definer["_read"]] = "Y";
  want[definer["_sbrk"]] = "Y";
  want[definer["curbrk"]] = "Y";
  want[definer["_sigvec"]] = "Y";
  want[definer["_sprintf"]] = "Y";
  want[definer["_strlen"]] = "Y";
  want[definer["_syscall"]] = "Y";
  want[definer["_write"]] = "Y";
  want[definer["_writev"]] = "Y";

# now take transitive closure of wanted modules

    for (needmore = "Y"; needmore == "Y"; needmore = "N") {
	for ( i = 0; i < epcount; i++) {
	    if (want[definer[ep[i]]] != "Y") {
		want[definer[ep[i]]] = "N";
		for ( j = 0; j < refcount[ep[i]]; j++ ) {
		    if (want[referrer[ep[i] "." j]] == "Y") {
			want[definer[ep[i]]] = "Y";
			needmore = "Y";
			break;
		    }
		}
	    }
	}
    }

# write out all wanted module name and entry points

    for ( i = 0 ; i < epcount ; i++)
	if (want[definer[ep[i]]] == "Y")
	    printf "%s %s\n", definer[ep[i]], ep[i];
}
