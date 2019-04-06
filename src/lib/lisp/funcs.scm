(define (rfact x)
    (if (< x 2) 1 (* x (rfact (- x 1)))))

(define (cfact x)
    (do ((x (+ x 1) (- x 1)) 
         (acc 1 (* acc x)))
        ((= x 1) acc)))
    
(define (zero? n) (= n 0))
(define (positive? n) (> n 0))
(define (negative? n) (< n 0))

(define (sqrt x) (expt x 1/2))
          
(define (print . vals)
    (define (loop args)
        (cond ((null? args) (newline))
          ((null? (cdr args)) (display (car args)) (newline))
          (else (display (car args)) (loop (cdr args)))))
    (loop vals))
