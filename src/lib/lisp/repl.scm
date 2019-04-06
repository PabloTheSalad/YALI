(define (repl)
    (display "What's your name: ")
    (define name (read))
    (if (not (= name 'quit))
        (begin (display "Hello ") 
               (display name)
               repl)
        (display "Goodnye")))
    
