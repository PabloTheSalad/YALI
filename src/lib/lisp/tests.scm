(test "hygienic macro" 
    (let ((it #f)) 
        (let ((t 'okay)) 
            (orr it t)))
    'okay)

(test "2 + 2 == 4"
    (+ 2 2)
    4)
    
(test "1/2 + 1/2 == 1"
    (+ 1/2 1/2)
    1)
    
(test "1/2 > 1/3"
    (> 1/2 1/3)
    #t)
    
(test "let test 1"
    (let ((x 10) (y 2)) 
         (+ x y))
    12)
    
(test "let test 2"
    (let ((x 10)) 
         (let ((y 2)) 
              (+ x y)))
    12)
    
(test "factorial test"
    (= (cfact 25) (rfact 25))
    #t)

(test "define test"
    (begin (define x 10) x)
    10)
    
(test "define id test"
    (begin (define (id x) x) (id 10))
    10)
    
(test "define clojure"
    (begin (define (x) 
                   (define y 10) 
                   (lambda () y)) 
           ((x)))
    10)
    
(test "quote test"
    '(1 2 3)
    (list 1 2 3))
    
(test "vector-ref test"
    (vector-ref #(1 2 3) 1)
    2)
    
(test "vector-set! test"
    (begin (define vec #(1 2 3 4)) 
           (vector-set! vec 2 'okay) 
           vec)
    #(1 2 okay 4))
