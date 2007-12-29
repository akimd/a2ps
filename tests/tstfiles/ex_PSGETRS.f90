! Gilles GRASSEAU (CNRS/IDRIS - France) <Gilles.Grasseau@idris.fr> - Nov. 1996

program test_psgetrs
!-------------------------------------------------------------------
! Description : 
!       Exemple de factorisation LU puis de resolution du systeme : 
!        A * x = b ,  ou  A est une matrice (5,5). 
!
! Principe :
!       Les matrice/vecteurs A, x et b sont globaux et les matrices/ 
!       vecteurs dist_a, dist_x et dist_b sont respectivement les   
!       matrices/vecteurs distribuees sur la grille de proc. 2x2. 
!       La repartition de la matrice se fait par blocs cycliques 2x2 
!       (2 elements par ligne et 2 elements par colonne - 
!       voir Exemple de repartition d'une matrice). 
!       La repartition des vecteurs se fait par blocs cycliques 2x1 
!       (le processeur logique (0,0) possede les blocs {x(1:2) ,x(5)} 
!       et le processeur logique (1,0) possede le bloc  x(3:4) ).  
!
! Algorithme :
!   1 - Initialisation du BLACS et autres.
!   2 - Distribution de la matrice A et du vecteur x vers les blocs  
!       locaux dist_a et dist_x de chaque processeur logique.
!       (voir Distribution de matrice) 
!       On a simule la situation ou seul le processeur (0,0) possede  
!       la matrice et les vecteurs globaux A, x et b. 
!   3 - Factorisation LU et resolution.
!   4 - Recuperation et impression des resultats par le processeur
!       logique (0,0).
!
! Remarque :
!   1 - Le tableau de travail 'work' a ete dimensionne a la valeur
!       donnee par INITBUFF pour psgetrs ( > au buffer de psgetrf).
!   2 - La liste des pivots locaux 'dist_piv' est dimensionnee a 
!       'n_max' + 'rb_size'.
!
!-------------------------------------------------------------------
  implicit none

  integer       :: pe, npes       ! Identificateur du processeur et
                                  ! nombre de processeurs physiques.

  integer, parameter:: nprow=2, npcol=2 ! Grille de proc. logiques.
  integer           :: prow, pcol       ! Coord. de mon proc. logique

  integer :: icntxt                      ! Contexte (grille de procs)
  integer, dimension(8) ::  desc_a       ! Descripteur de la matrice.
  integer, dimension(8) ::  desc_x       ! Descripteur des vecteurs.

  integer, parameter   :: n=5            ! Ordre  matrices/vecteurs.
  real, dimension(n,n) :: a              ! Matrice globale a.
  real, dimension(n) :: x                ! Vecteur resultat x(global)
  real, dimension(n) :: b                ! Vecteur b (global)

  integer, parameter :: n_max=3          ! Dim. des matrices/vecteurs
                                         ! locaux.
  real, dimension(n_max,n_max):: dist_a  ! Matrice locale.
  real, dimension(n_max)      :: dist_x  ! Vecteur local.
  real, dimension(n_max)      :: dist_b  ! Vecteur local.

  integer, parameter :: rb_size=2,   &   ! Taille des blocs lignes 
                        cb_size=2        ! et colonnes.

  integer, dimension(n_max+rb_size):: dist_piv ! Vecteur local des
                                               ! pivots.

  integer :: info, i, j

  data a/ 0.5, 0.0, 2.3, 0.0,-2.6,  & ! Coefficients de la matrice 
          0.0, 0.5,-1.4, 0.0,-0.7,  & ! globale A.
          2.3,-1.4, 0.5, 0.0, 0.0,  &
          0.0, 0.0, 0.0, 1.0, 0.0,  &
         -2.6,-0.7, 0.0, 0.0, 0.5/  

  data b/-2.1, 0.3, 1.5, 0.0,-1.5/    ! Coefficients du vecteur global


  integer,parameter:: minb=rb_size, maxb=minb ! Calcul de l'espace 
  integer,parameter:: minp=nprow, maxd=n      ! de travail
  integer,parameter:: wsize=2*maxb*(((maxd/minb)/minp)*maxb+maxb)*8
  real, dimension(wsize/8) ::  work

! Initialisation BLACS et autres.
!--------------------------------------
  call initbuff(work,wsize)   ! Init. des buffers internes au PBLAS
  call blacs_pinfo(pe, npes)  ! Init. mon PE, nbre de procs physiques
  call blacs_gridinit(icntxt, & ! Init. Grille de proc. logiques.
          'C', nprow, npcol)

  call blacs_gridinfo(icntxt, &       ! Mes coord. dans la grille 
           nprow, npcol, prow, pcol)  ! de processeurs logiques.

  call descinit(desc_a, n, n,       & ! Init. descripteur de la 
          rb_size, cb_size, 0, 0,   & ! matrice a
          icntxt, n_max, info)
  if (info.lt.0) stop 'descinit'

  call descinit(desc_x, n, 1,     & ! Init. descripteur du vecteur x
          rb_size, 1, 0, 0,       &
          icntxt, n_max, info)    &
  if (info.lt.0) stop 'descinit'

! Distribution de la matrice 'a' vers la matrice distribuee 'dist_a'.
!--------------------------------------------------------------------
  call distribue(icntxt, a, n, dist_a, n_max, prow, pcol)

! Distribution du vecteur 'b' sur le vecteur distribue 'dist_b'.
!------------------------------------------------------------------
  if ((prow==0).and.(pcol==0)) then
    dist_b(1:2) = b(1:2)             ! copie de b(1:2) dans proc(0,0)
    call sgesd2d(icntxt,2,1,       & ! b(3:5) envoie au proc (1,0)
                  b(3),n,1,0)            
    dist_b(3) = b(5)                 ! copie de b(5) dans proc (0,0)
  end if
  if ((prow==1).and.(pcol==0)) then
    call sgerv2d(icntxt, 2,1, dist_b(1), & ! reception b(3:5) dans 
                  n_max, 0, 0)             ! dist_b(1:2).
  end if

!  Calculs
!----------------------------------
  call PSGETRF(n, n, dist_a, 1, 1, desc_a,         &
               dist_piv, info)
  if (info /= 0) print *,'Erreur dans la factorisation : ',info

  call PSGETRS( 'N', n, 1, dist_a, 1, 1, desc_a,   &
              dist_piv, dist_b, 1, 1, desc_x, info)
  if (info /= 0) print *,'Erreur dans la resolution : ',info

!  Recuperation des resultats 'dist_b' -> 'x'
!----------------------------------------------
  if ((prow==1).and.(pcol==0)) then
    call sgesd2d(icntxt,2,1,dist_b(1),  & ! envoie de dist_b(1:2) 
                 n_max, 0, 0)             ! au proc (0,0)
  end if
  if ((prow==0).and.(pcol==0)) then
    x(1:2) = dist_b(1:2)        ! copie de dist_b(1:2) dans proc(0,0)
    call sgerv2d(icntxt,2,1,  & ! reception dist_b(1:2) dans x(3:4)
                 x(3), n, 1, 0)
    x(5) = dist_b(3)            ! copie de dist_b(3) dans proc (0,0)
  end if

! Impression des resultats
!-----------------------------
  if ((prow==0).and.(pcol==0)) then
    write(6,*) ' resultats :'
    write(6,'(1x,5F5.1)') (x(j),j=1,n)
  end if

end program

!--------------------------------------------------------------------
! Distribution de la matrice 'x' vers la matrice distribuee 'dist_x'.
!--------------------------------------------------------------------
subroutine distribue( icntxt, x, n, dist_x, n_max, prow, pcol)
  implicit none
  integer :: icntxt                    ! Contexte (grille de procs)
  integer :: n                         ! Ordre de la matrice X
  integer :: n_max                     ! Ordre de la matrice locale.
  real,dimension(n,n):: x              ! Matrice globale X.
  real,dimension(n_max,n_max):: dist_x ! Matrice locale.
  integer            :: prow, pcol     ! Coord. de mon proc. logique

  if ((prow == 0) .and. (pcol == 0)) then       ! Emission par le 
                                                ! processeur (0,0)
    call sgesd2d( icntxt, 2,2, x(1,3), n, 0, 1)
    call sgesd2d( icntxt, 2,2, x(3,1), n, 1, 0)
    call sgesd2d( icntxt, 2,2, x(3,3), n, 1, 1)
    call sgesd2d( icntxt, 2,1, x(3,5), n, 1, 0)
    call sgesd2d( icntxt, 1,2, x(5,3), n, 0, 1)
  end if

  if ((prow == 0) .and. (pcol == 0)) then  ! Copie des blocs 
    dist_x(1:2,1:2) = x(1:2,1:2)           ! appartenant au proc(0,0)
    dist_x(1:2,3) = x(1:2,5)
    dist_x(3,1:2) = x(5,1:2)
    dist_x(3,3) = x(5,5)
  end if
                                            ! Reception par les 
  if ((prow == 0) .and. (pcol == 1)) then   ! autres processeurs.
    call SGERV2D( icntxt, 2,2, dist_x(1,1), n_max, 0, 0)
    call SGERV2D( icntxt, 1,2, dist_x(3,1), n_max, 0, 0)
  end if
  if ((prow == 1) .and. (pcol == 0)) then
    call SGERV2D( icntxt, 2,2, dist_x(1,1), n_max, 0, 0)
    call SGERV2D( icntxt, 2,1, dist_x(1,3), n_max, 0, 0)
  end if
  if ((prow == 1) .and. (pcol == 1)) then
    call SGERV2D( icntxt, 2,2, dist_x(1,1), n_max, 0, 0)
  end if

end subroutine distribue
