#define log_alloc(arr, tag, ierr) call LogMemAlloc("arr",size(arr),tbs_(arr),t_r,tag,ierr)
#define LogAlloc(ERR,NAME,LEN,SIZE,TAG) CALL LogMemAlloc(NAME,LEN,SIZE,this_routine,TAG)
#define LogDealloc(TAG) CALL LogMemDealloc(this_routine,TAG)
#define log_dealloc(tag) LogDealloc(tag)
#define IsNullDet(nI) (nI(1).eq.0)

! Is the specified orbital occupied or not?
! TODO: Use ilut_int/ilut_off here?
#define IsOcc(ilut,orb) btest(ilut((orb-1)/bits_n_int), mod(orb-1,bits_n_int))
#define IsNotOcc(ilut,orb) (.not.IsOcc(ilut,orb))
#define IsUnoccDet(sgn) all(abs(sgn) < 1.e-12_dp)

! Is the specified orbital alpha or beta? Generate the appropriate pair.
#define is_beta(orb) btest(orb, 0)
#define is_alpha(orb) (.not.is_beta(orb))
#define is_one_alpha_beta(orb1,orb2) (btest(orb1,0) .neqv. btest(orb2,0)) 
#define ab_pair(orb) (ieor(orb-1,1)+1)
#define get_beta(orb) (ibclr(orb-1,0)+1)
#define get_alpha(orb) (ibset(orb-1,0)+1)

! Get the index of the replica that is paired with ind:
#define paired_replica(ind) (ind+2*mod(ind,2)-1)

! The spin where 1=alpha, 2=beta
#define get_spin(orb) (1+iand(orb,1))
! The spin where 1=alpha, -1=beta
#define get_spin_pn(orb) (1-2*iand(orb,1))

! Is the specified orbital part of a doubly occupied pair?
#define IsDoub(ilut,orb) (IsOcc(ilut,orb).and.IsOcc(ilut,ab_pair(orb)))

! Are the two orbitals specified (may be the same orbital) from the same
! spatial orbital?
#define is_in_pair(orb1,orb2) (ibclr(orb1-1,0) == ibclr(orb2-1,0))

! Set or clear orbitals in a bit representation
#define ilut_int(orb) ((orb-1)/bits_n_int)
#define ilut_off(orb) mod(orb-1,bits_n_int)
#define set_orb(ilut, orb) ilut(ilut_int(orb))=ibset(ilut(ilut_int(orb)),ilut_off(orb))
#define clr_orb(ilut, orb) ilut(ilut_int(orb))=ibclr(ilut(ilut_int(orb)),ilut_off(orb))

! Useful for fixing things. Requires this_routine to be defined
#ifdef __DEBUG
#define ASSERT(x) \
if (.not. (x)) then; \
 call stop_all (this_routine, "Assert fail: "//"x"); \
endif
#define ASSERTROOT(x) \
if ((iProcIndex.eq.Root).and.(.not. (x))) then; \
 call stop_all (this_routine, "Assert fail: "//"x"); \
endif
! Do some debugging if X>=Y
#define IFDEBUG(PrintLevel,ThisLevel) if (PrintLevel>=ThisLevel)
#define IFDEBUGEQ(PrintLevel,ThisLevel) if (PrintLevel==ThisLevel)
#define IFDEBUGEQTHEN(PrintLevel,ThisLevel) if (PrintLevel==ThisLevel) then
#define IFDEBUGTHEN(PrintLevel,ThisLevel) if (PrintLevel>=ThisLevel) then
#define ENDIFDEBUG endif
#else
#define ASSERT(x)
#define ASSERTROOT(x)
#define IFDEBUG(PrintLevel,ThisLevel) if(.false.)
#define IFDEBUGEQ(PrintLevel,ThisLevel) if(.false.)
#define IFDEBUGEQTHEN(PrintLevel,ThisLevel) if(.false.) then
#define IFDEBUGTHEN(PrintLevel,ThisLevel) if(.false.) then
#define ENDIFDEBUG endif
#endif

! Write out from the root node (concisely)
#define root_write if (iProcIndex == 0) write
#define root_print root_write (6, *) 

! Make Re / Cplx builds easier
#ifdef __CMPLX
#ifdef __PROG_NUMRUNS
#define ARR_RE_OR_CPLX(arr,index) cmplx(arr(2*index-1), arr(2*index), dp)
#else
#define ARR_RE_OR_CPLX(arr,index) cmplx(arr(1), arr(2), dp)
#endif
#elif defined(__DOUBLERUN)
#define ARR_RE_OR_CPLX(arr,index) real(arr(index), dp)
#elif defined(__PROG_NUMRUNS)
#define ARR_RE_OR_CPLX(arr,index) real(arr(index), dp)
#else
#define ARR_RE_OR_CPLX(arr,index) real(arr(1), dp)
#endif

#ifdef __CMPLX
! 1->1 ,2->1, 3->2 ...
#define part_type_to_run(pt) (1+((pt)-1)/2)
#ifdef __PROG_NUMRUNS
#define min_part_type(run) (2*(run)-1)
#define max_part_type(run) (2*(run))
#define mag_of_run(signs, run) (signs(2*(run)-1)**2 + signs(2*(run))**2)**5e-1_dp
#define is_run_unnocc(signs, run) (signs(2*(run)-1)**2 + signs(2*(run))**2)**5e-1_dp <1.0e-12_dp 
#else
#ifdef __DOUBLERUN
#define min_part_type(run) (2*(run)-1)
#define max_part_type(run) (2*(run))
#define mag_of_run(signs, run) (signs(2*(run)-1)**2 + signs(2*(run))**2)**5e-1_dp
#define is_run_unnocc(signs, run) (signs(2*(run)-1)**2 + signs(2*(run))**2)**5e-1_dp <1.0e-12_dp 
#else
#define min_part_type(run) 1
#define max_part_type(run) 2
#define mag_of_run(signs, run) (signs(1)**2 + signs(1)**2)**5e-1_dp
#define is_run_unnocc(signs, run) (signs(1)**2 + signs(1)**2)**5e-1_dp <1.0e-12_dp 
#endif
#endif
#else
! 1->1 ,2->2, 3->3 ...
#define part_type_to_run(pt) pt
#ifdef __PROG_NUMRUNS
#define min_part_type(run) run
#define max_part_type(run) run
#else
#ifdef __DOUBLERUN
#define min_part_type(run) run
#define max_part_type(run) run
#else
#define min_part_type(run) 1
#define max_part_type(run) 1
#endif
#endif
#define mag_of_run(signs, run) abs(signs(run)) 
#define is_run_unnocc(signs, run) abs(signs(run))<1.0e-12_dp 
#endif



! Define types for C pointers to work between various compilers with
! differing levels of brokenness.
#if defined(__PATHSCALE__) || defined(__ISO_C_HACK) || defined(__OPEN64__) || defined(NAGF95)
#define loc_neci loc
#ifdef POINTER8
#define c_ptr_t integer(int64)
#else
#define c_ptr_t integer(int32)
#endif
#elif defined(__GFORTRAN__)
#define c_ptr_t type(c_ptr)
#define loc_neci g_loc
#else
#define c_ptr_t type(c_ptr)
#define loc_neci c_loc
#endif

! ***** HACK *****
! gfortran was playing up using a parameter defined to equal C_NULL_PTR
! --> use pre-processor defines instead!
#ifdef CBINDMPI
#if defined(__PATHSCALE__) || defined(__ISO_C_HACK) || defined(__OPEN64__)
#ifdef POINTER8
#define MPI_IN_PLACE (0_int64)
#else
#define MPI_IN_PLACE (0_int32)
#endif
#else
#define MPI_IN_PLACE (C_NULL_PTR)
#endif
#endif

#define debug_line(unit, msg) write(unit,*) __LINE__, __FILE__, char(9), msg ; flush(unit)
#define debug_out(unit, msg) write(unit,*), char(9), msg

