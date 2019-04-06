(define (make-complex r i)
    (define (sum x)
        (make-complex (+ (x 'real) r) (+ (x 'image) i)))
    (define (sub x)
        (make-complex (- r (x 'real)) (- i (x 'image))))
    (define (mul x)
        (let ((a r) (b i)
              (c (x 'real)) (d (x 'image)))
             (make-complex (- (* a c) (* b d))
                           (+ (* b c) (* a d)))))
    (define (div x)
        (let* ((a r) (b i)
              (c (x 'real)) (d (x 'image))
              (cd (+ (expt c 2) (expt d 2))))
             (make-complex (/ (+ (* a c) (* b d)) cd) 
                           (/ (- (* b c) (* a d)) cd))))
    (define (set-real x) (set! r x))
    (define (set-image x) (set! i x))
    (define (to-pair) (cons r i))
    (define (dispatch m)
        (cond ((eq? m 'type) 'complex) ; получение метки типа
              ((eq? m 'real) r) ; получение вещественной части
              ((eq? m 'image) i) ; получение мнимой части
              ((eq? m 'sum) sum) ; сложение
              ((eq? m 'sub) sub) ; вычитание
              ((eq? m 'mul) mul) ; умножение
              ((eq? m 'div) div) ; деление
              ((eq? m 'set-real) set-real) ; изменение вещественной части
              ((eq? m 'set-image) set-image) ; изменение мнимой части
              ((eq? m 'to-pair) (to-pair)) ; преобразование в пару значений
              (else (error "COMPLEX don't understand message" m)))) 
              ; вывод ошибки если команда не найдена
    dispatch)
    
(define C1 (make-complex 5 7)) ; 5 + 7i
(define C2 (make-complex 8 8)) ; 8 + 8i

(define C3 ((C1 'sum) C2)) ; Пример сложения комплексных чисел
(define C4 ((C1 'div) C2)) ; Пример деления комплексных чисел
