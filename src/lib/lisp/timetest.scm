(define (cfact x)
    (do ((x (+ x 1) (- x 1)) 
         (acc 1 (* acc x)))
        ((= x 1) acc)))

(define res '())

(do ((x 100 (- x 1)))
    ((= x 0) res)
    (set! res (cfact 1000.0)))
    
(display "Result: ") (display res) (newline)
