(define-syntax syntax-rules
  (lambdaSF@YALI (x)
    (syntax-case x ()
      ((_ (i ...) ((keyword . pattern) template) ...)
       (syntax (lambdaSF@YALI (x)
                 (syntax-case x (i ...)
                   ((_ . pattern) (syntax template))
                   ...)))))))

(defineSF@YALI ids?
      (lambdaSF@YALI (ls)
        (or (null? ls)
            (and (identifier? (car ls))
                 (ids? (cdr ls))))))

(define-syntax lambda
    (lambdaSF@YALI (x)
        (syntax-case x ()
            ((_ args body body* ...)
             (or (identifier? (syntax args)) (ids? (syntax args)))
             (syntax (lambdaSF@YALI args (evlis body body* ...)))))))

(define-syntax define
    (lambda (x)
        (syntax-case x ()
            ((_ (name . args) body body* ...)
             (identifier? (syntax name))
             (syntax (defineSF@YALI name (lambda args body body* ...))))
            ((_ name value)
             (identifier? (syntax name))
             (syntax (defineSF@YALI name value))))))

(define-syntax if
    (syntax-rules ()
        ((_ p then else) (ifSF@YALI p then else))
        ((_ p then) (ifSF@YALI p then '()))))

(define-syntax let
  (lambda (x)
    (syntax-case x ()
      ((_ ((i v) ...) body body* ...)
       (ids? (syntax (i ...)))
       (syntax ((lambda (i ...) body body* ...) v ...)))
      ((_ name ((i v) ...) body body* ...)
       (ids? (syntax (i ...)))
       (syntax (begin (define (name) body body* ...) 
                      ((lambda (i ...) body body* ...) v ...)))))))

(define-syntax let*
  (syntax-rules ()
    ((_ () e1 e2 ...) (let () e1 e2 ...))
    ((_ ((i1 v1) (i2 v2) ...) e1 e2 ...)
     (let ((i1 v1))
       (let* ((i2 v2) ...) e1 e2 ...)))))

(define-syntax defmacro
    (syntax-rules ()
        ((_ name (i ...) body body* ...)
         (define-syntax name
            (syntax-rules ()
                ((_ i ...)
                 (begin body body* ...)))))))

(define-syntax cond
  (syntax-rules (else)
    ((_ (else e1 e2 ...)) (begin e1 e2 ...))
    ((_ (e0 e1 e2 ...)) (if e0 (begin e1 e2 ...)))
    ((_ (e0 e1 e2 ...) c1 c2 ...)
     (if e0 (begin e1 e2 ...) (cond c1 c2 ...)))))

(define-syntax when
  (syntax-rules ()
    ((_ test e e* ...)
     (if test (begin e e* ...)))))

;(define-syntax delay
;    (syntax-rules ()
;        ((_ x) (lambda () x))))

;(define-syntax force
;    (syntax-rules ()
;        ((_ x) (x))))

(define (list . l) l)

(define-syntax orr
  (syntax-rules ()
    ((_) #f)
    ((_ e) e)
    ((_ e1 e2 e3 ...)
     (let ((t e1)) (if t t (orr e2 e3 ...))))))

(define-syntax let-syntax
    (syntax-rules ()
        ((_ ((i trans) ...) body body* ...)
         (begin (define-syntax i trans) ... body body* ...))))
