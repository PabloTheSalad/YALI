(define-syntax let ; определение макроса с именем let
  (lambda (x)
    (syntax-case x ()
      ((_ ((i v) ...) body body* ...)
       (ids? (syntax (i ...)))
       (syntax ((lambda (i ...) body body* ...) v ...)))
      ((_ name ((i v) ...) body body* ...)
       (ids? (syntax (i ...)))
       (syntax (begin (define (name) body body* ...) 
                      ((lambda (i ...) body body* ...) v ...)))))))
                      
;Примеры использования макроса
                      
(let ((x 1) (y 2)) (+ x y)) ; --> 3

(let ((a 1) (b 2)
      (c 3) (d 4))
     (+ (* a b) (* c d))) ; --> 14

(define-syntax my-if ; определение макроса my-if
    (syntax-rules (then else) ; определение ключевых слов макроса
        ((_ p then t else e) (if p t e)))) ; тело макроса
        
(my-if (> 2 3) then 1 else 2) ; пример использования
