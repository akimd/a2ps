//------------------------------------------------------------------+
//   MARIE                                         Yves Caseau      |
//   typing.cl                                                      |
//                                                                  |
//   Moteur Algebrique pour la Resolution d'Inferences Elementaires |
//   Copyright (C) 1986-97  by Y. CASEAU. All Rights Reserved       |
//------------------------------------------------------------------+

//-------------------------------------------------------------------
//This file contains:
//a lot of useful stuff for manipulating algebraic terms
//-------------------------------------------------------------------

//*******************************************************************
//*               Part 1:  get_range/get_domain                     *
//*               Part 2:  Utility methods for compilation          *
//*               Part 3:  Inversion of Properties                  *
//*******************************************************************

//*******************************************************************
//*               Part 1:  get_range/get_domain                     *
//*******************************************************************

// a small range inference algorithm, to improve the system
//
get_range(self:abstract_relation,x:type) : type
 -> (case self
      (phi x, 
       composition get_range(self.r1,
                                         get_range(self.r2, x)),
       ands get_range(self.r1, x) ^
                   get_range(self.r2, x),
       ors get_range(self.r1, x) U
                  get_range(self.r2, x),
       psi (if (self.op % class) self.op
                 else extract_range(self.op,
                                    list(get_range(self.r1, x),
                                         get_range(self.r2, x)))),
       array (if self.multivalued? member(self.range) else self.range),
       property (if (self = Id) x else extract_range(self, list(x))), 
       product self.ens, 
       annotation get_range(self.v.definition, x),
       connotation get_range(self.r, x),
       denotation (if unknown?(definition, self.v)
                           self.v.range
                        else get_range(self.v.definition,
                                             x)),
       property_inverse 
         get_domain(self.r, x),
       any error("error: range is not defined for ~S(~S)", self,
                 owner(self))))  

// a similar domain inference algorithm
//
get_domain(self:abstract_relation,x:type) : type
 -> (case self
      (phi x, 
       composition get_domain(self.r2,
                                          get_domain(self.r1,
                                                           x)),
       ands get_domain(self.r1, x) ^ 
                   get_domain(self.r2, x),
       ors get_domain(self.r1, x) U
                  get_domain(self.r2, x),
       property (if (self = Id) x
                else let y := {} in 
                       (for r in self.restrictions 
                          (if (self.multivalued? | x ^ r.range)
                              y :U domain!(r)),
                        y)),
       property_inverse 
         get_range(self.r, x),
       product (if known?(BASE) BASE else any),
       psi (if inv?(self)
                    get_range(inverse!(self), x)
                 else domain!(self.r1) ^ domain!(self.r2)),
       psi get_domain(self.r1, x) ^
                  get_domain(self.r2, x),
       denotation get_domain(self.v.definition, x),
       any domain!(self)))

// we extend range_type to take the type of arguments into account.
//
extract_range(self:property,l:list) : type
 -> (let %first := l[1],
         %rest := (copy(l) << 1),
         prop := Optimize/restriction!(self, l),
         %res := (case prop (restriction prop.range, any self.range)) in 
       (if self.multivalued? member(%res) else %res))  
extract_range(self:property_operator,l:list) : type
 -> extract_range(self.op, l)  

// A small range inference algorithm, to improve the system.
// (x X y) is the signature of the input pair
//
get_range(self:derivative,x:type,y:type) : type
 -> (case self
      (Tone y, 
       Tinv x,
       Tphi get_domain(self.e1, x, y),
       compr get_range(self.r1,
                                   get_range(self.e2, x, y)),
       compl get_range(self.e1, x, y),
       Tand get_range(self.e1, x, y) ^
                   get_range(self.r2, any),
       Tunion get_range(self.e1, x, y) U
                     get_range(self.e2, x, y),
       Tpsi extract_range(self.op,
                                list(get_range(self.e1, x, y),
                                     get_range(self.r2,
                                                     get_domain(self.e1, x, y)))),
       Tannotation2
         get_range(self.e1, x, y),
       Tannotation1
         get_range(self.v.definition, any),
       Tconnotation2
         get_range(self.r,
                         get_domain(self.e1, x, y)),
       Tconnotation1
         get_range(self.e1, x, y),
       Tif get_range(self.r1,
                                 get_domain(self.e1, x, y)) U
                  get_range(self.r2,
                                  get_domain(self.e1, x, y)),
       Tdenotation get_range(self.e1, x, y),
       any error("error: get_range is not defined for ~S (~S)", self,
                 owner(self))))  

// A small domain inference algorithm, to improve the system.
// (x X y) is the signature of the input pair
//
get_domain(self:derivative,x:type,y:type) : type
 -> (case self
      (Tone x, 
       Tinv y,
       Tphi get_domain(self.e1, x, y),
       compr get_domain(self.e2, x, y),
       compl get_domain(self.r2,
                                    get_domain(self.e1, x, y)),
       Tand get_domain(self.e1, x, y) ^
                   get_domain(self.r2, any),
       Tunion get_domain(self.e1, x, y) U
                     get_domain(self.e2, x, y),
       Tpsi get_domain(self.e1, x, y) ^
                   get_domain(self.r2, any),
       Tannotation2
         get_domain(self.e1, x, y),
       Tannotation1
         get_domain(self.e1, x, y) ^
           get_domain(self.v.definition, any),
       Tconnotation2
         get_domain(self.v.definition, any),
       Tconnotation1
         get_domain(self.e1, x, y),
       Tdenotation get_domain(self.e1, x, y),
       Tif get_domain(self.e1, x, y),
       any error("error: get_domain is not defined for ~S (~S)", self,
                 owner(self))))  

//*******************************************************************
//*               Part 2:  Utility methods for compilation          *
//*******************************************************************
// a protected substitution
//
Logic/cpsubst(self:any,x:any,y:any) : any
 -> substitution(instruction_copy(self), x, y)

// instruction copy with fresh local variables
//
Logic/cpfresh(self:any) : any
 -> (let l := bound_variables(self),
         x := instruction_copy(self) in 
       (for y in l x := substitution(x, y, Variable(pname = y.pname)), x))  

// make a copy with fresh variable + a substitution
//
cpfresh(self:any,x1:Variable,x2:Variable) : any
 -> substitution(cpfresh(self), x1, x2)

// make a new two variable conclusion
//
cpfresh(self:any,x1:Variable,x2:Variable,y1:Variable,y2:Variable) : any
 -> substitution(substitution(cpfresh(self), x1, x2), y1, y2)

// we create so many binary messages ...
//
Logic/message!(x:any,p:property,y:any) : Call -> Call(p, list(x, y))

Logic/make_a_and(self:list) : any
 -> (if (size(self) = 1) self[1] else And(args = self))

//*******************************************************************
//*               Part 3:  Inversion of Properties                  *
//*******************************************************************
// now we van define the inversion of a psi ------------------------
//
// a psi term is usually non inversible.
// two exceptions are interesting:
//   - the second subterm is a constant (a constant is always placed
//     in the second place) and we have an addition or a multiplication
//   - the two subterm are inversible and the operation is a mapping
//
inv?(self:psi) : boolean
 -> (let %d := description[self.op] in
       ((%d = group_operation &
         (inv?(self.r1) & self.r2 % product)) | 
        ((%d = monoid |
          (%d = binary_operation & 
           known?(ternary_inverse_of[self.op]))) & 
         ((inv?(self.r1) & const?(self.r2)) |
          (inv?(self.r2) & const?(self.r1))))))

// this is only called if the psi is inversible
//
inverse!(self:psi) : any
 -> (let %o := self.op,
         t1 := self.r1,
         t2 := self.r2 in
       (if (description[%o] = group_operation)
           compose(inverse!(t1), 
                         psify(%o, Id,
                                     compose(function_inverse[%o],
                                                   t2)))
        else if (description[%o] = monoid) 
           (if const?(t2) 
               compose(inverse!(t1),
                             compose(psify(ternary_inverse[%o],
                                                       Id, t2),
                                           phi(op = 
                                                       comparison_inverse[%o],
                                                     r1 = Id,
                                                     r2 = t2)))
            else compose(inverse!(t2),
                               compose(psify(ternary_inverse[%o],
                                                         Id, t1),
                                             phi(op = 
                                                         comparison_inverse[%o],
                                                       r1 = Id,
                                                       r2 = t1))))
        else if known?(ternary_inverse_of[%o])
           (if const?(t2)
               compose(inverse!(t1),
                             psify(ternary_inverse_of[%o],
                                         Id, t2))
            else compose(inverse!(t2), psify(%o, t1, Id))) 
        else if (description[%o] = mapping)
           ands(r1 = 
                        compose(inverse!(t1),
                                      projection1[%o]),
                      r2 =
                        compose(inverse!(t2),
                                      projection2[%o]))))

// check if a relation can be inversed without trouble
//
inv?(self:relation) : boolean
 -> (known?(inverse, self) |
     (case self 
       (relation finite?(self.domain), 
        property not({ r in self.restrictions | 
                       not(finite?(domain!(r)))}))))
inverse!(self:relation) : relation
 -> (let r1 := get(inverse, self) in
       case r1
        (relation r1,
         any property_inverse(r = self)))

// a constant expression is either a constant or an expression involving
// constants
//
const?(self:abstract_relation) : boolean
 -> (case self
      (constant true, 
       composition const?(self.r2),
       psi (const?(self.r1) &
                  const?(self.r2))))

//*******************************************************************
//*               Part 4:  Algebra Canonical Injection              *
//*******************************************************************

domain!(r:relation) : type -> r.domain

// a property is an abstract relation ----------------------------------
// "inversion" is straightforward, we create a message
//
inversion(self:property,x:any,y:Variable) : any
 -> (if self.multivalued?
        Call(selector = %, args = list(y, inversion!(self, x))) 
     else Call(selector = =, args = list(y, inversion!(self, x))))
inversion!(self:property,x:any) : any
 -> (if (self = Id) x else Call(selector = self, args = list(x)))

// an extensional relation
//
inversion(self:array,x:any,y:Variable) : any
 -> Call(selector = =, args = list(y, inversion!(self, x)))
inversion!(self:array,x:any) : any
 -> Call(selector = nth, args = list(self, x))

