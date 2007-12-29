#!/usr/pd/bin/runhugs
{- A sample Haskell file -}
module Test
where
import qualified Monad
import Prelude hiding (zip3)

{-#specialise qsort :: [String]->[String] #-} -- a pragma

-- Quicksort
qsort :: Ord a => [a] -> [a]     -- type annotation
qsort []     = []
qsort (x:xs) = qsort elts_lt_x ++ [x] ++ qsort elts_greq_x
                 where
                   elts_lt_x   = [y | y <- xs, y < x]
                   elts_greq_x = [y | y <- xs, y >= x]

-- Fibbonacci sequence
fib             = 1 : 1 : [ a+b | (a,b) <- zip fib (tail fib) ]

-- Binary tree
data Tree a = Branch (Tree a) (Tree a) | Leaf a

-- State monad from "http://haskell.org/tutorial/monads.html"
-- note that ">>=" should not be prited as ">\geq"
data S = Tree String
data SM a = SM (S -> (a,S))  -- The monadic type
instance Monad SM where
  -- defines state propagation
  SM c1 >>= fc2         =  SM (\s0 -> let (r,s1) = c1 s0
                                          SM c2 = fc2 r in
                                         c2 s1)
  return k              =  SM (\s -> (k,s))

leap = \n -> ( (n `mod` 4 == 0)&&(n `mod` 100 > 0) || (n `mod` 400 == 0))

a<->b = a ++ "<->" ++ b   -- <-> is a new binary operator

infixr 7 <->    -- infix declaration can be anywhere in the module

-- dark corners of Haskell syntax are not  pretty-printed correctly:
ifå' = True     -- characters outside ASCII are allowed,
(<=«) :: String->String->String -- <=« is a valid operator name,
a <=« b = a++"<=<<"++b
a |-- b = a++"|--"++b  -- operator names can even inlude double dash,
                       -- but many compilers will confuse it with a comment
data Pair = String ::: String  -- ::: is a valid constructor
(x,hiding,qualified) = (0,0,0)       -- not actually reserved words
string_with_gaps = "Hel\  \lo,\HTwo\
      \rld"++['\33']                 -- the value is "Hello,\tworld!"
