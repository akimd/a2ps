(**************************************************************************)
(* Creation and manipulation of VRML objects                              *)
(**************************************************************************)

#open "VRcaML";;
#open "default";;
#open "exceptions";;
#open "basic_types";;
#open "font";;
#open "fonts";;
#open "colors";;




(**************************************************************************)
(* Creation of VRML objects                                               *)
(**************************************************************************)

(* New empty referential **************************************************)

(* new_referential: vrml_object *)
let new_referential = default_vrml_object ;;


(* New VRML object ********************************************************)

(* new_vrml_object :
 vrml_positionable list -> vrml_object list -> vect3D -> rotation
-> vect3D -> frame -> bool -> anchor -> billboard -> collision
-> vrml_object *)
let new_vrml_object objects children position rotation scale
                    frame unique anchor billboard collision =
    { objects   = objects ;
      children  = children ;
      position  = position ;
      rotation  = rotation ;
      scale     = scale ;
      frame     = frame ;
      unique    = unique ;
      anchor    = anchor ;
      billboard = billboard ;
      collision = collision }
;;


(**************************************************************************)
(* Setting fields                                                         *)
(**************************************************************************)

(* set_objects : vrml_object -> vrml_positionable list -> vrml_object *)
let setf_objects object objects =
    new_vrml_object objects object.children object.position
                    object.rotation object.scale object.frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

let set_objects = setf_objects ;;

(* set_children : vrml_object -> vrml_object list -> vrml_object *)
let setf_children object children =
    new_vrml_object object.objects children object.position
                    object.rotation object.scale object.frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

let set_children = setf_children ;;

(* setf_position : vrml_object -> float -> float -> float -> vrml_object *)
let setf_position object position =
    new_vrml_object object.objects object.children position
                    object.rotation object.scale object.frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

(* setf_rotation :
   vrml_object -> float -> float -> float -> float -> vrml_object *)
let setf_rotation object rotation =
    new_vrml_object object.objects object.children object.position
                    rotation object.scale object.frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

(* setf_scale : vrml_object -> float -> float -> float -> vrml_object *)
let setf_scale object scale =
    new_vrml_object object.objects object.children object.position
                    object.rotation scale object.frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

(* setf_frame : vrml_object -> frame -> vrml_object *)
let setf_frame object frame =
    new_vrml_object object.objects object.children object.position
                    object.rotation object.scale frame
                    object.unique object.anchor object.billboard
                    object.collision
;;

let set_frame = setf_frame ;;

(* setf_unique : vrml_object -> bool -> vrml_object *)
let setf_unique object unique =
    new_vrml_object object.objects object.children object.position
                    object.rotation object.scale object.frame
                    unique object.anchor object.billboard
                    object.collision
;;

let set_unique = setf_unique ;;

(* setf_anchor : vrml_object -> anchor -> vrml_object *)
let setf_anchor object anchor =
    new_vrml_object object.objects object.children object.position
                    object.rotation object.scale object.frame
                    object.unique anchor object.billboard
                    object.collision
;;

(* setf_billboard : vrml_object -> billboard -> vrml_object *)
let setf_billboard object billboard =
    new_vrml_object object.objects object.children object.position
                    object.rotation object.scale object.frame
                    object.unique object.anchor billboard
                    object.collision
;;

(* setf_collision : vrml_object -> collision -> vrml_object *)
let setf_collision object collision =
    new_vrml_object object.objects object.children object.position
                    object.rotation object.scale object.frame
                    object.unique object.anchor object.billboard
                    collision
;;
